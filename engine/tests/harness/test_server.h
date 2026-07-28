#pragma once

#include "lights/core/net/server/connection_registry.h"
#include "lights/core/net/server/server.h"

#include <boost/asio/ip/address.hpp>

#include <memory>
#include <stdexcept>
#include <string>

namespace OZZ::net::server::testing {

    // Real Server on loopback, for exercising the connection framework end-to-end.
    // Multiple reactor threads so cross-thread Send()/Kill() posting gets exercised.
    class TestServer {
    public:
        explicit TestServer(ConnectionFactory factory) : registry(std::move(factory)) {
            constexpr int kBasePort = 19191;
            constexpr int kMaxTries = 20;

            for (int i = 0; i < kMaxTries; ++i) {
                port = kBasePort + i;
                auto candidate = Server::Create(
                    tcp::endpoint{asio::ip::make_address("127.0.0.1"), static_cast<unsigned short>(port)},
                    registry,
                    ServerSettings{.reactorThreadCount = 2});

                if (candidate->IsListening()) {
                    server = candidate;
                    break;
                }
            }

            if (!server) {
                throw std::runtime_error("TestServer: no port available in range starting at " + std::to_string(kBasePort));
            }

            server->Start();
        }

        ~TestServer() {
            if (server) server->Stop();
        }

        TestServer(const TestServer&) = delete;
        TestServer& operator=(const TestServer&) = delete;

        std::string url() const { return "ws://127.0.0.1:" + std::to_string(port); }

    private:
        ConnectionRegistry registry;
        int port{0};
        std::shared_ptr<Server> server;
    };

} // namespace OZZ::net::server::testing
