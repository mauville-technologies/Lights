//
// Created by ozzadar on 2025-11-30.
//

#include "lights/scene/resource_manager.h"

#include "lights/rendering/texture.h"
#include "lights/util/memory_literals.h"

#include <deque>
#include <fstream>
#include <nlohmann/json.hpp>
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

    bool ResourceManager::LoadSpritesheet(const SpritesheetLocation& location) {
        const auto& [jsonPath, texturePath] = location;
        spdlog::info("Parsing spritesheet {}", jsonPath.string());
        // Load json file
        std::ifstream file(jsonPath.string());
        nlohmann::json data = nlohmann::json::parse(file);

        // get metadata
        if (!data.contains("meta")) {
            spdlog::error("No meta information in spritesheet");
            return false;
        }

        const auto meta = data.at("meta");
        if (!meta.contains("size")) {
            spdlog::error("No size meta information in spritesheet");
            return false;
        }

        const auto size = meta.at("size");
        if (!size.contains("w") || !size.contains("h")) {
            spdlog::error("No w/h meta information in spritesheet");
            return false;
        }

        const auto sheetWidth = size.at("w").get<uint32_t>();
        const auto sheetHeight = size.at("h").get<uint32_t>();
        spdlog::info("Spritesheet {} has size {}x{}", jsonPath.string(), sheetWidth, sheetHeight);

        constexpr auto extractFrameInformation =
            [](nlohmann::json const& value) -> OZZ::scene::SpritesheetFrameInformation {
            OZZ::scene::SpritesheetFrameInformation info{};

            // Get the animation name
            if (!value.contains("filename")) {
                spdlog::error("No frame information in spritesheet");
                return info;
            }
            const auto filename = value.at("filename");
            std::string stem = std::filesystem::path(filename.get<std::string>()).stem().string();
            const auto pos = stem.find_last_of('_');
            info.AnimationName = stem.substr(0, pos);
            info.FrameNumber = std::stoi(stem.substr(pos + 1));

            // let's get the size now
            if (!value.contains("frame")) {
                spdlog::error("No frame information in spritesheet");
                return info;
            }

            const auto frame = value.at("frame");
            const auto x = frame.at("x").get<uint32_t>();
            const auto y = frame.at("y").get<uint32_t>();
            const auto frameWidth = frame.at("w").get<uint32_t>();
            const auto frameHeight = frame.at("h").get<uint32_t>();

            info.Position = {x, y};
            info.Size = {frameWidth, frameHeight};

            // and finally the pivot
            if (!value.contains("pivot")) {
                spdlog::info("No pivot in spritesheet");
                return info;
            }

            const auto pivot = value.at("pivot");
            const auto pivotX = pivot.at("x").get<float>();
            const auto pivotY = pivot.at("y").get<float>();

            info.Pivot = {pivotX, pivotY};
            return info;
        };

        // get the frames array
        if (!data.contains("frames")) {
            spdlog::info("No frames in spritesheet");
            return false;
        }
        const auto frames = data.at("frames");
        if (!frames.is_array()) {
            spdlog::info("Frames is not array");
            return false;
        }

        std::unordered_map<std::string, OZZ::scene::SpritesheetAnimationFrames> animations{};
        for (const auto& frame : frames) {

            const auto frameInfo = extractFrameInformation(frame);

            if (!animations.contains(frameInfo.AnimationName)) {
                // create the vector
                animations.emplace(frameInfo.AnimationName, OZZ::scene::SpritesheetAnimationFrames{});
            }

            animations[frameInfo.AnimationName].emplace_back(frameInfo);
        }

        // sort the animations by frame number
        for (auto& unsortedFrames : animations | std::views::values) {
            std::ranges::sort(unsortedFrames, std::less{}, &OZZ::scene::SpritesheetFrameInformation::FrameNumber);
        }

        OZZ::scene::SpritesheetInformation sheetInfo{
            .TextureSize = {sheetWidth, sheetHeight},
            .Animations = animations,
        };
        spriteSheets.emplace(std::make_pair(texturePath.string(), sheetInfo));
        return true;
    }

    const std::unordered_map<std::string, SpritesheetInformation>& ResourceManager::GetSpritesheets() const {
        return spriteSheets;
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