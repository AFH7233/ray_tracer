#ifndef _SCENE_READER_
    #define _SCENE_READER_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <ctype.h>
    #include "camera.h"
    #include "sphere.h"
    #include "object.h"
    #include "list.h"

    #define OBJECT_OPEN  '{'
    #define OBJECT_CLOSE  '}'
    #define ARRAY_OPEN  '['
    #define ARRAY_CLOSE  ']'
    #define SEPARATOR  ','
    #define TAG_DELIMITER  '\"'
    #define DEFINITION_START  ':'
    #define MAX_STRING_SIZE 100
    #define MAP_SIZE 500

    #define CAMERA  "camera"
    #define CAMERA_FOV  "fov"
    #define CAMERA_POSITION "position"
    #define CAMERA_UP  "up"

    #define VECTOR_X "x"
    #define VECTOR_Y "y"
    #define VECTOR_Z "z"

    #define COLOR "color"

    #define COLOR_R "r"
    #define COLOR_G "g"
    #define COLOR_B "b"

    #define FOCUS "focus"

    #define SPHERE  "sphere"
    #define RADIUS  "radius"
    #define CENTER  "center"

    #define MATERIAL "material"
    #define MATERIAL_EMMITANCE "emmitance"
    #define MATERIAL_SPREAD "angle of spread"
    #define MATERIAL_P_DIFF "probability diffuse"

    #define OBJ_FILE    "file"
    #define WIDTH   "width"
    #define HEIGHT  "height"
    #define NUMBER_OF_THREADS "threads"
    #define RAYS_PER_PIXEL "rays"
    #define NUMBER_OF_BOUNCES "bounces"

    typedef enum json_type json_type;
    enum json_type {TAG, OBJECT_BEGIN, OBJECT_END, VALUE, OBJECT, ARRAY_BEGIN, ARRAY_END};

    typedef enum parse_state parse_state;
    enum parse_state {JSON_SEARCH, TAG_SEARCH, VALUE_SEPARATOR_SEARCH, VALUE_SEARCH, SEPARATOR_SEARCH, ARRAY_SEARCH};

    typedef struct parse_tree parse_tree;
    struct parse_tree
    {
        json_type type;
        char* value;
    };

    typedef struct json_node json_node;
    struct  json_node
    {
        json_type type;
        char key[MAX_STRING_SIZE];
        void* value;
    };

    typedef struct scene scene;
    struct scene
    {
        camera camara;
        vector focus;
        double width;
        double height;
        size_t threads;
        size_t rays_per_pixel;
        size_t bounces;
        list* objects;
    };

    


    scene read_scene(char* const file_name);

#endif