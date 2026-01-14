//
// Created by ozzadar on 2024-12-19.
//

#include "lights/rendering/texture.h"
#include "lights/util/memory_literals.h"
#include "spdlog/spdlog.h"
#include <glad/glad.h>

namespace OZZ {
    Texture::Texture() {
        glGenTextures(1, &textureId);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &textureId);
    }

    void Texture::Reserve(const glm::ivec2 Size) {
        width = Size.x;
        height = Size.y;
        // Reserve is generally used for rendertargets and FBOs. We mark it as loaded so we can let rendering happen
        bLoaded.store(true);
        Bind();
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    }

    void Texture::UploadData(Image* image) {
        width = image->GetWidth();
        height = image->GetHeight();

        glBindTexture(GL_TEXTURE_2D, textureId);
        int format = GL_RGBA;

        switch (image->GetChannels()) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                spdlog::warn("Unsupported image format. Defaulting to RGBA");
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, image->GetDataPointer());
        glGenerateMipmap(GL_TEXTURE_2D);
        bLoaded.store(true);
    }

    void Texture::UploadData(Image* image, size_t offset) {

        width = image->GetWidth();
        height = image->GetHeight();

        glBindTexture(GL_TEXTURE_2D, textureId);
        int format = GL_RGBA;

        switch (image->GetChannels()) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                spdlog::warn("Unsupported image format. Defaulting to RGBA");
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        constexpr auto budgetBytes = 256_MiB;
        auto bytesPerRow = width * image->GetChannels();
        auto tileBytes = 16ULL * 1024 * 1024;

        auto tileHeight = (tileBytes / bytesPerRow) * bytesPerRow;
        tileHeight = std::max(1ULL, tileHeight);

        for (unsigned long long y = 0; y < height; y += tileHeight) {
            auto h = std::min(tileHeight, static_cast<unsigned long long>(height - y));
            auto newOffset = offset + (y * width * image->GetChannels());

            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            0,
                            0,
                            width,
                            static_cast<int>(h),
                            format,
                            GL_UNSIGNED_BYTE,
                            reinterpret_cast<const void*>(newOffset));
        }
    }

    void Texture::FinalizeUpload() {
        bLoaded.store(true);
    }

    void Texture::Bind() {
        if (auto loaded = IsLoaded(); !loaded) {
            spdlog::debug("Attempted to bind unloaded texture {}", textureId);
            return;
        }
        glBindTexture(GL_TEXTURE_2D, textureId);

        // TODO: Separate sampler params from the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
} // namespace OZZ