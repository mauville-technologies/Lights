//
// Created by ozzadar on 2024-12-19.
//

#pragma once
#include "ozz_rendering/rhi_device.h"

#include <atomic>
#include <lights/core/util/image.h>

namespace OZZ {
    class Texture {
    public:
        Texture(rendering::RHIDevice* inDevice, rendering::TextureDescriptor&& inDescriptor);
        ~Texture();

        void Reserve(glm::uvec2 size);
        void UploadData(Image* image);
        void FinalizeUpload();

        const rendering::TextureDescriptor& GetDescriptor() const { return descriptor; }

        rendering::RHITextureHandle GetRHIHandle() const { return rhiTextureHandle; }

        bool IsLoaded() const { return bLoaded.load(); };

    private:
        rendering::RHIDevice* device;
        rendering::TextureDescriptor descriptor;

        rendering::RHITextureHandle rhiTextureHandle{rendering::RHITextureHandle::Null()};
        std::atomic<bool> bLoaded{false};
    };
} // namespace OZZ