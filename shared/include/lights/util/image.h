//
// Created by ozzadar on 2024-12-19.
//

#pragma once
#include <vector>
#include <filesystem>

namespace OZZ {
    class Image {
    public:
        using path = std::filesystem::path;
        Image(const path& texturePath, int desiredChannels = 4);
        Image(const unsigned char* inData, int inWidth, int inHeight, int inChannels);
        ~Image();

        void FlipPixels(bool bVertical = true, bool bHorizontal = true);

        [[nodiscard]] inline const std::vector<unsigned char>& GetData() const { return data; }
        [[nodiscard]] inline int GetWidth() const { return width; }
        [[nodiscard]] inline int GetHeight() const { return height; }
        [[nodiscard]] inline int GetChannels() const { return channels; }

    private:
        std::vector<unsigned char> data;
        int width { 0 };
        int height { 0 };
        int channels { 0 };
    };
}