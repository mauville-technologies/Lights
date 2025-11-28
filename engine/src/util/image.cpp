//
// Created by ozzadar on 2024-12-19.
//

#include "lights/util/image.h"
#include <algorithm>
#include <lights/util/configuration.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <stb_image_write.h>

namespace OZZ {
    std::tuple<glm::vec2, glm::vec2, std::unique_ptr<Image>>
    Image::MergeImages(const std::vector<std::unique_ptr<Image>>& images) {
        // let's assume a max width of 4096
        constexpr int maxTextureDimension = 4096;

        auto result = std::make_unique<Image>();

        // we're going to merge the images top-left to bottom-right
        // so we need to find the max width and height
        // we also need to know the max channels, we'll fill any missing channels with 0's
        int maxWidth = 0;
        int maxHeight = 0;
        int maxChannels = 0;
        for (const auto& image : images) {
            maxWidth = std::max(maxWidth, image->GetWidth());
            maxHeight = std::max(maxHeight, image->GetHeight());
            maxChannels = std::max(maxChannels, image->GetChannels());
        }

        // calculate the number of rows and columns, and the total size
        int numCols = maxTextureDimension / (maxWidth * maxChannels);
        int numRows = (static_cast<int>(images.size()) / numCols) + (images.size() % numCols != 0 ? 1 : 0);

        result->width = maxWidth * numCols * maxChannels;
        result->height = maxHeight * numRows;

        // resize data array
        result->data.resize(result->width * result->height * maxChannels);

        // loop through all the images, and copy them into the result image in the appropriate position
        for (int i = 0; i < images.size(); ++i) {
            const auto& image = images[i];

            // calculate the position in the result image
            int resultStartXPosition = ((i % numCols) * (maxWidth * maxChannels));
            int resultStartYPosition = ((i / numCols) * maxHeight);

            // copy the image data into the result image
            for (int y = 0; y < image->GetHeight(); y++) {
                for (int x = 0; x < image->GetWidth(); x++) {
                    auto currentX = resultStartXPosition + (x * maxChannels);
                    auto currentY = resultStartYPosition + y;

                    // copy the pixel data
                    for (int channel = 0; channel < maxChannels; channel++) {
                        if (channel < image->GetChannels()) {
                            result->data[(currentY * result->width) + currentX + channel] =
                                image->GetData()[(y * image->GetWidth() * image->GetChannels()) +
                                                 (x * image->GetChannels()) + channel];
                        } else {
                            result->data[(currentY * result->width) + currentX + channel] = 0; // fill with 0's
                        }
                    }
                }
            }
        }

        result->channels = maxChannels;
        result->height = maxHeight * numRows;
        result->width = maxWidth * numCols * maxChannels;
        return {glm::vec2{numCols, numRows}, glm::vec2{maxWidth, maxHeight}, std::move(result)};
    }

    Image::Image(const path& texturePath, int desiredChannels) {
        stbi_set_flip_vertically_on_load(true);
        stbi_info(texturePath.string().c_str(), &width, &height, &channels);

        if (auto* iData = stbi_load(texturePath.string().c_str(), &width, &height, &channels, channels)) {
            data = std::vector<unsigned char>(iData, iData + width * height * channels);
            stbi_image_free(iData);
        }
    }

    Image::Image(const unsigned char* inData, int inWidth, int inHeight, int inChannels) {
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

    void Image::FillColor(const glm::vec4& color, const glm::vec2& size) {
        // clear data
        data.clear();
        // resize the data vector to the size of the image
        width = static_cast<int>(size.x);
        height = static_cast<int>(size.y);
        channels = 4; // RGBA
        data.resize(width * height * channels);
        // fill the data with the color
        for (int i = 0; i < width * height; i += channels) {
            data[i] = static_cast<unsigned char>(color.r * 255);
            data[i + 1] = static_cast<unsigned char>(color.g * 255);
            data[i + 2] = static_cast<unsigned char>(color.b * 255);
            data[i + 3] = static_cast<unsigned char>(color.a * 255);
        }
    }

    void Image::FlipPixels(bool bVertical, bool bHorizontal) {
        uint8_t stepSize = channels;

        if (bVertical && bHorizontal) {
            for (int i = 0; i < width * height * stepSize / 2; i += stepSize) {
                for (int j = 0; j < stepSize; j++) {
                    std::swap(data[i + j], data[(width * height * stepSize - 1) - (i + j)]);
                }
            }
        } else if (bVertical) {
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
                    std::swap(data[i * stepSize + (j * width)], data[(width - 1 - i) * stepSize + (j * width)]);
                }
            }
        }
    }

    void Image::SaveToFile(std::filesystem::path imagePath) {
        // check if the path exists
        if (std::filesystem::exists(imagePath)) {
            // remove the file
            std::filesystem::remove(imagePath);
        }
        // save the image
        if (stbi_write_png(imagePath.string().c_str(), width, height, channels, data.data(), width * channels) == 0) {
            spdlog::error("Could not save image to file: {}", imagePath.string());
        } else {
            spdlog::info("Saved image to file: {}", imagePath.string());
        }
    }
} // namespace OZZ
