#ifndef _JSON_
#define _JSON_

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

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
#define MAX_STRING_SIZE 200
#define MAX_STRING_SIZE_COPY (MAX_STRING_SIZE-1)
#define MAP_SIZE 499

enum json_type {
  TAG,
  OBJECT_BEGIN,
  OBJECT_END,
  VALUE_BOOLEAN,
  VALUE_STRING,
  VALUE_NULL,
  VALUE_NUMBER,
  OBJECT,
  ARRAY_BEGIN,
  ARRAY_END
};
typedef enum json_type json_type;

enum value_type { JSON_NUMBER, JSON_ARRAY, JSON_OBJECT, JSON_COLLISION, JSON_STRING, JSON_BOOLEAN, JSON_NULL };
typedef enum value_type value_type;

enum parse_state { JSON_SEARCH, TAG_SEARCH, VALUE_SEPARATOR_SEARCH, VALUE_SEARCH, SEPARATOR_SEARCH, ARRAY_SEARCH };
typedef enum parse_state parse_state;

typedef struct parse_tree parse_tree;
struct parse_tree {
  json_type type;
  char *value;
};

typedef union json_value json_value;
union json_value {
  char *string;
  double number;
  bool boolean;
};

typedef struct json_object json_object;
struct json_object {
  value_type type;
  char *tag;
  union {
    json_value value;
    struct {
      size_t length;
      size_t cap;
      json_object **map;
    };
  };

};

json_object *read_json(char *const file_name);
json_object *get_json_object(json_object root[1], char *key);
json_object *get_json_element(json_object root[1], size_t index);
void free_json(json_object root[1]);

#endif
