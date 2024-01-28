#include "dynamic_array.h"

object_array new_array(void) {
  void **elements = calloc(10, sizeof(unsigned char *));
  object_array arr = {
      .cap = 10,
      .length = 0,
      .elements = elements
  };
  return arr;
}

object_array new_array_with_cap(size_t capacity) {
  void **elements = calloc(capacity, sizeof(unsigned char *));
  object_array arr = {
      .cap = capacity,
      .length = 0,
      .elements = elements
  };
  return arr;
}

void array_push(object_array *arr, void *value) {
  array_add(arr, arr->length, value);
  arr->length++;
}

void *array_pop(object_array *arr) {
  if (arr->length > 0) {
    arr->length--;
    return arr->elements[arr->length];
  }
  return NULL;
}

void *array_peek(object_array *arr) {
  if (arr->length > 0) {
    return arr->elements[arr->length - 1];
  }
  return NULL;
}

void array_add(object_array *arr, size_t index, void *value) {
  if (index < arr->cap) {
    arr->elements[index] = value;
  } else {
    void **temp = arr->elements;
    arr->elements = calloc(2 * arr->cap, sizeof(unsigned char *));
    for (size_t i = 0; i < arr->length; i++) {
      arr->elements[i] = temp[i];
    }
    free(temp);
    arr->cap = 2 * arr->cap;
    arr->elements[index] = value;
  }
}

void *array_get(object_array *arr, size_t index) {
  if (index < arr->length) {
    return arr->elements[index];
  } else {
    return NULL;
  }
}
void free_array(object_array *arr) {
  for (size_t i = 0; i < arr->length; i++) {
    if (arr->elements[i] != NULL) {
      free(arr->elements[i]);
      arr->elements[i] = NULL;
    }
  }
}
