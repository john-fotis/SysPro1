#include <cstring>

#include "../header/BloomFilter.hpp"
#include "../header/hashFunctions.hpp"

BloomFilter::BloomFilter(const BloomFilter &filter) {
  if (this == &filter) return;
  size = filter.getSize();
  delete[] bitArray;
  bitArray = new char[size/BITS_IN_BYTE]; // the size we copied was in bits!
  hashFunctionsNumber = filter.getFunctionsNumber();
  for (unsigned int pos = 0; pos < size; pos++)
    if (filter.checkBit(pos)) set(pos);
    else reset(pos);
}

BloomFilter &BloomFilter::operator=(const BloomFilter &filter) {
  if (this == &filter) return *this;
  size = filter.getSize();
  delete[] bitArray;
  bitArray = new char[size/BITS_IN_BYTE]; // the size we copied was in bits!
  hashFunctionsNumber = filter.getFunctionsNumber();
  for (unsigned int pos = 0; pos < size; pos++)
    if (filter.checkBit(pos)) set(pos);
    else reset(pos);
  return *this;
}

void BloomFilter::insert(const std::string &input) {
  // Insert input if we know it's NOT already present
  if (!check(input)) {
    unsigned long hash = 0;
    for (unsigned int i = 0; i < hashFunctionsNumber; i++) {
      hash = hash_i((unsigned char *)input.c_str(), i);
      set(hash % size);
    }
  }
}

bool BloomFilter::check(const std::string &input) {
  bool found = true;
  unsigned long hash = 0;
  for (unsigned int i = 0; i < hashFunctionsNumber; i++) {
    hash = hash_i((unsigned char *)input.c_str(), i);
    found = found && checkBit(hash % size);
  }
  return found;
}

void BloomFilter::arrayStatus() const {
  for (unsigned int i = 0; i < size; i++)
    std::cout << (checkBit(i) == true) << " ";
  std::cout << std::endl;
}