//
// Created by ozzadar on 2024-12-18.
//

#include "lights/core/rendering/shader.h"
#include "spdlog/spdlog.h"
#include <fstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace OZZ {
    Shader::Shader(ShaderFileParams&& shaderFiles) : shaderId(GL_INVALID_INDEX) {
        // load files
        std::ifstream vertexFile(shaderFiles.Vertex);
        std::ifstream fragmentFile(shaderFiles.Fragment);

        if (!vertexFile.is_open()) {
            throw std::runtime_error("Failed to open vertex shader file");
        }

        if (!fragmentFile.is_open()) {
            throw std::runtime_error("Failed to open fragment shader file");
        }

        std::string vertexSource((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
        std::string fragmentSource((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());
        std::string geometrySource;

        if (!shaderFiles.Geometry.empty()) {
            std::ifstream geometryShader(shaderFiles.Geometry);
            geometrySource = std::string((std::istreambuf_iterator(geometryShader)), std::istreambuf_iterator<char>());
        }

        compile(vertexSource, fragmentSource, geometrySource);
    }

    Shader::Shader(const ShaderSourceParams& shaderSource) {
        compile(shaderSource.Vertex, shaderSource.Fragment, shaderSource.Geometry);
    }

    Shader::~Shader() {
        // Destroy the shader
        glDeleteProgram(shaderId);
    }

    void Shader::Bind() {
        glUseProgram(shaderId);
    }

    void Shader::SetInteger(const std::string& name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
        glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::compile(const std::string& vertexSource,
                         const std::string& fragmentSource,
                         const std::string& geometrySource) {
        uint32_t vertex, fragment, geometry;

        int success;
        char infoLog[512];

        // Vertex Shader
        const char* vShaderCode = vertexSource.c_str();
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            spdlog::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
        }

        // Fragment shader
        const char* fShaderCode = fragmentSource.c_str();
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
        }

        // Geometry shader
        bool bHasGeometry = !geometrySource.empty();
        if (bHasGeometry) {
            const char* gShaderCode = geometrySource.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, nullptr);
            glCompileShader(geometry);
            glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
                spdlog::error("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n{}", infoLog);
            }
        }

        shaderId = glCreateProgram();
        glAttachShader(shaderId, vertex);
        glAttachShader(shaderId, fragment);
        if (bHasGeometry) {
            glAttachShader(shaderId, geometry);
        }
        glLinkProgram(shaderId);

        glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
            spdlog::error("ERROR::SHADER::LINKING FAILED\n{}", infoLog);
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    int Shader::getUniformLocation(const std::string& name) const {
        return glGetUniformLocation(shaderId, name.c_str());
    }
} // namespace OZZ
