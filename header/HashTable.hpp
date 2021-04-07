#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "List.hpp"
#include "hashFunctions.hpp"

#define TABLE_CAPACITY 1000

template <typename T>
class HashTable {
private:
  unsigned int size;
  unsigned int totalEntries;
  List<T> *table; // Array of collision buckets used for the chains

  unsigned long hashFunction(std::string key) {
    return hash_i((unsigned char *)key.c_str(), key.length()) % size;
  }
public:
  HashTable(unsigned int capacity=TABLE_CAPACITY)
  : size(capacity), totalEntries(0) { table = new List<T>[size]; }
  ~HashTable() { delete[] table; }

  unsigned int getSize() const { return size; }
  unsigned int getTotalEntries() const { return totalEntries; }

  void insert(std::string key, T node);
  void remove(std::string key, T node);

  T *search(std::string key, T node);
  void print() const;
};


#endif