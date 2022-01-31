#ifndef _RAY_TRACING_DEFINITIONS_
    #define _RAY_TRACING_DEFINITIONS_

    #include <stdlib.h>
    #include <stdio.h>

    #include "image.h"
    #include "camera.h"
    #include "algebra.h"
    #include "ray.h"
    #include "object.h"
    #include "bvh.h"
    #include "sphere.h"
    #include "list.h"

    // geometria must be a pointer to a sphere
    #define new_sphere_object(geometria, material) (object) { \
        .geometry = geometria, \
        .material = material, \
        .get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition, \
        .transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation, \
        .bounding_box  = get_sphere_bounding_box(geometria), \
        .surface_area = get_sphere_area(geometria), \
        .get_bounding_box = (box (*) (void*)) get_sphere_bounding_box \
    } 

    #define fill_allocated_sphere_object(o, geometria, material) { \
        o->geometry = geometria; \
        o->material = material; \
        o->get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition; \
        o->transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation; \
        o->bounding_box  = get_sphere_bounding_box(geometria); \
        o->surface_area = get_sphere_area(geometria); \
        o->get_bounding_box = (box (*) (void*)) get_sphere_bounding_box; \
    } 


#endif