
#ifndef __TODOLIST_H_
#define __TODOLIST_H_

#include <multiset.h>
#include <time.h>

#include "strings.h"

class TodoList;

class TodoListItem {
  String line;
  time_t when;
public:
  TodoListItem(String l, time_t w)
    : line(l), when(w) { }

  bool operator<(const TodoListItem &t) const
    { return when < t.when; }

  friend class TodoList;
};

class TodoList {

  multiset<TodoListItem, less<TodoListItem> > todoQueue;

public:
  TodoList();
  ~TodoList();

  void addDeban(String, String, time_t);

  String getNext();
};

#endif
