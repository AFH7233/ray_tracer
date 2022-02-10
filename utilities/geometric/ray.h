#ifndef _RAY_
    #define _RAY_

    #include <tgmath.h>
    #include <stdbool.h>
    #include "algebra.h"

    #define new_ray(a, b) (ray) { .origin=a, .direction=b}
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
        normal surface_normal;
        vector point;
    };

    ray diffuse_ray(normal surface_normal, vector surface_point);
    ray specular_ray(normal surface_normal, vector surface_point, ray incoming_ray, double angle_spread);
    ray refract_ray(normal surface_normal, vector surface_point, ray incoming_ray, double angle_spread, double cosT, double n);

#endif