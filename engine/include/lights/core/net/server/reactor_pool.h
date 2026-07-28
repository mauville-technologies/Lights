#pragma once

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>

#include <functional>
#include <memory>
#include <thread>
#include <vector>

namespace OZZ::net::server {

    namespace asio = boost::asio;

    // Owns N io_context worker threads (0 -> std::thread::hardware_concurrency()).
    // Start()/Stop() manage the threads; Contexts() is what Server round-robins across.
    class ReactorPool {
    public:
        explicit ReactorPool(unsigned threadCount = 0);
        ~ReactorPool();

        ReactorPool(const ReactorPool&) = delete;
        ReactorPool& operator=(const ReactorPool&) = delete;

        void Start();
        void Stop();

        std::vector<std::reference_wrapper<asio::io_context>> Contexts();

    private:
        std::vector<std::unique_ptr<asio::io_context>> contexts;
        std::vector<asio::executor_work_guard<asio::io_context::executor_type>> workGuards;
        std::vector<std::thread> threads;
    };

} // namespace OZZ::net::server
