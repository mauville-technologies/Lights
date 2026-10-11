#ifndef __EMSCRIPTEN__

#include "lights/core/net/server/connection_registry.h"
#include "ozz_rendering/utils/enums.h"

#include <spdlog/spdlog.h>

namespace OZZ::net::server {

    ConnectionRegistry::ConnectionRegistry(ConnectionFactory factory)
        : factory(std::move(factory)) {}

    std::shared_ptr<ConnectionHandler> ConnectionRegistry::NewConnection(tcp::socket&& socket) {
        const uint64_t id = nextId.fetch_add(1, std::memory_order_relaxed);

        auto connection = std::make_shared<ConnectionHandler>(
            std::move(socket), from_index<ConnectionId>(id), [this](ConnectionId droppedId) {
                connectionDropped(droppedId);
            });
        connection->AttachDelegate(factory(*connection));

        {
            std::scoped_lock lock(mutex);
            connections.emplace(from_index<ConnectionId>(id), connection);
        }

        spdlog::info("New connection {}", id);
        connection->Run();
        return connection;
    }

    void ConnectionRegistry::connectionDropped(ConnectionId id) {
        std::scoped_lock lock(mutex);
        if (connections.erase(id) == 0) {
            spdlog::error("Failed to remove connection {} from connection registry.", to_index(id));
            return;
        }
        spdlog::info("Connection {} dropped", to_index(id));
    }

    std::shared_ptr<ConnectionHandler> ConnectionRegistry::GetConnection(ConnectionId id) {
        std::scoped_lock lock(mutex);
        auto it = connections.find(id);
        return it == connections.end() ? nullptr : it->second;
    }

} // namespace OZZ::net::server

#endif // !__EMSCRIPTEN__
