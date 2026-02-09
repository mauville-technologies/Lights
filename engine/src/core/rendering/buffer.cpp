//
// Created by ozzadar on 2024-12-18.
//

#include "lights/core/rendering/buffer.h"

namespace OZZ {
    Buffer::Buffer(uint32_t bufferType) : type(bufferType) {
        // create the buffer
        glCreateBuffers(1, &vbo);
    }

    Buffer::~Buffer() {
        // destroy the buffer and vao
        glDeleteBuffers(1, &vbo);
    }

    void Buffer::UploadData(const void* data, int64_t offset, int64_t uploadSize, bool replace) {
        if (replace) {
            glDeleteBuffers(1, &vbo);
            vbo = 0;
            glCreateBuffers(1, &vbo);
            glNamedBufferStorage(vbo, uploadSize, data, GL_DYNAMIC_STORAGE_BIT);
            size = uploadSize;
        } else {
            if (offset + uploadSize > size) {
                spdlog::error("Trying to write outside of buffer bounds! Offset: {}, UploadSize: {}, BufferSize: {}",
                              offset,
                              uploadSize,
                              size);
                return;
            }
        }

        glNamedBufferSubData(vbo, offset, uploadSize, data);
    }

    void Buffer::Bind() const {
        glBindBuffer(type, vbo);
    }

    IndexVertexBuffer::IndexVertexBuffer() {
        glGenVertexArrays(1, &vao);
        vertexBuffer = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
        indexBuffer = std::make_unique<Buffer>(GL_ELEMENT_ARRAY_BUFFER);
    }

    IndexVertexBuffer::~IndexVertexBuffer() {
        Unbind();
        vertexBuffer.reset();
        indexBuffer.reset();
        glDeleteVertexArrays(1, &vao);
    }

    void IndexVertexBuffer::UploadData(std::span<const Vertex> vertices, std::span<const uint32_t> indices) {
        vertexBuffer->UploadData(vertices.data(), 0, vertices.size() * sizeof(Vertex), true);
        indexBuffer->UploadData(indices.data(), 0, indices.size() * sizeof(uint32_t), true);
        Bind();
        Vertex::BindAttribPointers();
        Unbind();
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
        glCreateBuffers(1, &pbo);
        glNamedBufferStorage(pbo, numBytes, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

        mappedPointer = static_cast<std::byte*>(
            glMapNamedBufferRange(pbo, 0, numBytes, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        Unbind();

        assert(mappedPointer && "mapped pointer is null -- failed to generate gpu pbo buffer");
        ringBuffer = std::make_unique<util::RingBuffer>(static_cast<std::byte*>(mappedPointer), numBytes);
    }

    GPUStagingBuffer::~GPUStagingBuffer() {
        glUnmapNamedBuffer(pbo);
        glDeleteBuffers(1, &pbo);

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