//
// Created by ozzadar on 2024-12-19.
//

#pragma once
#include <lights/util/image.h>

namespace OZZ {
    class Texture {
    public:
        Texture();
        ~Texture();

        void UploadData(Image* image);

        void Bind();

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
    private:
        uint32_t textureId;
        int width { 0 };
        int height { 0 };
    };
} // OZZ