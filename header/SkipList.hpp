#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#include <ctime>
#include <cstdlib>

#define DEFAULT_LEVELS 5
#define PROBABILITY 0.5

static bool seeded = false;

template <typename T>
class SkipList {
private:
  unsigned int maxLevel;
  struct skipNode {
    T data;
    skipNode **nextAtLevel;
  };
  unsigned int size;
  skipNode *head, *const tail;
  const double p;

  // Creates a random number of levels up to maxLevel
  unsigned int generateLevels() {
    unsigned int level = 0;
    double probability = 0.0;
    while (probability < p && level < maxLevel) {
      probability = ((double) rand()/RAND_MAX);
      level++;
    }
    return level;
  }

  skipNode *createNode(T data, unsigned int lvls = DEFAULT_LEVELS) {
    skipNode *node = new skipNode;
    node->data = data;
    node->nextAtLevel = new skipNode *[lvls];
    for (int i = 0; i < lvls; i++)
      node->nextAtLevel[i] = tail;
    return node;
  }

  void deleteNode(skipNode *node) {
    if (!node) return;
    delete[] node->nextAtLevel;
    delete node;
  }

  // Copies the given list recursively so that
  // the final form is identical with the original
  void recCopy(skipNode *node) {
    if (node) {
      recCopy(node->nextAtLevel[0]);
      insert(node->data);
    }
  }

  // Deletes all nodes except the arbitary head node
  void flush() { while (!empty()) remove(getFirst()); }

public:
  SkipList(unsigned int lvls=DEFAULT_LEVELS, double probability=PROBABILITY)
  : maxLevel(lvls), size(0), tail(NULL), p(PROBABILITY) {
    if(!seeded) { srand(time(NULL)); seeded=true; }
    this->head = createNode(-1, maxLevel);
  }
  ~SkipList() {
    flush();
    delete[] head->nextAtLevel; 
    delete head;
    delete tail;
  }
  SkipList(const SkipList &l);

  SkipList &operator=(const SkipList &l);

  unsigned int getMaxLevel() const { return maxLevel; }
  unsigned int getSize() const { return size; }
  T &getFirst() { return head->nextAtLevel[0]->data; }

  void insert(const T data);
  void remove(const T data);

  bool empty() const { return !size; }
  void print() const;
  T *getNode(unsigned int pos);
  T *search(const T data) const;
};

#endif