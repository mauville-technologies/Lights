//
// Created by ozzadar on 2024-12-17.
//

#include "server.h"
#include "lights/network/client_messages.h"

#include <spdlog/spdlog.h>

namespace OZZ {
    Server::Server(asio::io_context &inContext, short inPort)
            : context(inContext),
              acceptor(inContext, tcp::endpoint(tcp::v4(), inPort)) {
        spdlog::info("Server started on port: {}", acceptor.local_endpoint().port());
        startAccept();

        // initialize thread pool
        for (auto i = 0; i < std::thread::hardware_concurrency(); i++) {
            threadPool.emplace_back([this, i] {
                spdlog::info("Thread started {}", i);
                context.run();
                spdlog::info("Thread stopping {}", i);
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
        auto socket = std::make_shared<tcp::socket>(acceptor.get_executor());

        acceptor.async_accept(*socket, [this, socket](std::error_code ec) {
            if (!ec) {
                spdlog::info("Client connected: {}", socket->remote_endpoint().address().to_string());
                std::shared_ptr<ConnectedClient> newClient = clients.emplace_back(
                        std::make_shared<ConnectedClient>(socket, [this, &newClient](ConnectedClient *lostClient) {
                            spdlog::info("Client disconnected");
                            clientDisconnected(lostClient);
                        }));

                spdlog::info("Number of clients: {}", clients.size());
            } else {
                spdlog::error("Error accepting client: {}", ec.message());
            }

            // continue handling new connections
            startAccept();
        });
    }

    void Server::clientDisconnected(ConnectedClient *client) {
        std::erase_if(clients, [client](std::shared_ptr<ConnectedClient> &c) {
            return c.get() == client;
        });

        // print number of clients
        spdlog::info("Number of clients: {}", clients.size());
    }

} // OZZ