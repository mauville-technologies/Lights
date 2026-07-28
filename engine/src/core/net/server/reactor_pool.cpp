#ifndef __EMSCRIPTEN__

#include "lights/core/net/server/reactor_pool.h"

#include <algorithm>

namespace OZZ::net::server {

    ReactorPool::ReactorPool(unsigned threadCount) {
        const unsigned count = threadCount == 0 ? std::max(1u, std::thread::hardware_concurrency()) : threadCount;
        contexts.reserve(count);
        for (unsigned i = 0; i < count; ++i) {
            contexts.push_back(std::make_unique<asio::io_context>());
        }
    }

    ReactorPool::~ReactorPool() { Stop(); }

    void ReactorPool::Start() {
        if (!threads.empty()) return;

        // Without a work guard, run() returns immediately (no work queued yet) and every
        // reactor thread exits before ever seeing a connection.
        workGuards.reserve(contexts.size());
        for (auto& ctx : contexts) {
            workGuards.push_back(asio::make_work_guard(*ctx));
        }

        threads.reserve(contexts.size());
        for (auto& ctx : contexts) {
            threads.emplace_back([&ctx] { ctx->run(); });
        }
    }

    void ReactorPool::Stop() {
        if (threads.empty()) return;

        workGuards.clear();
        for (auto& ctx : contexts) {
            ctx->stop();
        }
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
        threads.clear();
    }

    std::vector<std::reference_wrapper<asio::io_context>> ReactorPool::Contexts() {
        std::vector<std::reference_wrapper<asio::io_context>> refs;
        refs.reserve(contexts.size());
        for (auto& ctx : contexts) {
            refs.emplace_back(*ctx);
        }
        return refs;
    }

} // namespace OZZ::net::server

#endif // !__EMSCRIPTEN__
