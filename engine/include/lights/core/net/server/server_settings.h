#pragma once

namespace OZZ::net::server {

    // Deliberately its own header with no Boost/Asio includes, so consumers can embed
    // this in a config struct without pulling networking headers into config code.
    struct ServerSettings {
        // 0 = std::thread::hardware_concurrency()
        unsigned reactorThreadCount{0};
    };

} // namespace OZZ::net::server
