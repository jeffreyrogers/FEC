#pragma once

#include "codec.h"

namespace FEC {

// Golay(24, 12, 8)
class Golay : Codec {
public:
  std::vector<uint8_t> encode(std::vector<uint8_t> &data) { return data; }

  std::vector<uint8_t> decode(std::vector<uint8_t> &data) { return data; }
};

} // namespace FEC
