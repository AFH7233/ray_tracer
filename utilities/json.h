#ifndef _JSON_
    #define _JSON_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <ctype.h>
    #include "dynamic_array.h"
    #include "list.h"

    #define OBJECT_OPEN  '{'
    #define OBJECT_CLOSE  '}'
    #define ARRAY_OPEN  '['
    #define ARRAY_CLOSE  ']'
    #define SEPARATOR  ','
    #define TAG_DELIMITER  '\"'
    #define DEFINITION_START  ':'
    #define MAX_STRING_SIZE 100
    #define MAX_STRING_SIZE_COPY (MAX_STRING_SIZE-1)
    #define MAP_SIZE 499


    typedef enum json_type json_type;
    enum json_type {TAG, OBJECT_BEGIN, OBJECT_END, VALUE, OBJECT, ARRAY_BEGIN, ARRAY_END};

    typedef enum value_type value_type;
    enum value_type {JSON_VALUE, JSON_ARRAY, JSON_OBJECT, JSON_COLLISION};

    typedef enum parse_state parse_state;
    enum parse_state {JSON_SEARCH, TAG_SEARCH, VALUE_SEPARATOR_SEARCH, VALUE_SEARCH, SEPARATOR_SEARCH, ARRAY_SEARCH};

    typedef struct parse_tree parse_tree;
    struct parse_tree
    {
        json_type type;
        char* value;
    };

    typedef union json_value json_value;
    union json_value
    {
        char string[MAX_STRING_SIZE];
        double floating;
        int integer;
    };

    typedef struct json_object json_object;
    struct json_object
    {
        value_type type;
        char tag[MAX_STRING_SIZE];
        union 
        {
            char value[MAX_STRING_SIZE];
            struct {
                size_t length;
                size_t cap;
                json_object** map;
            };
        };
        
    };

    json_object* read_json(char* const file_name);
    json_object* get_json_object(json_object* root, char* key);
    json_object* get_json_element(json_object* root, size_t index);
    void free_json(json_object* root);

#endif