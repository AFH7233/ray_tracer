#ifndef _SCENE_READER_
    #define _SCENE_READER_

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <ctype.h>
    #include "camera.h"
    #include "object.h"
    #include "list.h"

    #define OBJECT_OPEN  '{'
    #define OBJECT_CLOSE  '}'
    #define SEPARATOR  ','
    #define TAG_DELIMITER  '\"'
    #define DEFINITION_START  ':'

    #define CAMERA  "camera"
    #define SPHERE  "sphere"
    #define OBJ_FILE    "file"
    #define WIDTH   "width"
    #define HEIGHT  "height"
    #define NUMBER_OF_THREADS "threads"
    #define RAYS_PER_PIXEL "rays"
    #define NUMBER_OF_BOUNCES "bounces"

    typedef enum json_type json_type;
    enum json_type {TAG, OBJECT_BEGIN, OBJECT_END, VALUE};

    typedef enum parse_state parse_state;
    enum parse_state {JSON_SEARCH, TAG_SEARCH, VALUE_SEARCH, SEPARATOR_SEARCH};


    typedef struct parse_tree parse_tree;
    struct parse_tree
    {
        json_type type;
        char* value;
    };
    

    typedef struct scene scene;
    struct scene
    {
        camera camara;
        double width;
        double height;
        size_t threads;
        size_t rays_per_pixel;
        size_t bounces;
        list* objects;
    };

    


    scene read_scene(char* const file_name);
#endif