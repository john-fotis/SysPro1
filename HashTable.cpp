#include <iostream>
#include "HashTable.hpp"
#include "Record.hpp"

template <typename T>
void HashTable<T>::insert(unsigned int key, T node) {
  unsigned long index = hashFunction(key);
  table[index].insertFirst(node);
  totalEntries++;
}

template <typename T>
void HashTable<T>::remove(unsigned int key, T node) {
  unsigned long index = hashFunction(key);
  table[index].popValue(node);
  totalEntries--;
}

template <typename T>
void HashTable<T>::print() const {
  for (int bucket=0; bucket<size; bucket++) {
    if (!table[bucket].empty()) {
      std::cout << "|| Index " << bucket << " || ";
      table[bucket].print();
      // std::cout << std::endl;
    }
  }
}

template <typename T>
listNode<T> *HashTable<T>::search() const {
  listNode<T> *temp = NULL;
  return temp;
}

// Explicit initiation of template to help the compiler see it
// template class HashTable<int>;
template class HashTable<std::string>;
template class HashTable<Record>;

// ... and so on ...