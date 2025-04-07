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

    Image::Image(const unsigned char *inData, int inWidth, int inHeight, int inChannels) {
        // resize the data vector to the size of the image
        data.resize(inWidth * inHeight * inChannels);
        // copy the data into the vector
        std::copy_n(inData, inWidth * inHeight * inChannels, data.begin());
        // set the width, height and channels
        width = inWidth;
        height = inHeight;
        channels = inChannels;
    }

    Image::~Image() {
        data.clear();
    }

    void Image::FlipPixels(bool bVertical, bool bHorizontal) {
        uint8_t stepSize = channels;

        if (bVertical && bHorizontal) {
            for (int i = 0; i < width * height * stepSize / 2; i += stepSize) {
                for (int j = 0; j < stepSize; j++) {
                    std::swap(data[i + j], data[(width * height * stepSize - 1) - (i + j)]);
                }
            }
        }
        else if (bVertical) {
            // loop through each row, from the bottom, rebuilding the image
            for (int i = 0; i < height / 2; i++) {
                for (int j = 0; j < width * stepSize; j++) {
                    std::swap(data[i * width * stepSize + j], data[(height - 1 - i) * width * stepSize + j]);
                }
            }
        } else if (bHorizontal) {
            // loop through each column, from the right, rebuilding the image
            for (int i = 0; i < width / 2; i++) {
                for (int j = 0; j < height * stepSize; j++) {
                    std::swap(data[i * stepSize + (j*width)], data[(width - 1 - i) * stepSize + (j*width)]);
                }
            }
        }
    }
}
