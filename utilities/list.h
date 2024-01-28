#ifndef _LIST_
#define _LIST_
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct list list;
struct list {
  bool is_visited;
  void *value;
  list *next;
};

typedef struct garbage garbage;
struct garbage {
  size_t cap;
  size_t length;
  void *bin;
};

list *new_list(void);
list *add_node(list *head, void *value);
list *push_node(list *head, void *value);
void free_list(list *head);

#endif
