//
// Created by ozzadar on 2025-11-30.
//

#include "lights/scene/resource_manager.h"

#include "lights/rendering/texture.h"

#include <deque>
#include <spdlog/spdlog.h>
#include <utility>

namespace OZZ::scene {
    ResourceManager::ResourceManager(Window* parentContextWindow)
        : contextWindow(std::make_unique<ContextWindow>(parentContextWindow)) {
        jobThread = std::jthread([this](const std::stop_token& tok) {
            run(tok);
        });
    }

    ResourceManager::~ResourceManager() {
        jobThread.request_stop();
        queueCondition.notify_all();
    }

    std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::filesystem::path& path) {
        auto newTexture = std::make_shared<Texture>();
        queueJob([newTexture = newTexture, path = std::move(path)] {
            const auto image = std::make_unique<Image>(path);
            newTexture->UploadData(image.get());
        });
        return newTexture;
    }

    std::shared_ptr<Texture> ResourceManager::LoadTexture(std::shared_ptr<Image> image) {
        auto newTexture = std::make_shared<Texture>();
        queueJob([newTexture = newTexture, image = std::move(image)] {
            newTexture->UploadData(image.get());
        });
        return newTexture;
    }

    void ResourceManager::queueJob(const std::function<void()>& job) {
        {
            auto queueLock = std::scoped_lock(queueMutex);
            queuedJobs.push_back(job);
        }
        queueCondition.notify_one();
    }

    void ResourceManager::run(const std::stop_token& tok) {
        if (!contextWindow) {
            spdlog::error("Resource manager provided invalid context window");
            return;
        }

        // we bind the context so that we can do opengl things on the different thread
        contextWindow->MakeContextCurrent();
        while (true) {
            std::function<void()> job;
            {
                auto queueLock = std::unique_lock(queueMutex);
                queueCondition.wait(queueLock, [&] {
                    return tok.stop_requested() || !queuedJobs.empty();
                });

                if (tok.stop_requested()) {
                    break;
                }

                job = std::move(queuedJobs.front());
                queuedJobs.pop_front();
            }
            job();
        }
    }

} // namespace OZZ::scene