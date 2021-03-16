#include <iostream>

#include "List.hpp"
#include "Person.hpp"
#include "Virus.hpp"
#include "Country.hpp"
#include "Record.hpp"
#include "VirusCountryEntry.hpp"

template <typename T>
void List<T>::insertFirst(T data) {
  listNode *node = createNode(data);
  if (!this->head) {
    node->next = NULL;
    this->tail = node;
  } else {
    node->next = this->head;
  }
  this->head = node;
  this->size++;
}

template <typename T>
void List<T>::insertLast(T data) {
  listNode *node = createNode(data);
  if (!this->head) {
    this->head = node;
  } else {
    this->tail->next = node;
  }
  this->tail = node;
  this->size++;
}

template <typename T>
void List<T>::insertAscending(T data) {
  if (empty() || this->head->data >= data) {
    insertFirst(data);
  } else if (this->tail->data <= data) {
    insertLast(data);
  } else if (this->size == 1) {
    (this->head->data >= data)? insertFirst(data):insertLast(data);
  } else {
    listNode *node = createNode(data);
    listNode *current = this->head;
    listNode *next = current->next;
    for (; next->data < data; next = current->next)
      current = current->next;
    node->next = next;
    current->next = node;
    this->size++;
  }
}

template <typename T>
void List<T>::insertDescending(T data) {
  if (empty() || this->head->data < data) {
    insertFirst(data);
  } else if (this->tail->data > data) {
    insertLast(data);
  } else if (this->size == 1) {
    (this->head->data < data) ? insertFirst(data) : insertLast(data);
  } else {
    listNode *node = createNode(data);
    listNode *current = this->head;
    listNode *next = current->next;
    for (; next->data >= data; next = current->next)
      current = current->next;
    node->next = next;
    current->next = node;
    this->size++;
  }
}

template <typename T>
void List<T>::popFirst() {
  if (empty()) return;
  listNode *temp = this->head;
  this->head = this->head->next;
  if (!this->head) {
    this->tail = NULL;
  } else {
    if (!this->head->next)
      this->tail = this->head;
  }
  delete temp;
  this->size--;
}

template <typename T>
void List<T>::popLast() {
  if (empty()) return;
  listNode *temp = this->tail;
  if (this->head->next) {
    listNode *newTail = this->head;
    while (newTail->next != this->tail)
      newTail = newTail->next;
    newTail->next = NULL;
    this->tail = newTail;
  } else {
    this->head = this->tail = NULL;
  }
  delete temp;
  this->size--;
}

template <typename T>
void List<T>::popValue(T data) {
  if (empty() || !search(data)) return;
  if (this->head->data == data) {
    popFirst();
  } else if (this->tail->data == data) {
    popLast();
  } else {
    listNode *current = this->head, *previous = NULL;
    while (current && current->data != data) {
      previous = current;
      current = current->next;
    }
    previous->next = current->next;
    delete current;
    size--;
  }
}

template <typename T>
List<T> &List<T>::sortAscending(List<T> &l) {
  if (empty()) return *this;
  List<T> newList;
  listNode *temp = l.head;
  for (; temp; temp = temp->next)
    newList.insertAscending(temp->data);
  l.flush();
  temp = newList.head;
  for (; temp; temp = temp->next)
    l.insertLast(temp->data);
  return l;
}

template <typename T>
List<T> &List<T>::sortDescending(List<T> &l) {
  l.sortAscending(l);
  l.invert();
  return l;
}

template <typename T>
List<T> &List<T>::invert() {
  if(empty()) return *this;
  listNode *current = this->head;
  listNode *next = NULL, *previous = NULL;
  while (current) {
    next = current->next;
    current->next = previous;
    previous = current;
    current = next;
  }
  this->head = previous;
  return *this;
}

template <typename T>
void List<T>::print() const {
  if (empty()) return;
  listNode *temp = this->head;
  for (; temp->next; temp = temp->next)
    std::cout << temp->data << " ==> ";
  std::cout << temp->data << std::endl;
}

template <typename T>
T *List<T>::getNode(unsigned int pos) {
  if (pos > size) return NULL;
  listNode *temp = head;
  for (; pos > 0; pos--)
    temp = temp->next;
  T *ptr = &temp->data;
  return ptr;
}

template <typename T>
T *List<T>::search(const T data) const {
  if (empty()) return NULL;
  listNode *temp = this->head;
  T *ptr = NULL;
  while (temp && temp->data != data)
    temp = temp->next;
  if (temp) ptr = &temp->data;
  return ptr;
}

template <typename T>
bool List<T>::contains(const T &data) const {
  if (empty()) return false;
  listNode *temp = this->head;
  while (temp && temp->data != data)
    temp = temp->next;
  return temp;
}

// Explicit initiation of template to help the compiler see it
template class List<char>;
template class List<int>;
template class List<long>;
template class List<float>;
template class List<double>;
template class List<std::string>;
template class List<Person>;
template class List<Virus>;
template class List<Country>;
template class List<Record>;
template class List<VirusCountryEntry>;
// ... and so on ...