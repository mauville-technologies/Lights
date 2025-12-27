//
// Created by ozzadar on 2025-12-26.
//

#pragma once
#include <span>
#include <vector>

namespace OZZ::util {
    struct RingBufferAllocation {
        std::byte* buffer{nullptr};
        size_t size{0};

        [[nodiscard]] bool IsValid() const { return buffer != nullptr && size > 0; }
    };

    class RingBuffer {
    public:
        explicit RingBuffer(size_t size);

        RingBufferAllocation Allocate(const size_t size);
        bool Consume(size_t size);

        std::span<const std::byte> GetView(size_t offset, size_t size);

        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] size_t Size() const;
        [[nodiscard]] size_t SpaceAvailable() const;
        [[nodiscard]] size_t SpaceAllocated() const;

    private:
        std::vector<std::byte> ringBuffer;
        std::byte* head;
        std::byte* tail;
    };
} // namespace OZZ::util