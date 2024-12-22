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
    private:
        uint32_t textureId;
    };
} // OZZ