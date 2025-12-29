//
// Created by ozzadar on 2025-12-26.
//

#pragma once
#include <span>
#include <vector>

namespace OZZ::util {
    struct RingBufferAllocation {
        size_t offset{0};
        size_t size{0};

        [[nodiscard]] bool IsValid() const { return size > 0; }
    };

    class RingBuffer {
    public:
        /**
         * Creates and maintains its own array of bytes
         * @param inSize requested size
         */
        explicit RingBuffer(size_t inSize);

        /**
         * Uses externally managed byte blob
         * @param buffer pointer to blob
         * @param inSize size of blob
         */
        explicit RingBuffer(std::byte* buffer, size_t inSize);

        RingBufferAllocation Allocate(size_t reqSize);
        bool Consume(size_t size);

        std::span<const std::byte> GetView(size_t offset, size_t size);

        std::byte* GetBuffer() const { return pointer; }

        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] size_t Size() const;
        [[nodiscard]] size_t SpaceAvailable() const;
        [[nodiscard]] size_t SpaceAllocated() const;

    private:
        std::vector<std::byte> ringBuffer;

        std::byte* pointer;
        size_t size;

        std::byte* head;
        std::byte* tail;
    };
} // namespace OZZ::util