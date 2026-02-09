//
// Created by ozzadar on 2025-11-30.
//

#pragma once
#include "lights/core/platform/platform_window.h"
#include "lights/core/platform/window.h"
#include "lights/core/rendering/buffer.h"
#include "lights/core/util/image.h"

#include <condition_variable>
#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <thread>

namespace OZZ {
    class Texture;
}

namespace OZZ::scene {
    struct SpritesheetFrameInformation {
        std::string AnimationName;
        uint32_t FrameNumber;
        glm::ivec2 Position{};
        glm::ivec2 Size{};
        glm::vec2 Pivot{};
    };

    using SpritesheetAnimationFrames = std::vector<SpritesheetFrameInformation>;

    struct SpritesheetInformation {
        glm::ivec2 TextureSize;
        std::unordered_map<std::string, SpritesheetAnimationFrames> Animations;
    };

    struct SpritesheetLocation {
        std::filesystem::path JsonMetadata;
        std::filesystem::path Texture;
    };

    class ResourceManager {
    public:
        explicit ResourceManager();
        ~ResourceManager();

        std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path);
        std::shared_ptr<Texture> LoadTexture(std::shared_ptr<Image> image);

        bool LoadSpritesheet(const SpritesheetLocation& location);
        const std::unordered_map<std::string, SpritesheetInformation>& GetSpritesheets() const;
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

        // TODO: @paulm -- the resource manager has some weird separation of concerns at the moment. I should come back
        // to this
        std::unordered_map<std::string, SpritesheetInformation> spriteSheets{};
    };

} // namespace OZZ::scene
