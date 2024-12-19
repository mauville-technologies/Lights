//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <vector>
#include "vertex.h"
#include <memory>

namespace OZZ {

    class Buffer {
    public:
        Buffer(uint32_t bufferType);
        ~Buffer();

        void UploadData(void* data, size_t size);

        void Bind() const;
    private:
        uint32_t vbo;
        uint32_t type;
    };

    class IndexVertexBuffer {
    public:
        IndexVertexBuffer();
        ~IndexVertexBuffer();

        void UploadData(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

        void Bind() const;
        void Unbind() const;

        [[nodiscard]] uint32_t GetIndexCount() const { return indexCount; };

    private:
        uint32_t vao;
        uint32_t indexCount;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
    };
} // OZZ