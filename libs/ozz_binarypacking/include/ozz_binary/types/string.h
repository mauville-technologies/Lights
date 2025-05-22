//
// Created by paulm on 5/13/25.
//

#pragma once

namespace OZZ::binary {

    struct String {
        String() : String("") {};
        String(const std::string& inValue) {
            Bytes.resize(inValue.size() + 1);
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::String);

            for (size_t i = 0; i < inValue.size(); ++i) {
                Bytes[i + 1] = static_cast<uint8_t>(inValue[i]);
            }
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::String;
        }

        operator std::string_view() {
            return {reinterpret_cast<const char*>(Bytes.data() + 1), Bytes.size() - 1};
        }

        std::vector<uint8_t> Bytes;
    };
}