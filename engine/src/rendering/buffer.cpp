//
// Created by ozzadar on 2024-12-18.
//

#include "lights/rendering/buffer.h"

namespace OZZ {
    Buffer::Buffer(uint32_t bufferType) : type(bufferType) {
        // create the buffer
        glGenBuffers(1, &vbo);
    }

    Buffer::~Buffer() {
        // destroy the buffer and vao
        glDeleteBuffers(1, &vbo);
    }

    void Buffer::UploadData(const void* data, size_t size) {
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

    void IndexVertexBuffer::UploadData(std::span<const Vertex> vertices, std::span<const uint32_t> indices) {
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

    GPUStagingBuffer::GPUStagingBuffer(size_t numBytes) : size(numBytes) {
        glGenBuffers(1, &pbo);
        Bind();
        glBufferStorage(
            GL_PIXEL_UNPACK_BUFFER, numBytes, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        mappedPointer = static_cast<std::byte*>(glMapBufferRange(
            GL_PIXEL_UNPACK_BUFFER, 0, numBytes, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        Unbind();

        assert(mappedPointer && "mapped pointer is null -- failed to generate gpu pbo buffer");
        ringBuffer = std::make_unique<util::RingBuffer>(static_cast<std::byte*>(mappedPointer), numBytes);
    }

    GPUStagingBuffer::~GPUStagingBuffer() {
        Bind();
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glDeleteBuffers(1, &pbo);
        Unbind();

        while (!inFlightRegions.empty()) {
            const auto& inFlight = inFlightRegions.front();
            glDeleteSync(inFlight.Fence);
            inFlightRegions.pop_front();
        }

        ringBuffer.reset();
    }

    std::pair<std::byte*, size_t> GPUStagingBuffer::GetBuffer() const {
        return {mappedPointer, size};
    }

    std::byte* GPUStagingBuffer::GetAllocatedPointer(const util::RingBufferAllocation& allocation) const {
        return mappedPointer + allocation.offset;
    }

    void GPUStagingBuffer::Bind() const {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    }

    void GPUStagingBuffer::Unbind() const {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    util::RingBufferAllocation GPUStagingBuffer::GetSlice(const size_t size) {
        return ringBuffer->Allocate(size);
    }

    void GPUStagingBuffer::RegisterInFlight(InFlightRegion&& inFlight) {
        inFlightRegions.push_back(std::move(inFlight));
    }

    void GPUStagingBuffer::Tick() {
        // check fences
        while (!inFlightRegions.empty()) {
            const auto& front = inFlightRegions.front();

            const GLenum result = glClientWaitSync(front.Fence, 0, 0);
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                // pop
                glDeleteSync(front.Fence);
                ringBuffer->Consume(front.Size);
                front.Callback();
                inFlightRegions.pop_front();
                continue;
            }
            break;
        }
    }
} // namespace OZZ