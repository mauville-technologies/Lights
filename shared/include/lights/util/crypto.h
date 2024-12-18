//
// Created by ozzadar on 2024-12-17.
//

#pragma once

#include "sha.h"
#include "filters.h"
#include "hex.h"
#include "osrng.h"
#include <string>

namespace OZZ {
    inline std::string GenerateSalt(size_t length) {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::SecByteBlock buffer(length);
        rng.GenerateBlock(buffer, buffer.size());

        std::string salt;
        CryptoPP::ArraySource(buffer, buffer.size(), true,
                              new CryptoPP::HexEncoder(
                                      new CryptoPP::StringSink(salt)
                              )
        );

        return salt;
    }

    inline std::string HashPassword(const std::string& password) {
        CryptoPP::SHA512 hash;
        std::string digest;

        CryptoPP::StringSource(password,
                               true,
                               new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));

        return digest;
    }

    inline bool VerifyPassword(const std::string& password, const std::string& hash) {
        return HashPassword(password) == hash;
    }
}