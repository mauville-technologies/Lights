//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include <string>
#include <memory>
#include <utility>
#include <glad/glad.h>
#include "texture.h"
#include "shader.h"

namespace OZZ {
    struct TextureMapping {

        std::string SlotName;
        int SlotNumber;
        std::shared_ptr<Texture> TextureResource;
    };

    enum class DrawMode {
        Triangles,
        Lines,
        LineLoop
    };

    inline GLenum ToGLEnum(const DrawMode& mode) {
        switch (mode) {
            case DrawMode::Lines:
                return GL_LINES;
            case DrawMode::LineLoop:
                return GL_LINE_LOOP;
            case DrawMode::Triangles:
            default:
                return GL_TRIANGLES;
        }
    }

    class Material {
    public:
        Material() = default;
        ~Material() = default;

        void Bind();

        void SetShader(std::shared_ptr<Shader> inShader) {
            this->shader = std::move(inShader);
        }

        [[nodiscard]] std::shared_ptr<Shader> GetShader() const {
            return shader;
        }

        void AddTextureMapping(const TextureMapping& mapping);
        void RemoveTextureMapping(const std::string& slotName);

        [[nodiscard]] const std::vector<TextureMapping>& GetTextureMappings() const {
            return textureMappings;
        }

        DrawMode GetDrawMode() const {
            return drawMode;
        }

        void SetDrawMode(const DrawMode mode) {
            drawMode = mode;
        }

    private:
        std::shared_ptr<Shader> shader;
        std::vector<TextureMapping> textureMappings;

        DrawMode drawMode { DrawMode::Triangles };
    };

} // OZZ