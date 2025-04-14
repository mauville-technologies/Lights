//
// Created by ozzadar on 2024-12-19.
//

#include "lights/rendering/material.h"
#include <glad/glad.h>
#include <algorithm>

namespace OZZ {
    void Material::Bind() {
        shader->Bind();
        glLineWidth(settings.LineWidth);
        glPointSize(settings.PointSize);

        for (const auto &mapping : textureMappings) {
            glActiveTexture(mapping.SlotNumber);
            shader->SetInteger(mapping.SlotName, mapping.SlotNumber - GL_TEXTURE0);
            mapping.TextureResource->Bind();
        }

        for (const auto &[Name, Value] : uniformSettings) {
            if (std::holds_alternative<int>(Value)) {
                shader->SetInteger(Name, std::get<int>(Value));
            } else if (std::holds_alternative<glm::vec3>(Value)) {
                shader->SetVec3(Name, std::get<glm::vec3>(Value));
            } else if (std::holds_alternative<glm::vec4>(Value)) {
                shader->SetVec4(Name, std::get<glm::vec4>(Value));
            }
        }
    }
    void Material::AddTextureMapping(const TextureMapping &mapping) {
        textureMappings.push_back(mapping);
    }

    void Material::RemoveTextureMapping(const std::string &slotName) {
        textureMappings.erase(std::remove_if(textureMappings.begin(), textureMappings.end(),
                     [slotName](const TextureMapping &mapping) {
                         return mapping.SlotName == slotName;
                     }), textureMappings.end());
    }

    void Material::AddUniformSetting(const UniformSetting &setting) {
        // find first setting
        auto it = std::find_if(uniformSettings.begin(), uniformSettings.end(),
            [&setting](const UniformSetting &s) {
                return s.Name == setting.Name;
            });

        if (it != uniformSettings.end()) {
            // if found, update the value
            it->Value = setting.Value;
            return;
        }

        uniformSettings.push_back(setting);
    }

    void Material::RemoveUniformSetting(const std::string &name) {
        uniformSettings.erase(std::remove_if(uniformSettings.begin(), uniformSettings.end(),
            [name](const UniformSetting &setting) {
                return setting.Name == name;
            }), uniformSettings.end());
    }
} // OZZ