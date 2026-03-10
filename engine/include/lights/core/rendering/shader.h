//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include "ozz_rendering/rhi_device.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace OZZ {
    class Shader {
    public:
        using path = std::filesystem::path;
        Shader(rendering::RHIDevice* inDevice, rendering::ShaderFileParams&& shaderFiles);
        Shader(rendering::RHIDevice* inDevice, rendering::ShaderSourceParams&& shaderSource);
        ~Shader();

        void Bind(rendering::RHIFrameContext& frameContext);

        rendering::RHIPipelineLayoutDescriptor GetLayoutDescriptor() const;

        const rendering::RHIShaderHandle GetRHIHandle() const { return rhiShaderHandle; }

    private:
        rendering::RHIDevice* device;
        rendering::RHIShaderHandle rhiShaderHandle{rendering::RHIShaderHandle::Null()};
    };
} // namespace OZZ
