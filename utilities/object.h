#ifndef _OBJECT_3D_
    #define _OBJECT_3D_

    #include <stdbool.h>
    #include "box.h"
    #include "color.h"
    #include "ray.h"

    typedef struct properties properties;
    struct properties
    {
        color_RGB color;
        double emmitance;
        double p_diffract;
        double angle_spread_reflect;
    };
    

    typedef struct object object;
    struct object
    {
        box bounding_box;
        double surface_area;
        properties material;
        void* geometry;
        geometry_collition (*get_geometry_collition) (void* , ray);
        void (*transform_geometry) (matrix, void*);
        box (*get_bounding_box) (void*);
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

    collition get_collition(object *restrict object_3D, ray pixel_ray);
    void transform_object(matrix transformation, object* object_3D);
#endif