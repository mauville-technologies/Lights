//
// Created by ozzadar on 2025-12-26.
//

#include <lights/core/util/ring_buffer.h>
#include <spdlog/spdlog.h>

namespace OZZ::util {
    RingBuffer::RingBuffer(const size_t inSize) : ringBuffer(inSize), pointer(ringBuffer.data()), size(inSize) {
        head = tail = pointer;
    }

    RingBuffer::RingBuffer(std::byte* buffer, size_t inSize) : ringBuffer(0), pointer(buffer), size(inSize) {
        head = tail = pointer;
    }

    RingBufferAllocation RingBuffer::Allocate(const size_t reqSize) {
        spdlog::debug("Ringbuffer allocate: space available: {}", SpaceAvailable());
        if (SpaceAvailable() < reqSize) {
            return {};
        }
        const size_t headIndex = head - pointer;
        const size_t tailIndex = tail - pointer;

        // if (IsEmpty() && tail == head && head == pointer) {
        //     // whole buffer available, just move head
        //     auto* headCopy = head;
        //     head = head + reqSize;
        //     return {
        //         .offset = headIndex,
        //         .size = reqSize,
        //     };
        // }

        // is tail behind the head?
        if (headIndex >= tailIndex) {
            // is there enough free space at the end of the array?
            if (const auto distanceToEnd = size - headIndex; distanceToEnd >= reqSize) {
                auto* headCopy = head;
                head = head + reqSize;
                return {
                    .offset = headIndex,
                    .size = reqSize,
                };
            }

            // check front to tail for space
            const auto distanceToTail = tailIndex;
            if (distanceToTail > reqSize) {
                head = pointer + reqSize;
                return {
                    .offset = 0,
                    .size = reqSize,
                };
            }

            spdlog::error("Ringbuffer in weird state, investigate. No space available even though space available. "
                          "headIndex > tailIndex");
            return {};
        }

        // Tail ahead of head, check distance to be certain, move head
        const auto spaceAvailable = tailIndex - headIndex - 1;
        if (spaceAvailable >= reqSize) {
            auto* headCopy = head;
            head = head + reqSize;
            return {
                .offset = headIndex,
                .size = reqSize,
            };
        }

        spdlog::error("Ringbuffer in weird state, investigate. No space available even though space available. "
                      "headIndex < tailIndex");
        return {};
    }

    bool RingBuffer::Consume(const size_t size) {
        if (SpaceAllocated() < size) {
            spdlog::error("RingBuffer::Consume() -- requesting consumption of more data than was allocated");
            return false;
        }
        const auto tailIndex = tail - pointer;
        const auto newTailIndex = tailIndex + size;

        tail = pointer + (newTailIndex % Size());

        spdlog::debug("Ringbuffer Consume: new available: {}", SpaceAvailable());
        return true;
    }

    std::span<const std::byte> RingBuffer::GetView(size_t offset, size_t size) {
        if (offset > Size() || size > Size() - offset) {
            spdlog::warn("Requested ringbuffer view out of bounds.");
            return {};
        }
        return {pointer + offset, size};
    }

    bool RingBuffer::IsEmpty() const {
        return head == tail;
    }

    size_t RingBuffer::Size() const {
        return size;
    }

    size_t RingBuffer::SpaceAvailable() const {
        const auto headIndex = head - pointer;
        const auto tailIndex = tail - pointer;
        const auto size = Size();
        return (tailIndex - headIndex - 1 + size) % size;
    }

    size_t RingBuffer::SpaceAllocated() const {
        const auto headIndex = head - pointer;
        const auto tailIndex = tail - pointer;
        const auto size = Size();
        return (headIndex - tailIndex + size) % size;
    }
} // namespace OZZ::util