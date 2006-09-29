#ifndef TREEMAP_H
#define TREEMAP_H

#ifdef DEBUG
#include <iostream>
#endif

template <class Value> class TreeMap {
private:
  class Entry {
  public:
    int position;
    Value* value;
    Entry* parent;
    Entry* left;
    Entry* right;

    Entry(int position, Value* value) {
      Entry::position = position;
      Entry::value = value;
      parent = 0;
      left = 0;
      right = 0;
    }

    ~Entry() {
      delete left;
      delete right;
    }

#ifdef DEBUG
    void print() {
      using namespace std;
      cout << "[" << position << ", ";
      if (left) left->print(); else cout << 0;
      cout << ", ";
      if (right) right->print(); else cout << 0;
      cout << "]";
    }
#endif
  };

  Entry* root;
  Entry* front;
  Entry* it;
  int mapSize;

  Entry* successorOf(Entry* e) {
    if (e->right) {
      e = e->right;
      while (e->left) e = e->left;
    } else {
      Entry* p = e->parent;
      while (p && p->right == e) {
	e = p;
	p = p->parent;
      }
      e = p;
    }
    return e;
  }

public:
  TreeMap() {
    root = 0;
    front = 0;
    it = 0;
    mapSize = 0;
  }

  ~TreeMap() {
    delete root;
    delete front;
  }

  void add(int position, Value* value) {
    Entry* entry = new Entry(position, value);
    ++mapSize;
    if (front == 0 || front->position > position) front = entry;
    if (root == 0) {
      root = entry;
      return;
    }

    Entry* e = root;
    while (true) {
      if (e->position <= entry->position) {
	if (e->right) e = e->right;
	else {
	  e->right = entry;
	  entry->parent = e;
	  return;
	}
      } else {
	if (e->left) e = e->left;
	else {
	  e->left = entry;
	  entry->parent = e;
	  return;
	}
      }
    }
  }

  void beginIteration() {
    it = front;
  }

  Value* next() {
    if (it) {
      Value* ret = it->value;
      it = successorOf(it);
      return ret;
    } else {
      return 0;
    }
  }

  int size() {
    return mapSize;
  }

  void clear() {
    delete root;
    root = 0;
    front = 0;
    it = 0;
    mapSize = 0;
  }

#ifdef DEBUG
  void print() {
    using namespace std;
    root->print();
    cout << endl;
  }
#endif
};

#endif //TreeMap_H
