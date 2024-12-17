//
// Created by ozzadar on 2024-12-17.
//


#include <lights/lights.h>
#include <spdlog/spdlog.h>
#include <asio.hpp>

#include "network/server.h"

asio::io_context context;

// if ctrl+c is pressed, stop the context
void signal_handler(int signum) {
    spdlog::info("Interrupt signal ({}) received", signum);
    context.stop();
}

int main() {
    spdlog::info("Lights version: {}.{}", (int)VERSION_MAJOR, (int)VERSION_MINOR);

    try {
        // set up signal handler
        signal(SIGINT, signal_handler);

        OZZ::Server server(context, 8080);
        context.run();
    } catch (std::exception &e) {
        spdlog::error("Exception: {}", e.what());
    }

    return 0;
}