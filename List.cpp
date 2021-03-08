#include <iostream>
#include "List.hpp"
#include "Record.hpp"

template <typename T>
void List<T>::insertFirst(T data) {
  listNode<T> *node = new listNode<T>();
  node->data = data;
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
  listNode<T> *node = new listNode<T>();
  node->data = data;
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
    listNode<T> *node = new listNode<T>();
    node->data = data;
    listNode<T> *current = this->head;
    listNode<T> *next = current->next;
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
    listNode<T> *node = new listNode<T>();
    node->data = data;
    listNode<T> *current = this->head;
    listNode<T> *next = current->next;
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
  listNode<T> *temp = this->head;
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
  listNode<T> *temp = this->tail;
  if (this->head->next) {
    listNode<T> *newTail = this->head;
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
    listNode<T> *current = this->head, *previous = NULL;
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
  listNode<T> *temp = l.head;
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
  listNode<T> *current = this->head;
  listNode<T> *next = NULL, *previous = NULL;
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
  listNode<T> *temp = this->head;
  for (; temp->next; temp = temp->next)
    std::cout << temp->data << " ==> ";
  std::cout << temp->data << std::endl;
}

template <typename T>
listNode<T> *List<T>::search(T data) const {
  if (empty()) return NULL;
  listNode<T> *temp = this->head;
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
template class List<Record>;
// ... and so on ...