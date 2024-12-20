//
// Created by ozzadar on 2024-12-19.
//

#include "texture.h"
#include "spdlog/spdlog.h"
#include <glad/glad.h>

namespace OZZ {
    Texture::Texture() {
        glGenTextures(1, &textureId);
    }

    Texture::~Texture() {
        glDeleteTextures(1, &textureId);
    }

    void Texture::UploadData(Image *image) {
        glBindTexture(GL_TEXTURE_2D, textureId);
        int format = GL_RGBA;

        switch (image->GetChannels()) {
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                spdlog::warn("Unsupported image format. Defaulting to RGBA");
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     image->GetWidth(), image->GetHeight(), 0,
                     format, GL_UNSIGNED_BYTE, image->GetData().data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture::Bind() {
        glBindTexture(GL_TEXTURE_2D, textureId);

        // TODO: Separate sampler params from the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
} // OZZ