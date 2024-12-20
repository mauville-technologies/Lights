//
// Created by ozzadar on 2024-12-19.
//

#include "lights/util/image.h"
#include <stb_image.h>

namespace OZZ {
    Image::Image(const path& texturePath, int desiredChannels) {
        stbi_set_flip_vertically_on_load(true);
        auto* iData = stbi_load(texturePath.string().c_str(), &width, &height, &channels, desiredChannels);
        if (iData) {
            data = std::vector<unsigned char>(iData, iData + width * height * channels);
            stbi_image_free(iData);
        }
    }

    Image::~Image() {
        data.clear();
    }
}