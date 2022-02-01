#ifndef _POLYGON_
    #define _POLYGON_

    #include "algebra.h"
    #include "ray.h"
    #include "box.h"

    typedef struct polygon polygon;
    struct polygon
    {
        size_t num_vertices;
        vector* vertices;
        normal* normals;
        bool is_transformed;
    };
    

    typedef struct face face;
    struct  face
    {
        polygon* cloud;
        size_t indices_vertex[3];
        size_t indices_normals[3]; 
    };


    geometry_collition get_face_collition(face *restrict surface, ray pixel_ray);
    void transform_face_with_mutation(matrix transformation, face* surface);
    box get_face_bounding_box(face *restrict  surface);
    double get_face_area(face *restrict  surface);
    
#endif