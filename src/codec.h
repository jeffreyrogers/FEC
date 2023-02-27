#pragma once

#include <vector>

namespace FEC {

class InvalidSourceLength : std::exception {
public:
  const char *what() const noexcept {
    return "Invalid source length. Input data cannot be expanded to byte "
           "boundary";
  }
};

class InvalidDataLength : std::exception {
public:
  const char *what() const noexcept {
    return "Invalid data length. Data cannot be decoded to byte boundary";
  }
};

class DecodingError : std::exception {
public:
  const char *what() const noexcept {
    return "Data could not be decoded. Too many errors.";
  }
};

class Codec {
public:
  virtual std::vector<uint8_t> encode(std::vector<uint8_t> &data) = 0;
  virtual std::vector<uint8_t> decode(std::vector<uint8_t> &data) = 0;
};

} // namespace FEC
