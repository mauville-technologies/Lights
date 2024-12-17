//
// Created by ozzadar on 2024-12-17.
//

#include <lights/lights.h>
#include <spdlog/spdlog.h>
#include <asio.hpp>
#include "client.h"

int main() {
    // Print library version
    spdlog::info("Lights version: {}.{}", (int)VERSION_MAJOR, (int)VERSION_MINOR);

    try {
        // Create an io_context object
        asio::io_context context;

        // Create a server object
        OZZ::Client client(context, "127.0.0.1", 8080);
        client.Run();
        context.run();
    } catch (std::exception &e) {
        spdlog::error("Exception: {}", e.what());
    }
    return 0;
}