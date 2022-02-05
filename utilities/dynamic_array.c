#include "dynamic_array.h"

object_array new_array(){
    void** elements = calloc(10, sizeof(unsigned char*));
    object_array arr = {
        .cap = 10,
        .length = 0,
        .elements = elements
    };
    return arr;
}

void array_push(object_array arr, void* value){
    array_add(arr, arr.length, value);
}

void array_add(object_array arr, size_t index, void* value){
    if(index < arr.cap){
        arr.elements[index] = value;
    } else {
        void** temp = arr.elements;
        arr.elements = calloc(2*arr.cap, sizeof(unsigned char*));
        for(size_t i=0; i<arr.length; i++){
            arr.elements[i] = temp[i];
        }
        free(temp);
        arr.cap = 2*arr.cap;
        arr.elements[arr.length] = value;
        arr.length++;               
    }
}


void* array_get(object_array arr, size_t index){
    if(index < arr.length){
        return arr.elements[index];
    } else {
        return NULL;
    }
}
void free_array(object_array arr){
    for(size_t i=0; i<arr.length; i++){
        if(arr.elements[i] != NULL){
            free(arr.elements[i]);
            arr.elements[i] = NULL;
        }
    }
}