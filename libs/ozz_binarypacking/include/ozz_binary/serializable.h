//
// Created by paulm on 5/13/25.
//

#pragma once
#include <span>

#include "types/identifier.h"

namespace OZZ::binary {
    template <typename T>
    concept BinaryArrayType = std::same_as<T, std::span<uint8_t>> ;

    template <typename T>
    concept SerializableType  = requires (T v) {
        {v} -> std::convertible_to<std::span<uint8_t>>;
    };

    template <typename ...T>
    // all types must be of SerializableType
    requires (SerializableType<T> && ...)
    struct Packet {
        // Store instances of each type
        std::tuple<T...> values;

        // Default constructor
        Packet() = default;

        // Constructor with values
        explicit Packet(T... args) : values(std::forward<T>(args)...) {}

        // Access element by index
        template<std::size_t I>
        auto& get() { return std::get<I>(values); }

        template<std::size_t I>
        const auto& get() const { return std::get<I>(values); }

        // Optional: Convert entire packet to bytes
        operator std::span<uint8_t>() {
            _buffer.clear();

            // put the bytes into the buffer
            size_t totalSize = 0;
            std::apply([&totalSize](auto&&... args) {
                ((totalSize += std::span<uint8_t>(args).size()), ...);
            }, values);

            size_t headerSize = sizeof(size_t) + 1;
            _buffer.resize(headerSize + totalSize);

            // packet header
            _buffer[0] = static_cast<uint8_t>(TypeIdentifier::Packet);
            for (size_t i = 0; i < sizeof(size_t); ++i) {
                _buffer[1 + i] = static_cast<uint8_t>((totalSize >> (i * 8)) & 0xFF);
            }

            std::apply([This = this, &headerSize](auto&&... args) {
                size_t offset = 0;
                auto copyElement = [This, headerSize](auto&& arg, size_t& offset) {
                    auto span = std::span<uint8_t>(arg);
                    std::ranges::copy(span, This->_buffer.data() + headerSize + offset);
                    offset += span.size();
                };

                ((copyElement(args, offset)) , ...);
            }, values);

            return std::span(_buffer);
        }

    private:
        std::vector<uint8_t> _buffer;
    };
}
