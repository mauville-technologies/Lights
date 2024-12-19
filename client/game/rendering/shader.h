//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <filesystem>

namespace OZZ {
    class Shader {
    public:
        using path = std::filesystem::path;
        Shader(const path& vertexPath, const path& fragmentPath);
        ~Shader();

        void Bind();
    private:
        void compile(const std::string& vertexSource, const std::string& fragmentSource);

    private:
        uint32_t shaderId;
    };

} // OZZ