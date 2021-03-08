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
  List<T> *table; // Array of collision buckets

  unsigned long hashFunction(long key) {
    unsigned long hash = 0;
    hash = ((key >> 16) ^ key) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash);
    return hash % size;
  }
public:
  HashTable(unsigned int capacity=TABLE_CAPACITY) {
    size = capacity;
    totalEntries = 0;
    table = new List<T>[size];
  }
  ~HashTable() { delete[] table; }
  // HashTable &(HashTable &t) {size=t.size; totalEntries = t.totalEntries; copy(t.table);}

  void insert(unsigned int key, T data);
  void remove(unsigned int key, T data);

  void print() const;
  listNode<T> *search() const;
};


#endif