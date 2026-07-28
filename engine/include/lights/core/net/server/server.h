#pragma once

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

#include "reactor_pool.h"
#include "server_settings.h"

namespace OZZ::net::server {

    class ConnectionRegistry;

    namespace asio = boost::asio;
    using tcp      = asio::ip::tcp;

    // Self-contained server bootstrap: owns a dedicated single-thread accept io_context
    // (isolated from I/O so connection bursts don't compete with reads/writes) plus a
    // ReactorPool for accepted sockets. Constructed only via Create() -- the accept
    // handler captures shared_from_this(), which is UB unless owned by a shared_ptr.
    class Server : public std::enable_shared_from_this<Server> {
    public:
        static std::shared_ptr<Server> Create(const tcp::endpoint& endpoint,
                                              ConnectionRegistry& registry,
                                              ServerSettings settings = {});

        ~Server();

        // Starts the acceptor thread, the reactor pool, and the accept loop.
        void Start();

        // Idempotent. Stops accepting, stops the reactor pool, and joins the acceptor thread.
        void Stop();

        bool IsListening() const;

    private:
        Server(const tcp::endpoint& endpoint, ConnectionRegistry& registry, ServerSettings settings);

        void doAccept();
        void onAccept(boost::system::error_code ec, tcp::socket socket);

        asio::io_context acceptorIoc{1};
        // Without this, run() sees no work queued yet and returns immediately, leaving
        // the acceptor thread dead before doAccept() ever gets a chance to register work.
        std::optional<asio::executor_work_guard<asio::io_context::executor_type>> acceptorWorkGuard;
        std::thread acceptorThread;
        ReactorPool pool;
        std::vector<std::reference_wrapper<asio::io_context>> targetContexts;

        tcp::acceptor acceptor;
        std::size_t nextTarget{0};
        ConnectionRegistry& registry;
        bool running{false};
        // Set only once open/bind/listen all succeed -- acceptor.is_open() alone stays
        // true after open() even if the later bind() (e.g. port in use) fails.
        bool listening{false};
    };

} // namespace OZZ::net::server
