//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <algorithm>
#include <utility>
#include "lights/network/client_messages.h"

namespace OZZ {

    using OnCloseHandler = std::function<void()>;
    using OnLoginRequestSignature = std::function<void(std::string, std::string)>;

    class ConnectedClient {
    public:
        explicit ConnectedClient(std::shared_ptr<asio::ip::tcp::socket> socket);
        ~ConnectedClient();

        void SendLoginResponse(bool success, const std::string& username);
        void LoggedInElsewhere();

    private:
        void read();
        void handleRead();

        template<typename T>
        void write(T message) {
            asio::error_code ec;
            socket->write_some(asio::buffer(std::vector<uint8_t>(message)), ec);

            if (ec) {
                spdlog::error("Error writing to client: {}", ec.message());
                OnClose();
            }
        };

        // close() is called from destructor. If we want to disconnect the client from elsewhere, we should call OnClose to trigger the destruction chain
        void close();
    public:
        OnCloseHandler OnClose;
        OnLoginRequestSignature OnLoginRequest;

        // TODO: This mutex is a dirty hack to properly handle the initialization sequence
        std::mutex ClientMutex;
    private:
        std::shared_ptr<asio::ip::tcp::socket> socket;
        ClientMessageType queuedMessageType;
    };

} // OZZ

