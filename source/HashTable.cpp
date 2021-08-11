#include <iostream>

#include "../header/HashTable.hpp"
#include "../header/Person.hpp"
#include "../header/VirusCountryEntry.hpp"

template <typename T>
void HashTable<T>::insert(std::string key, T node) {
  unsigned long index = hashFunction(key);
  table[index].insertFirst(node);
  totalEntries++;
}

template <typename T>
void HashTable<T>::remove(std::string key, T node) {
  unsigned long index = hashFunction(key);
  table[index].popValue(node);
  totalEntries--;
}

template <typename T>
T *HashTable<T>::search(std::string key, T node) {
  unsigned long index = hashFunction(key);
  T *temp = NULL;
  temp = table[index].search(node);
  return temp;
}

template <typename T>
void HashTable<T>::print() const {
  for (unsigned int bucket=0; bucket<size; bucket++) {
    if (!table[bucket].empty()) {
      std::cout << "|| Index " << bucket << " || ";
      table[bucket].print();
      // std::cout << std::endl;
    }
  }
}

// Explicit initiation of template to help the compiler see it
template class HashTable<int>;
template class HashTable<std::string>;
template class HashTable<Person>;
template class HashTable<VirusCountryEntry>;
// ... and so on ...