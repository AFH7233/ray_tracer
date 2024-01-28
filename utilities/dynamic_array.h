#ifndef _ARRAY_
#define _ARRAY_

#include <stdlib.h>

typedef struct object_array object_array;
struct object_array {
  size_t cap;
  size_t length;
  void **elements;
};

object_array new_array(void);
object_array new_array_with_cap(size_t length);
void array_add(object_array *arr, size_t index, void *value);
void array_push(object_array *arr, void *value);
void *array_pop(object_array *arr);
void *array_peek(object_array *arr);
void *array_get(object_array *arr, size_t index);
void free_array(object_array *arr);
#endif
