//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <algorithm>
#include "lights/network/client_messages.h"

namespace OZZ {

    using OnCloseHandler = std::function<void(class ConnectedClient*)>;

    class ConnectedClient {
    public:
        explicit ConnectedClient(std::shared_ptr<asio::ip::tcp::socket> socket, OnCloseHandler inCloseHandler = nullptr);
        ~ConnectedClient();
    private:
        void read();
        void handleRead();
        void write();
        void close();

        //Temp
        void ScheduleWrite();
    private:
        std::shared_ptr<asio::ip::tcp::socket> socket;

        ClientMessageType queuedMessageType;

        OnCloseHandler closeHandler;

        //Temp
        asio::steady_timer timer;
    };

} // OZZ

