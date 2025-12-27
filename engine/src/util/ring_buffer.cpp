//
// Created by ozzadar on 2025-12-26.
//

#include <lights/util/ring_buffer.h>
#include <spdlog/spdlog.h>

namespace OZZ::util {
    RingBuffer::RingBuffer(const size_t size) : ringBuffer(size) {
        head = tail = ringBuffer.data();
    }

    RingBufferAllocation RingBuffer::Allocate(const size_t size) {
        if (SpaceAvailable() < size) {
            return {};
        }

        if (IsEmpty()) {
            // whole buffer available, just move head
            auto* headCopy = head;
            head = head + size;
            return {
                .buffer = headCopy,
                .size = size,
            };
        }

        const auto headIndex = head - ringBuffer.data();
        const auto tailIndex = tail - ringBuffer.data();

        // is tail behind the head?
        if (headIndex > tailIndex) {
            // is there enough free space at the end of the array?
            if (const auto distanceToEnd = ringBuffer.size() - headIndex; distanceToEnd >= size) {
                auto* headCopy = head;
                head = head + size;
                return {
                    .buffer = headCopy,
                    .size = size,
                };
            }

            // check front to tail for space
            const auto distanceToTail = tailIndex;
            if (distanceToTail > size) {
                head = ringBuffer.data() + size;
                return {
                    .buffer = ringBuffer.data(),
                    .size = size,
                };
            }

            spdlog::error("Ringbuffer in weird state, investigate. No space available even though space available. "
                          "headIndex > tailIndex");
            return {};
        }

        // Tail ahead of head, check distance to be certain, move head
        const auto spaceAvailable = tailIndex - headIndex - 1;
        if (spaceAvailable >= size) {
            auto* headCopy = head;
            head = head + size;
            return {
                .buffer = headCopy,
                .size = size,
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
        const auto tailIndex = tail - ringBuffer.data();
        const auto newTailIndex = tailIndex + size;

        tail = ringBuffer.data() + (newTailIndex % Size());

        return true;
    }

    std::span<const std::byte> RingBuffer::GetView(size_t offset, size_t size) {
        if (offset > Size() || size > Size() - offset) {
            spdlog::warn("Requested ringbuffer view out of bounds.");
            return {};
        }
        return {ringBuffer.data() + offset, size};
    }

    bool RingBuffer::IsEmpty() const {
        return head == tail;
    }

    size_t RingBuffer::Size() const {
        return ringBuffer.size();
    }

    size_t RingBuffer::SpaceAvailable() const {
        const auto headIndex = head - ringBuffer.data();
        const auto tailIndex = tail - ringBuffer.data();
        const auto size = Size();
        return (tailIndex - headIndex - 1 + size) % size;
    }

    size_t RingBuffer::SpaceAllocated() const {
        const auto headIndex = head - ringBuffer.data();
        const auto tailIndex = tail - ringBuffer.data();
        const auto size = Size();
        return (headIndex - tailIndex + size) % size;
    }
} // namespace OZZ::util