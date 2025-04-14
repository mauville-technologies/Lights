//
// Created by ozzadar on 2024-12-18.
//

#include <fstream>
#include "lights/rendering/shader.h"
#include "spdlog/spdlog.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace OZZ {
    Shader::Shader(const Shader::path &vertexPath, const Shader::path &fragmentPath) {
        // load files
        std::ifstream vertexFile(vertexPath);
        std::ifstream fragmentFile(fragmentPath);

        if (!vertexFile.is_open()) {
            throw std::runtime_error("Failed to open vertex shader file");
        }

        if (!fragmentFile.is_open()) {
            throw std::runtime_error("Failed to open fragment shader file");
        }

        std::string vertexSource((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
        std::string fragmentSource((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());

        compile(vertexSource, fragmentSource);
    }

    Shader::~Shader() {
        // Destroy the shader
        glDeleteProgram(shaderId);
    }

    void Shader::Bind() {
        glUseProgram(shaderId);
    }

    void Shader::SetInteger(const std::string &name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::SetVec3(const std::string &name, const glm::vec3 &value) {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetVec4(const std::string &name, const glm::vec4 &value) {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string &name, const glm::mat4& value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::compile(const std::string &vertexSource, const std::string &fragmentSource) {
        uint32_t vertex, fragment;

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
        if (!fragment) {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
        }

        shaderId = glCreateProgram();
        glAttachShader(shaderId, vertex);
        glAttachShader(shaderId, fragment);
        glLinkProgram(shaderId);

        glGetProgramiv(shaderId, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(shaderId, 512, nullptr, infoLog);
            spdlog::error("ERROR::SHADER::LINKING FAILED\n{}", infoLog);
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    int Shader::getUniformLocation(const std::string &name) const {
        return glGetUniformLocation(shaderId, name.c_str());
    }



} // OZZ