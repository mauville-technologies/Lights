//
// Created by ozzadar on 2024-12-19.
//

#include "lights/rendering/material.h"
#include <glad/glad.h>

namespace OZZ {
    void Material::Bind() {
        shader->Bind();
        glLineWidth(2.f);
        glPointSize(5.f);

        for (const auto &mapping : textureMappings) {
            glActiveTexture(mapping.SlotNumber);
            shader->SetInteger(mapping.SlotName, mapping.SlotNumber - GL_TEXTURE0);
            mapping.TextureResource->Bind();
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

} // OZZ