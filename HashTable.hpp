#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "List.hpp"

#define TABLE_CAPACITY 1000

template <typename T>
class HashTable {
private:
  unsigned int size;
  unsigned int totalEntries;
  List<T> *table; // Array of collision buckets used for the chains

  unsigned long hashFunction(unsigned int key) {
    unsigned long hash = 0;
    hash = ((key >> 16) ^ key) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash);
    return hash % size;
  }
public:
  HashTable(unsigned int capacity=TABLE_CAPACITY)
  : size(capacity), totalEntries(0) { table = new List<T>[size]; }
  ~HashTable() { delete[] table; }

  unsigned int getSize() const { return size; }
  unsigned int getTotalEntries() const { return totalEntries; }

  void insert(unsigned int key, T node);
  void remove(unsigned int key, T node);

  T *search(unsigned int key, T node);
  void print() const;
};


#endif