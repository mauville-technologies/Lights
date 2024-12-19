//
// Created by ozzadar on 2024-12-18.
//

#include <fstream>
#include "shader.h"
#include "spdlog/spdlog.h"

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

        // TODO: In the hangouts engine, I attached texturesamplerbindings to the shader and bound them here
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

} // OZZ