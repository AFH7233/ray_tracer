#ifndef _OBJ_READER_
    #define _OBJ_READER_

    #include <tgmath.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <float.h>
    #include "algebra.h"
    #include "object.h"
    #include "polygon.h"
    #include "list.h"
    #include "dynamic_array.h"


    typedef struct  list_vector list_vector;
    struct list_vector
    {
        vector value;
        list_vector* next;
    };

    typedef struct  list_face list_face;
    struct list_face
    {
        size_t length;
        size_t indices_vertex[4];
        list_face* next;
    };

    typedef struct  obj_container obj_container;
    struct obj_container
    {
        size_t length;
        object* triangles;
    };

    obj_container read_obj_file(char* fileName, double scale, properties material, object_array* garbage);
    void free_obj(obj_container arr);
#endif