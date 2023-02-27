#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "fec.h"

// use this instead of stdlib assert so it will still work if NDEBUG is defined
#define ASSERT_EQ(l, r)                                                        \
  if (l != r) {                                                                \
    std::cout << "ERROR at function " << __func__ << "(), line " << __LINE__   \
              << " in file " << __FILE__ "\n";                                 \
    exit(-1);                                                                  \
  }

#define ASSERT_NOT_EQ(l, r)                                                    \
  if (l == r) {                                                                \
    std::cout << "ERROR at function " << __func__ << "(), line " << __LINE__   \
              << " in file " << __FILE__ "\n";                                 \
    exit(-1);                                                                  \
  }

std::vector<uint8_t> generate_random_bytes(size_t len) {
  std::independent_bits_engine<std::default_random_engine, 8, uint8_t> rbe;
  std::vector<uint8_t> data(len);
  std::generate(begin(data), end(data), std::ref(rbe));
  return data;
}

std::vector<uint8_t> corrupt_data(std::vector<uint8_t> &in) {
  std::vector<uint8_t> out;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 7);

  for (auto &b : in) {
    // corrupt one bit per byte
    int bit_to_flip = distribution(generator);
    out.push_back(b ^ (1 << bit_to_flip));
  }

  return out;
}

void test_bit_patterns() {
  FEC::Hamming hamming_codec;

  std::vector<uint8_t> bytes;
  for (int i = 0; i < 16; i++) {
    bytes.push_back(i | (i << 4));
    // bytes.push_back(i << 4);
  }

  for (size_t i = 0; i < bytes.size(); i++) {
    std::cout << "Input:\t\t" << std::bitset<8>(bytes[i]) << std::endl;
    std::vector<uint8_t> tmp{bytes[i]};
    auto encoded = hamming_codec.encode(tmp);
    auto corrupted = corrupt_data(encoded);

    std::cout << "Encoded:\t";
    for (size_t j = 0; j < encoded.size(); j++) {
      std::cout << std::bitset<8>(encoded[j]) << " ";
    }
    std::cout << std::endl;

    std::cout << "Corrupted:\t";
    for (size_t j = 0; j < encoded.size(); j++) {
      std::cout << std::bitset<8>(corrupted[j]) << " ";
    }
    std::cout << std::endl;

    auto decoded = hamming_codec.decode(corrupted);
    std::cout << "Decoded:\t" << std::bitset<8>(decoded[0]) << std::endl
              << std::endl;
  }
}

void test_random_data() {
  FEC::Hamming hamming_codec;

  std::cout << "Generating Data... ";
  auto data = generate_random_bytes(1000000);
  std::cout << "Done." << std::endl;

  std::cout << "Hamming encoding data... ";
  auto hamming_encoded = hamming_codec.encode(data);
  std::cout << "Done." << std::endl;

  std::cout << "Corrupting one bit per byte... ";
  auto hamming_corrupted = corrupt_data(hamming_encoded);
  std::cout << "Done." << std::endl;

  for (size_t i = 0; i < hamming_encoded.size(); i++) {
    ASSERT_NOT_EQ(hamming_encoded[i], hamming_corrupted[i]);
  }

  auto hamming_decoded = hamming_codec.decode(hamming_corrupted);

  ASSERT_EQ(data.size(), hamming_decoded.size());

  for (size_t i = 0; i < data.size(); i++) {
    ASSERT_EQ(data[i], hamming_decoded[i]);
  }

  std::cout << "Data was decoded successfully." << std::endl;
}

int main() {
  test_bit_patterns();
  test_random_data();
}
