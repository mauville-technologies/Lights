//
// Created by ozzadar on 2025-11-30.
//

#include "lights/scene/resource_manager.h"

#include "lights/rendering/texture.h"
#include "lights/util/memory_literals.h"

#include <deque>
#include <spdlog/spdlog.h>
#include <utility>

namespace OZZ::scene {
    ResourceManager::ResourceManager() {
        constexpr size_t bufferSizeInBytes = 256_MiB;
        stagingBuffer = std::make_unique<GPUStagingBuffer>(bufferSizeInBytes);
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
        queueJob([this, path, newTexture] {
            const auto info = Image::GetImageInfo(path);
            // get staging buffer slice
            const auto slice = stagingBuffer->GetSlice(info.SizeInBytes());
            auto ptr = stagingBuffer->GetAllocatedPointer(slice);
            const auto image = std::make_shared<Image>(ExternalBuffer, path, ptr);

            {
                std::lock_guard lock(renderJobsMutex);
                renderJobs.emplace_back([this, newTexture, image, slice]() {
                    stagingBuffer->Bind();
                    newTexture->UploadData(image.get(), slice.offset);
                    stagingBuffer->Unbind();

                    stagingBuffer->RegisterInFlight(
                        {slice.offset, slice.size, glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0), [newTexture]() {
                             newTexture->FinalizeUpload();
                         }});
                });
            }
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

    void ResourceManager::Tick() {
        stagingBuffer->Tick();
        std::deque<std::function<void()>> local;
        {
            std::lock_guard lock(renderJobsMutex);
            local.swap(renderJobs);
        }
        auto count = 0ULL;
        while (!local.empty()) {
            local.front()();
            local.pop_front();
            count++;

            if (count >= 5)
                break;
        }

        if (!local.empty()) {
            {
                auto queueLock = std::unique_lock(renderJobsMutex);
                // put back to front of queue
                renderJobs.insert(renderJobs.begin(), local.begin(), local.end());
            }
        }
    }

    void ResourceManager::queueJob(const std::function<void()>& job) {
        {
            auto queueLock = std::scoped_lock(queueMutex);
            queuedJobs.push_back(job);
        }
        queueCondition.notify_one();
    }

    void ResourceManager::run(const std::stop_token& tok) {
        // we bind the context so that we can do opengl things on the different thread
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