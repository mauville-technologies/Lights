//
// Created by ozzadar on 2024-12-19.
//

#include "lights/core/rendering/texture.h"
#include "lights/core/util/memory_literals.h"
#include "spdlog/spdlog.h"

namespace OZZ {
    namespace rendering {
        class RHIDevice;
    }

    Texture::Texture(rendering::RHIDevice* inDevice, rendering::TextureDescriptor&& inDescriptor)
        : device(inDevice)
        , descriptor(inDescriptor) {
        auto descriptorCopy = descriptor;
        rhiTextureHandle = device->CreateTexture(std::move(descriptorCopy));
    }

    Texture::~Texture() {
        device->FreeTexture(rhiTextureHandle);
        rhiTextureHandle = rendering::RHITextureHandle::Null();
    }

    void Texture::Reserve(const glm::uvec2 size) {
        // Reserve is generally used for rendertargets and FBOs. We mark it as loaded so we can let rendering happen
        if (size.x != descriptor.Width || size.y != descriptor.Height) {
            spdlog::error("Attempted to reserve texture data with mismatched dimensions. Expected {}x{}, got {}x{}",
                          descriptor.Width,
                          descriptor.Height,
                          size.x,
                          size.y);
            return;
        }
        bLoaded.store(true);
    }

    void Texture::UploadData(Image* image) {
        if (image->GetWidth() != descriptor.Width || image->GetHeight() != descriptor.Height) {
            spdlog::error("Attempted to upload image data with mismatched dimensions. Expected {}x{}, got {}x{}",
                          descriptor.Width,
                          descriptor.Height,
                          image->GetWidth(),
                          image->GetHeight());
            return;
        }

        device->UpdateTexture(
            rhiTextureHandle, image->GetDataPointer(), image->GetWidth() * image->GetHeight() * image->GetChannels());
        bLoaded.store(true);
    }

    void Texture::FinalizeUpload() {
        bLoaded.store(true);
    }
} // namespace OZZ