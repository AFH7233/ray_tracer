#ifndef _LIST_
    #define _LIST_
    #include <stdlib.h>
    #include <stdio.h>

    typedef struct  list list;
    struct list
    {
        void* value;
        list* next;
    };
    
    list* new_list();
    list* add_node(list* head,void* value);
    void free_list(list* head);
    
#endif