#ifndef BLOOMFILTER_HPP
#define BLOOMFILTER_HPP

#define CHAR_SIZE (sizeof(char)*8) // bits
#define BITS_IN_BYTE 8
#define HASH_FUNCTIONS_NUMBER 16

#include <iostream>

class BloomFilter {
private:
  unsigned int size; // in bits!
  unsigned int hashFunctionsNumber;
  char *bitArray;

  // Apply logical OR to the array with a mask (Mask Type: 0-0-0-0-0-0-0-0-1)
  void set(unsigned int pos) { bitArray[pos/CHAR_SIZE] |= (1 << pos%CHAR_SIZE); }
  // Apply logical AND to the array with a mask (Mask Type: 1-1-1-1-1-1-1-1-0)
  void reset(unsigned int pos) { bitArray[pos/CHAR_SIZE] &= ~(1 << pos%CHAR_SIZE); }
  // Check the result logical AND to the array with a mask (Mask Type: 0-0-0-0-0-0-0-0-1)
  bool checkBit(unsigned int pos) const { return ((bitArray[pos/CHAR_SIZE]) & (1 << pos%CHAR_SIZE)); }

public:
  // Input size of bitarray is expected in bytes, NOT bits
  BloomFilter(unsigned int sz=100, int functions=16)
  : size(sz*BITS_IN_BYTE), hashFunctionsNumber(functions) {
    bitArray = new char[sz]; // Allocating in bytes!
    for (unsigned int pos = 0; pos < size; pos++) reset(pos);
  }
  ~BloomFilter() { delete[] bitArray; }
  BloomFilter(const BloomFilter &filter);
  BloomFilter &operator=(const BloomFilter &filter);

  unsigned int getSize() const { return size; }
  unsigned int getFunctionsNumber() const { return hashFunctionsNumber; }

  void insert(const std::string &input);
  bool check(const std::string &input);
  // Prints the bitArray
  void arrayStatus() const;
};

#endif