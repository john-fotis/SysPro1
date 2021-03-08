#ifndef LIST_HPP
#define LIST_HPP

template <typename T>
struct listNode {
  T data;
  struct listNode *next;
};

template <typename T>
class List {
private:
  unsigned int size;
  listNode<T> *head;
  listNode<T> *tail;

  // Copies the given list recursively so that 
  // the final form is identical with the original
  void recCopy(listNode<T> *node) {
    if (node) {
      recCopy(node->next);
      insertFirst(node->data);
    }
  }
  // Empties the list
  void flush() { while(!empty()) popFirst(); }
public:
  List() { size = 0; head = tail = NULL; }
  List(const List &l) { size = 0; head = tail = NULL; recCopy(l.head); }
  ~List() { flush(); }

  unsigned int getSize() { return size; }
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
  void print() const;
  listNode<T> *search(T data) const;
};

#endif