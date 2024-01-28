#include "list.h"

list *new_list(void) {
  list *head = malloc(sizeof(list));
  if (head == NULL) {
    fprintf(stderr, "No memory \n");
    exit(EXIT_FAILURE);
  }
  head->value = NULL;
  head->next = NULL;
  head->is_visited = false;
  return head;
}

list *add_node(list *head, void *value) {
  head->value = value;
  list *new_head = new_list();
  new_head->next = head;
  new_head->is_visited = false;
  return new_head;
}

list *push_node(list *head, void *value) {
  head->value = value;
  list *new_head = new_list();
  head->next = new_head;
  new_head->is_visited = false;
  return new_head;
}

void free_list(list *head) {
  list *current = head->next;
  while (current != NULL) {
    if (current->value != NULL) {
      free(current->value);
    }
    list *temp = current;
    current = current->next;
    free(temp);
  }
  free(head);
  return;

}
