#ifndef _RAY_
    #define _RAY_

    #include <tgmath.h>
    #include <stdbool.h>
    #include "algebra.h"

    #define new_ray(a, b) { .origin=a, .direction=b}
    #define get_ray_point(ray, distance) add_vector(ray.origin, scale_vector(ray.direction,distance))

    typedef struct ray ray;
    struct ray
    {
        vector origin;
        normal direction;
    };

    typedef struct geometry_collition geometry_collition;
    struct geometry_collition
    {
        bool is_hit;
        double distance;
        normal normal;
        vector point;
    };

#endif