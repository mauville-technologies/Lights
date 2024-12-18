//
// Created by ozzadar on 2024-12-17.
//

#include "server.h"
#include "lights/network/client_messages.h"

#include <spdlog/spdlog.h>

namespace OZZ {
    Server::Server(asio::io_context &inContext, short inPort)
            : context(inContext),
              workGuard(asio::make_work_guard(context.get_executor())),
              acceptor(inContext, tcp::endpoint(tcp::v4(), inPort)) {
        spdlog::info("Server started on port: {}", acceptor.local_endpoint().port());

        startAccept();
        // initialize thread pool
        for (auto i = 0; i < std::thread::hardware_concurrency(); i++) {
            threadPool.emplace_back([this, i] {
                spdlog::trace("Thread started {}", i);
                context.run();
                spdlog::trace("Thread stopping {}", i);
            });
        }
    }

    Server::~Server() {
        context.stop();
        for (auto &thread : threadPool) {
            thread.join();
        }
    }

    void Server::startAccept() {
        spdlog::info("Waiting for client to connect...");
        auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());

        acceptor.async_accept(*socket, [this, socket](std::error_code ec) {
            if (!ec) {
                spdlog::info("Client connected: {}", socket->remote_endpoint().address().to_string());

                auto newClient = std::make_shared<ConnectedClient>(socket);
                if (!OnNewClient) {
                    spdlog::error("OnNewClient not set");
                } else {
                    std::lock_guard<std::mutex> lock(newClient->ClientMutex);
                    OnNewClient(newClient);
                }
            } else {
                spdlog::error("Error accepting client: {}", ec.message());
            }

            // continue handling new connections
            startAccept();
        });
    }
} // OZZ