//
// Created by ozzadar on 2024-12-19.
//

#pragma once
#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

namespace OZZ {

    struct ExternalBufferTag {};

    inline constexpr ExternalBufferTag ExternalBuffer{};

    class Image {
    public:
        struct ImageInfo {
            int Width;
            int Height;
            int Channels;

            int SizeInBytes() const { return Width * Height * Channels; }
        };

        // returnss a tuple of (gridSize, imageSize, image)
        static std::tuple<glm::vec2, glm::vec2, std::unique_ptr<Image>>
        MergeImages(const std::vector<std::unique_ptr<Image>>& images);

        using path = std::filesystem::path;
        static ImageInfo GetImageInfo(const path& imagePath);

        Image() = default;
        explicit Image(const path& texturePath);
        Image(const unsigned char* inData, int inWidth, int inHeight, int inChannels);
        Image(ExternalBufferTag, const path& texturePath, std::byte* inExternalBuffer);
        ~Image();

        /**
         * Generates a blank image with the specified color and size.
         * @param color Color to fill the image with (RGBA format, values between 0 and 1)
         * @param size Size of the resulting image
         */
        void FillColor(const glm::vec4& color, const glm::vec2& size);
        void FlipPixels(bool bVertical = true, bool bHorizontal = true);

        [[nodiscard]] inline const void* GetDataPointer() const {
            return bOwnsData ? static_cast<const void*>(data.data()) : static_cast<const void*>(externalBuffer);
        }

        [[nodiscard]] inline int GetWidth() const { return width; }

        [[nodiscard]] inline int GetHeight() const { return height; }

        [[nodiscard]] inline int GetChannels() const { return channels; }

        void SaveToFile(std::filesystem::path imagePath);

        inline bool IsValid() const { return !data.empty() && width > 0 && height > 0 && channels > 0; }

    private:
        [[nodiscard]] inline const std::vector<unsigned char>& GetData() const { return data; }

    private:
        std::vector<unsigned char> data{};
        bool bOwnsData{true};
        std::byte* externalBuffer = nullptr;

        int width{0};
        int height{0};
        int channels{0};
    };
} // namespace OZZ
