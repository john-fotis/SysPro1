#ifndef LIST_HPP
#define LIST_HPP

template <typename T>
class List {
private:
  struct listNode {
    T data;
    listNode *next;
  };
  unsigned int size;
  listNode *head;
  listNode *tail;

  listNode *createNode(T data) {
    listNode *node = new listNode();
    node->data = data;
    return node;
  }

  // Copies the given list recursively so that 
  // the final form is identical with the original
  void recCopy(listNode *node) {
    if (node) {
      recCopy(node->next);
      insertFirst(node->data);
    }
  }

public:
  List(): size(0), head(NULL), tail(NULL) {}
  List(const List &l): size(0), head(NULL), tail(NULL) { recCopy(l.head); }
  ~List() { flush(); }

  unsigned int getSize() const { return size; }
  T &getFirst() { return head->data; }
  T &getLast() { return tail->data; }

  void insertFirst(const T data);
  void insertLast(const T data);
  void insertAscending(const T data);
  void insertDescending(const T data);
  void popFirst();
  void popLast();
  void popValue(T data);

  List<T> &sortAscending(List<T> &l);
  List<T> &sortDescending(List<T> &l);
  List<T> &invert();

  bool empty() const { return (size==0 && head==NULL && tail==NULL); }
  void flush() { while(!empty()) popFirst(); } // Empties the list
  
  void print() const;
  T *getNode(unsigned int pos);
  T *search(const T data) const;
  bool contains(const T &data) const;
};

#endif