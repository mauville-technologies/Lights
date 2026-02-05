//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include "lights/util/ring_buffer.h"
#include "vertex.h"

#include <deque>
#include <functional>
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

    class GPUStagingBuffer {
    public:
        struct InFlightRegion {
            size_t Offset;
            size_t Size;
            GLsync Fence;
            std::function<void()> Callback;
        };

        explicit GPUStagingBuffer(size_t numBytes);
        ~GPUStagingBuffer();

        [[nodiscard]] std::byte* GetAllocatedPointer(const util::RingBufferAllocation& allocation) const;

        void Bind() const;
        void Unbind() const;
        [[nodiscard]] util::RingBufferAllocation GetSlice(size_t size);
        void RegisterInFlight(InFlightRegion&& inFlight);
        void Tick();

        [[nodiscard]] size_t GetSize() const { return size; }

    protected:
        // Unused probably
        [[nodiscard]] std::pair<std::byte*, size_t> GetBuffer() const;

    private:
        uint32_t pbo{0};
        std::byte* mappedPointer;
        size_t size;

        std::unique_ptr<util::RingBuffer> ringBuffer;
        std::deque<InFlightRegion> inFlightRegions;
    };

    class StorageBuffer {
    public:
    private:
        std::unique_ptr<Buffer> buffer;
    };
} // namespace OZZ