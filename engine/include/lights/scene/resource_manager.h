//
// Created by ozzadar on 2025-11-30.
//

#pragma once
#include "lights/platform/platform_window.h"
#include "lights/platform/window.h"
#include "lights/rendering/buffer.h"
#include "lights/util/image.h"

#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <thread>

namespace OZZ {
    class Texture;
}

namespace OZZ::scene {
    class ResourceManager {
    public:
        explicit ResourceManager();
        ~ResourceManager();

        std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path);

        std::shared_ptr<Texture> LoadTexture(std::shared_ptr<Image> image);

        /**
         * Runs the render jobs on the main thread
         */
        void Tick();

    private:
        void queueJob(const std::function<void()>& job);
        void run(const std::stop_token& tok);

    private:
        std::mutex queueMutex;
        std::deque<std::function<void()>> queuedJobs{};
        std::condition_variable queueCondition{};
        std::jthread jobThread;

        std::unique_ptr<GPUStagingBuffer> stagingBuffer;

        std::mutex renderJobsMutex;
        std::deque<std::function<void()>> renderJobs{};
    };

} // namespace OZZ::scene
