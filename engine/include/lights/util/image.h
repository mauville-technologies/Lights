//
// Created by ozzadar on 2024-12-19.
//

#pragma once
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>

namespace OZZ {
    class Image {
    public:
        // returnss a tuple of (gridSize, imageSize, image)
        static std::tuple<glm::vec2, glm::vec2, std::unique_ptr<Image>> MergeImages(const std::vector<std::unique_ptr<Image>>& images);

        using path = std::filesystem::path;
        Image() = default;
        explicit Image(const path& texturePath, int desiredChannels = 4);
        Image(const unsigned char* inData, int inWidth, int inHeight, int inChannels);
        ~Image();

        /**
         * Generates a blank image with the specified color and size.
         * @param color Color to fill the image with (RGBA format, values between 0 and 1)
         * @param size Size of the resulting image
         */
        void FillColor(const glm::vec4& color, const glm::vec2& size);
        void FlipPixels(bool bVertical = true, bool bHorizontal = true);

        [[nodiscard]] inline const std::vector<unsigned char>& GetData() const {
            return data;
        }

        [[nodiscard]] inline int GetWidth() const {
            return width;
        }

        [[nodiscard]] inline int GetHeight() const {
            return height;
        }

        [[nodiscard]] inline int GetChannels() const {
            return channels;
        }

        void SaveToFile(std::filesystem::path imagePath);

        inline bool IsValid() const {
            return !data.empty() && width > 0 && height > 0 && channels > 0;
        }

    private:
        std::vector<unsigned char> data{};
        int width{0};
        int height{0};
        int channels{0};
    };
}
