
#include "todolist.h"

TodoList::TodoList()
{
  todoQueue.clear();
}

TodoList::~TodoList()
{ }

void
TodoList::addDeban(String channel, String mask, time_t when)
{
  TodoListItem tdli(String("MODE ") + channel +
                    " -b " + mask, when);
  todoQueue.insert(tdli);
}

String
TodoList::getNext()
{
  multiset<TodoListItem, less<TodoListItem> >::iterator it;
  time_t current_time = time(0);

  it = todoQueue.begin();

  if (it == todoQueue.end() || (*it).when > current_time)
    return "";

  String result = (*it).line;
  todoQueue.erase(it);

  return result;
}
