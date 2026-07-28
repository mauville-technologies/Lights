#ifndef __EMSCRIPTEN__

#include "lights/core/net/server/server.h"

#include "lights/core/net/server/connection_registry.h"

#include <boost/asio/ip/address.hpp>
#include <spdlog/spdlog.h>

namespace OZZ::net::server {

    Server::Server(const std::string& bindHost, uint16_t port, ConnectionRegistry& registry, ServerSettings settings)
        : pool(settings.reactorThreadCount)
        , acceptor(acceptorIoc)
        , registry(registry) {
        boost::system::error_code ec;

        const auto address = asio::ip::make_address(bindHost, ec);
        if (ec) {
            spdlog::error("Server: invalid bind address '{}': {}", bindHost, ec.message());
            return;
        }
        const tcp::endpoint endpoint{address, port};

        acceptor.open(endpoint.protocol(), ec);
        if (ec) {
            spdlog::error("Server: open failed: {}", ec.message());
            return;
        }

        acceptor.set_option(asio::socket_base::reuse_address(true), ec);
        if (ec) {
            spdlog::error("Server: set_option(reuse_address) failed: {}", ec.message());
            return;
        }

        acceptor.bind(endpoint, ec);
        if (ec) {
            spdlog::error("Server: bind failed: {}", ec.message());
            return;
        }

        acceptor.listen(asio::socket_base::max_listen_connections, ec);
        if (ec) {
            spdlog::error("Server: listen failed: {}", ec.message());
            return;
        }

        listening = true;
    }

    std::shared_ptr<Server> Server::Create(const std::string& bindHost, uint16_t port, ConnectionRegistry& registry, ServerSettings settings) {
        return std::shared_ptr<Server>(new Server(bindHost, port, registry, settings));
    }

    Server::~Server() { Stop(); }

    void Server::Start() {
        if (!listening) {
            spdlog::error("Server: not starting -- acceptor failed to open (see earlier error).");
            return;
        }
        if (running) return;
        running = true;

        acceptorWorkGuard.emplace(acceptorIoc.get_executor());
        pool.Start();
        targetContexts = pool.Contexts();
        acceptorThread = std::thread([this] { acceptorIoc.run(); });
        doAccept();
    }

    void Server::Stop() {
        if (!running) return;
        running = false;

        // Closing (not stop()-ing the io_context) aborts the pending async_accept, whose
        // handler then sees running == false and doesn't re-arm -- letting run() drain and
        // return on its own once the work guard below is released. This also releases the
        // shared_ptr captured by that pending accept handler, which would otherwise keep
        // this Server alive indefinitely (a self-referential leak via acceptorIoc).
        boost::system::error_code ec;
        acceptor.close(ec);
        acceptorWorkGuard.reset();

        if (acceptorThread.joinable()) {
            acceptorThread.join();
        }
        pool.Stop();
    }

    bool Server::IsListening() const { return listening; }

    void Server::doAccept() {
        asio::io_context& target = targetContexts[nextTarget].get();
        nextTarget = (nextTarget + 1) % targetContexts.size();

        acceptor.async_accept(target, [self = shared_from_this()](boost::system::error_code ec, tcp::socket socket) {
            self->onAccept(ec, std::move(socket));
        });
    }

    void Server::onAccept(boost::system::error_code ec, tcp::socket socket) {
        if (ec) {
            spdlog::error("Server: accept failed: {}", ec.message());
        } else {
            registry.NewConnection(std::move(socket));
        }

        if (running) {
            doAccept();
        }
    }

} // namespace OZZ::net::server

#endif // !__EMSCRIPTEN__
