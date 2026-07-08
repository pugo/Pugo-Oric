// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>
// =========================================================================

#ifndef TAPE_UTILS_HPP
#define TAPE_UTILS_HPP

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>


namespace tape_utils
{
/**
 * Compute SHA-1 hash of data.
 */
class Sha1
{
public:
    /**
     * Update hash with data.
     * @param data pointer to data
     * @param size size of data in bytes
     */
    void update(const uint8_t* data, size_t size)
    {
        total_size += size;

        while (size > 0) {
            const size_t bytes_to_copy = std::min(size, block.size() - block_size);
            std::memcpy(block.data() + block_size, data, bytes_to_copy);

            block_size += bytes_to_copy;
            data += bytes_to_copy;
            size -= bytes_to_copy;

            if (block_size == block.size()) {
                process_block(block.data());
                block_size = 0;
            }
        }
    }

    /**
     * Finalize hash and return digest.
     * @return SHA-1 digest as an array of 20 bytes
     */
    std::array<uint8_t, 20> digest()
    {
        const uint64_t bit_size = total_size * 8;

        block[block_size++] = 0x80;

        if (block_size > 56) {
            std::fill(block.begin() + block_size, block.end(), 0);
            process_block(block.data());
            block_size = 0;
        }

        std::fill(block.begin() + block_size, block.begin() + 56, 0);
        for (int i = 0; i < 8; ++i) {
            block[56 + i] = static_cast<uint8_t>(bit_size >> ((7 - i) * 8));
        }
        process_block(block.data());

        std::array<uint8_t, 20> result{};
        for (size_t i = 0; i < state.size(); ++i) {
            result[i * 4] = static_cast<uint8_t>(state[i] >> 24);
            result[i * 4 + 1] = static_cast<uint8_t>(state[i] >> 16);
            result[i * 4 + 2] = static_cast<uint8_t>(state[i] >> 8);
            result[i * 4 + 3] = static_cast<uint8_t>(state[i]);
        }
        return result;
    }

private:
    void process_block(const uint8_t* input)
    {
        std::array<uint32_t, 80> words{};

        for (size_t i = 0; i < 16; ++i) {
            words[i] = (static_cast<uint32_t>(input[i * 4]) << 24) |
                       (static_cast<uint32_t>(input[i * 4 + 1]) << 16) |
                       (static_cast<uint32_t>(input[i * 4 + 2]) << 8) |
                       static_cast<uint32_t>(input[i * 4 + 3]);
        }

        for (size_t i = 16; i < words.size(); ++i) {
            words[i] = std::rotl(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);
        }

        uint32_t a = state[0];
        uint32_t b = state[1];
        uint32_t c = state[2];
        uint32_t d = state[3];
        uint32_t e = state[4];

        for (size_t i = 0; i < words.size(); ++i) {
            uint32_t f = 0;
            uint32_t k = 0;

            if (i < 20) {
                f = (b & c) | (~b & d);
                k = 0x5a827999;
            }
            else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ed9eba1;
            }
            else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8f1bbcdc;
            }
            else {
                f = b ^ c ^ d;
                k = 0xca62c1d6;
            }

            const uint32_t temp = std::rotl(a, 5) + f + e + k + words[i];
            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = temp;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
    }

    std::array<uint32_t, 5> state{{
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
        0xc3d2e1f0,
    }};
    std::array<uint8_t, 64> block{};
    uint64_t total_size = 0;
    size_t block_size = 0;
};

inline std::string sha1_hex(const uint8_t* data, size_t size)
{
    Sha1 sha1;
    sha1.update(data, size);

    std::string result;
    result.reserve(40);

    for (const uint8_t byte : sha1.digest()) {
        result += std::format("{:02x}", byte);
    }

    return result;
}

}

#endif
