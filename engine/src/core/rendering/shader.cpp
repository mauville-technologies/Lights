//
// Created by ozzadar on 2024-12-18.
//

#include "lights/core/rendering/shader.h"
#include "lights/core/util/profiling.h"
#include "spdlog/spdlog.h"
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

namespace OZZ {
    Shader::Shader(rendering::RHIDevice* inDevice, rendering::ShaderFileParams&& shaderFiles)
        : device(inDevice) {
        OZZ_PROFILE_FUNCTION;
        rhiShaderHandle = device->CreateShader(std::move(shaderFiles));
    }

    Shader::Shader(rendering::RHIDevice* inDevice, rendering::ShaderSourceParams&& shaderSource)
        : device(inDevice) {
        OZZ_PROFILE_FUNCTION;
        rhiShaderHandle = device->CreateShader(std::move(shaderSource));
    }

    Shader::~Shader() {
        // Destroy the shader
        device->FreeShader(rhiShaderHandle);
        rhiShaderHandle = rendering::RHIShaderHandle::Null();
    }

    void Shader::Bind(rendering::RHIFrameContext& frameContext) {
        OZZ_PROFILE_FUNCTION;
        device->BindShader(frameContext, rhiShaderHandle);
    }

    rendering::RHIPipelineLayoutDescriptor Shader::GetLayoutDescriptor() const {
        return device->GetShaderPipelineLayout(rhiShaderHandle);
    }

} // namespace OZZ
