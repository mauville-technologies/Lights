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

        void SendLoginResponse(bool success);

    private:
        void read();
        void handleRead();
        void write();
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

