//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include "vertex.h"
#include <memory>
#include <span>
#include <vector>

namespace OZZ {

    class Buffer {
    public:
        Buffer(uint32_t bufferType);
        ~Buffer();

        void UploadData(const void* data, size_t size);

        void Bind() const;

    private:
        uint32_t vbo;
        uint32_t type;
    };

    class IndexVertexBuffer {
    public:
        IndexVertexBuffer();
        ~IndexVertexBuffer();

        // void UploadData(std::span<Vertex> vertices, std::span<uint32_t> indices);
        void UploadData(std::span<const Vertex> vertices, std::span<const uint32_t> indices);

        void Bind() const;
        void Unbind() const;

        [[nodiscard]] int GetIndexCount() const { return indexCount; };

    private:
        uint32_t vao;
        int indexCount;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
    };
} // namespace OZZ