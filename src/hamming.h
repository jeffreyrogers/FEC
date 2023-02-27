#pragma once

#include <bit>

#include "codec.h"

namespace FEC {

// Hamming(8, 4)
class Hamming : Codec {
public:
  // Parity-Check Matrix
  // 1 0 1 0 1 0 1
  // 0 1 1 0 0 1 1
  // 0 0 0 1 1 1 1
  std::vector<uint8_t> decode(std::vector<uint8_t> &data) {
    std::vector<uint8_t> decoded;

    if ((data.size() % 2) != 0) {
      throw InvalidDataLength();
    }

    for (size_t i = 0; i < data.size(); i += 2) {
      uint8_t byte1 = data[i];
      uint8_t byte2 = data[i + 1];
      auto v = std::vector{byte1, byte2};

      uint8_t hi;
      uint8_t lo;

      for (size_t j = 0; j < v.size(); j++) {
        uint8_t e = v[j];
        uint8_t parity = std::popcount(e) % 2;

        // get error location
        uint8_t b1 = (e & 0b1000'0000) >> 7;
        uint8_t b2 = (e & 0b0100'0000) >> 6;
        uint8_t b3 = (e & 0b0010'0000) >> 5;
        uint8_t b4 = (e & 0b0001'0000) >> 4;
        uint8_t b5 = (e & 0b0000'1000) >> 3;
        uint8_t b6 = (e & 0b0000'0100) >> 2;
        uint8_t b7 = (e & 0b0000'0010) >> 1;

        uint8_t e1 = b1 ^ b3 ^ b5 ^ b7;
        uint8_t e2 = b2 ^ b3 ^ b6 ^ b7;
        uint8_t e3 = b4 ^ b5 ^ b6 ^ b7;

        uint8_t error_loc = e1 | (e2 << 1) | (e3 << 2);

        if (parity != 0) {
          // flip invalid bit
          if (error_loc == 0) { // parity bit in error
            e ^= 1;
          } else {
            e ^= 1 << (7 - (error_loc - 1));
          }
        } else if (error_loc != 0) {
          // If we reach this condition, then the parity was 0 but the error
          // location indicates an error. This means that a double-bit error has
          // occurred, which we cannot correct.
          throw DecodingError();
        }

        uint8_t val = 0;
        val |= ((e & 0b0010'0000) >> 5) << 3;
        val |= ((e & 0b0000'1000) >> 3) << 2;
        val |= ((e & 0b0000'0100) >> 2) << 1;
        val |= (e & 0b0000'0010) >> 1;

        if (j == 0) {
          hi = val << 4;
        } else {
          lo = val;
        }
      }

      decoded.push_back(hi | lo);
    }

    return decoded;
  }

  // Generator Matrix
  // 1 1 0 1
  // 1 0 1 1
  // 1 0 0 0
  // 0 1 1 1
  // 0 1 0 0
  // 0 0 1 0
  // 0 0 0 1
  std::vector<uint8_t> encode(std::vector<uint8_t> &data) {
    std::vector<uint8_t> out;

    // TODO: can this be done with SIMD?
    for (auto &byte : data) {
      uint8_t hi = byte & 0xF0;
      uint8_t lo = (byte & 0x0F) << 4;

      for (auto v = std::vector{hi, lo}; auto &e : v) {
        uint8_t d1 = (e & 0b1000'0000) >> 7;
        uint8_t d2 = (e & 0b0100'0000) >> 6;
        uint8_t d3 = (e & 0b0010'0000) >> 5;
        uint8_t d4 = (e & 0b0001'0000) >> 4;

        uint8_t encoded = 0;
        encoded |= ((d1 ^ d2 ^ d4) << 7);
        encoded |= ((d1 ^ d3 ^ d4) << 6);
        encoded |= (d1 << 5);
        encoded |= ((d2 ^ d3 ^ d4) << 4);
        encoded |= (d2 << 3);
        encoded |= (d3 << 2);
        encoded |= (d4 << 1);

        // parity bit
        encoded |= (std::popcount(encoded) % 2);
        out.push_back(encoded);
      }
    }

    return out;
  }
};

} // namespace FEC
