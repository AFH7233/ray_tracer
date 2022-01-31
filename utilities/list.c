#include "list.h"

list* new_list(){
    list* head  = malloc(sizeof(list));
    if(head == NULL){
        fprintf(stderr, "No memory \n");
        exit(EXIT_FAILURE);
    }
    head->value = NULL;
    head->next = NULL;
    return head;
}

list* add_node(list* head, void* value){
    head->value = value;
    list* new_head = new_list();
    new_head->next = head;
    return new_head;
}

void free_list(list* head){
    list* current = head;
    while (current != NULL){
        if(current->value != NULL){
            free(current->value);
        }
        list* temp = current;
        current = current-> next;
        free(temp);
    }
    return;
    
}