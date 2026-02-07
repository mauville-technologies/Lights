//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include "lights/util/ring_buffer.h"
#include "vertex.h"

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <span>
#include <spdlog/spdlog.h>

namespace OZZ {

    class Buffer {
    public:
        Buffer(uint32_t bufferType);
        ~Buffer();

        void UploadData(const void* data, int64_t offset, int64_t uploadSize, bool replace = false);
        void Bind() const;

        [[nodiscard]] size_t GetSize() const { return size; }

        [[nodiscard]] uint32_t GetHandle() const { return vbo; }

    private:
        uint32_t vbo;
        uint32_t type;
        size_t size;
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

    class StorageBufferBase {
    public:
        virtual ~StorageBufferBase() = default;
        virtual void Bind(uint32_t bindingPoint) const = 0;
    };

    template <typename T, size_t ElementCount>
    class StorageBuffer : public StorageBufferBase {
    public:
        explicit StorageBuffer() : buffer(std::make_unique<Buffer>(GL_SHADER_STORAGE_BUFFER)) {
            buffer->UploadData(nullptr, 0, BufferSize, true);
        }

        ~StorageBuffer() override { buffer.reset(); };

        void Bind(uint32_t bindingPoint) const override {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buffer->GetHandle());
        }

        bool UploadData(const std::array<T, ElementCount>& data) { return UploadData(data.data(), 0, ElementCount); }

        bool UploadData(const T* data, int64_t offset, const size_t numElementsToUpload) {
            if (numElementsToUpload + offset / sizeof(T) > NumElements) {
                spdlog::error("Cannot upload {} elements to storage buffer of size {} elements",
                              numElementsToUpload,
                              NumElements);
                return false;
            }
            buffer->UploadData(static_cast<const void*>(data), offset, sizeof(T) * numElementsToUpload, false);
            return true;
        };

        constexpr static size_t BufferSize = sizeof(T) * ElementCount;
        constexpr static size_t NumElements = ElementCount;

    private:
        std::unique_ptr<Buffer> buffer;
    };
} // namespace OZZ