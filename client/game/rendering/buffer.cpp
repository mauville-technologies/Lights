//
// Created by ozzadar on 2024-12-18.
//

#include "buffer.h"

namespace OZZ {
    Buffer::Buffer(uint32_t bufferType) : type(bufferType) {
        // create the buffer
        glGenBuffers(1, &vbo);
    }

    Buffer::~Buffer() {
        // destroy the buffer and vao
        glDeleteBuffers(1, &vbo);
    }

    void Buffer::UploadData(void* data, size_t size) {
        Bind();
        glBufferData(type, size, data, GL_STATIC_DRAW);
    }

    void Buffer::Bind() const {
        glBindBuffer(type, vbo);

    }

    IndexVertexBuffer::IndexVertexBuffer() {
        glGenVertexArrays(1, &vao);
        vertexBuffer = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
        indexBuffer = std::make_unique<Buffer>(GL_ELEMENT_ARRAY_BUFFER);

        // bind the vao
        Bind();
        Vertex::BindAttribPointers();
        Unbind();
    }

    IndexVertexBuffer::~IndexVertexBuffer() {
        Unbind();
        vertexBuffer.reset();
        indexBuffer.reset();
        glDeleteVertexArrays(1, &vao);
    }

    void IndexVertexBuffer::UploadData(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) {
        vertexBuffer->UploadData(vertices.data(), vertices.size() * sizeof(Vertex));
        indexBuffer->UploadData(indices.data(), indices.size() * sizeof(uint32_t));
        indexCount = static_cast<int>(indices.size());
    }

    void IndexVertexBuffer::Bind() const {
        glBindVertexArray(vao);
        vertexBuffer->Bind();
        indexBuffer->Bind();
    }

    void IndexVertexBuffer::Unbind() const {
        glBindVertexArray(0);
    }
} // OZZ