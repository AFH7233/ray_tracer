#ifndef _OBJECT_3D_
    #define _OBJECT_3D_

    #include "color.h"
    #include "ray.h"
    #include <stdbool.h>

    typedef struct properties properties;
    struct properties
    {
        color_RGB color;
    };
    

    typedef struct object object;
    struct object
    {
        properties material;
        void* geometry;
        geometry_collition (*get_geometry_collition) (void* , ray);
        void (*transform_geometry) (matrix, void*);
    };


    typedef struct collition collition;
    struct collition
    {
        bool is_hit;
        double distance;
        normal normal;
        vector point;
        properties material;
    };

    collition get_collition(object* object_3D, ray pixel_ray);

    
#endif