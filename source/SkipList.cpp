#include <iostream>

#include "../header/SkipList.hpp"
#include "../header/Record.hpp"

template <typename T>
SkipList<T>::SkipList(const SkipList &l)
: maxLevel(DEFAULT_LEVELS), tail(NULL), p(PROBABILITY) {
  if (this == &l) return;
  flush();
  recCopy(l.head->nextAtLevel[0]);
}

template <typename T>
SkipList<T> &SkipList<T>::operator=(const SkipList &l) {
  if (this == &l) return *this;
  flush();
  recCopy(l.head->nextAtLevel[0]);
  return *this;
}

template <typename T>
void SkipList<T>::insert(const T data) {
  // Array of pointers on every level which will 
  // be affected by the insertion of the new node
  skipNode *previousAtLevel[maxLevel];
  // Initialize them all pointing to the tail (NULL)
  for (int i = 0; i < maxLevel; i++)
    previousAtLevel[i] = tail;

  skipNode *temp = head;
  for (int i = maxLevel-1; i >= 0; i--) {
    while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
      temp = temp->nextAtLevel[i];
    // Save the last node on the current level before we have to move lower
    previousAtLevel[i] = temp;
  }

  // Don't insert duplicates
  if (temp->data != data) {
    // Create the new node with random number of maxLevel up to max allowed
    unsigned int randomLevels = generateLevels();
    skipNode *node = createNode(data, randomLevels);

    // Traverse from the top levels to the bottom and fix pointers affected
    for (int i = randomLevels-1; i >= 0; i--) {
      // Link the new node right after the last node we traversed in each level
      node->nextAtLevel[i] = previousAtLevel[i]->nextAtLevel[i];
      // Rearrange the pointers before new node at each level
      previousAtLevel[i]->nextAtLevel[i] = node;
    }
    this->size++;
  }
}

template <typename T>
void SkipList<T>::remove(const T data) {
  // Array of pointers on every level which will
  // be affected by the removal of the node
  skipNode *previousAtLevel[maxLevel];
  // Initialize them all pointing to the tail (NULL)
  for (int i = 0; i < maxLevel; i++)
    previousAtLevel[i] = tail;

  skipNode *temp = head;
  for (int i = maxLevel-1; i >= 0; i--) {
    while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
      temp = temp->nextAtLevel[i];
    // Save the last node on the current level before we have to move lower
    previousAtLevel[i] = temp;
  }

  // The next node at level zero MIGHT be the one we are looking for
  // as we stopped somewhere at level zero where temp->next[0] is >= data or NULL
  // To finoud out if this is the node we're looking for, we take the next step
  temp = temp->nextAtLevel[0];

  // Delete if found
  if (temp->data == data) {
    // Traverse from the top levels to the bottom and fix pointers affected
    for (int i = maxLevel-1; i >= 0; i--)
      // Only rearrange the nodes that we previous of the removed node at some level
      if (previousAtLevel[i]->nextAtLevel[i] == temp)
        previousAtLevel[i]->nextAtLevel[i] = temp->nextAtLevel[i];
    deleteNode(temp);
    this->size--;
  }
}

template <typename T>
void SkipList<T>::print() const {
  for (int i = maxLevel-1; i >= 0; i--) {
    // Skip the arbitrary head node as its value is negative
    skipNode *temp = head->nextAtLevel[i];
    std::cout << "Level\t" << i << ": ";
    while (temp) {
      std::cout << temp->data << " ";
      temp = temp->nextAtLevel[i];
    }
    std::cout << std::endl;
  }
}

template <typename T>
T *SkipList<T>::getNode(unsigned int pos) {
  if (pos > size) return NULL;
  skipNode *temp = head->nextAtLevel[0];
  for (; pos > 0; pos--)
    temp = temp->nextAtLevel[0];
  T *ptr = &temp->data;
  return ptr;
}

template <typename T>
T *SkipList<T>::search(const T data) const {
  if (empty()) return NULL;
  skipNode *temp = head;
  for (int i = maxLevel-1; i >= 0; i--)
    while (temp->nextAtLevel[i] && data > temp->nextAtLevel[i]->data)
      temp = temp->nextAtLevel[i];

  // We stopped somewhere at level 0, where the next node is either the tail
  // or its >= data, so we set temp = temp->next[0] to see if we found it.
  temp = temp->nextAtLevel[0];
  if (temp && temp->data == data) return &temp->data;
  return NULL;
}

template class SkipList<int>;
template class SkipList<Record>;