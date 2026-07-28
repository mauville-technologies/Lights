#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "connection_handler.h"

namespace OZZ::net::server {

    // Builds the delegate for a freshly-accepted connection. Supplied by the app at
    // registry-construction time -- keeps the registry itself free of any app-specific
    // knowledge (databases, schemas, etc).
    using ConnectionFactory = std::function<std::shared_ptr<ConnectionDelegate>(ConnectionHandler&)>;

    // Connections live across N reactor threads, so the mutex guards concurrent access.
    class ConnectionRegistry {
    public:
        explicit ConnectionRegistry(ConnectionFactory factory);

        std::shared_ptr<ConnectionHandler> NewConnection(tcp::socket&& socket);
        std::shared_ptr<ConnectionHandler> GetConnection(uint64_t id);

    private:
        void connectionDropped(uint64_t id);

        ConnectionFactory factory;
        std::atomic<uint64_t> nextId{1};

        std::mutex mutex;
        std::unordered_map<uint64_t, std::shared_ptr<ConnectionHandler>> connections;
    };

} // namespace OZZ::net::server
