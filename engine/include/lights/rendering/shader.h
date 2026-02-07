//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <filesystem>
#include <glm/glm.hpp>

namespace OZZ {
    struct ShaderFileParams {
        std::filesystem::path Vertex;
        std::filesystem::path Fragment;
        std::filesystem::path Geometry;
    };

    struct ShaderSourceParams {
        std::string Vertex;
        std::string Fragment;
        std::string Geometry;
    };

    class Shader {
    public:
        using path = std::filesystem::path;
        Shader(ShaderFileParams&& shaderFiles);
        Shader(const ShaderSourceParams& shaderSource);
        ~Shader();

        void SetInteger(const std::string& name, int value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        void Bind();

    private:
        void
        compile(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource);
        int getUniformLocation(const std::string& name) const;

    private:
        uint32_t shaderId;
    };
} // namespace OZZ
