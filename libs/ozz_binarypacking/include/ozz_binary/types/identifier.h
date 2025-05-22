//
// Created by paulm on 5/13/25.
//

#pragma once

namespace OZZ::binary {
    enum class TypeIdentifier : uint8_t {
        Int8 = 0x00,
        Int16 = 0x01,
        Int32 = 0x02,
        Int64 = 0x03,
        UInt8 = 0x04,
        UInt16 = 0x05,
        UInt32 = 0x06,
        UInt64 = 0x07,
        Float = 0x08,
        Double = 0x09,
        String = 0x0A,
        Boolean = 0x0B,
        Array = 0x0C,
        Packet = 0x0C,
    };
}