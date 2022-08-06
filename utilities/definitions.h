#ifndef _RAY_TRACING_DEFINITIONS_
    #define _RAY_TRACING_DEFINITIONS_

    #include <stdlib.h>
    #include <stdio.h>

    #include "geometric/algebra.h"
    #include "geometric/ray.h"
    #include "geometric/polygon.h"
    #include "geometric/sphere.h"
    #include "geometric/plane.h"

    #include "object.h"
    #include "image.h"
    #include "camera.h"
    #include "list.h"
    #include "obj_reader.h"
    #include "dynamic_array.h"
    #include "json.h"
    #include "thread_util.h"
    #include "bvh.h"
    

    // geometria must be a pointer to a sphere
    #define new_sphere_object(geometria, mat) (object) { \
        .geometry = geometria, \
        .material = mat, \
        .get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition, \
        .transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation, \
        .bounding_box  = get_sphere_bounding_box(geometria), \
        .surface_area = get_sphere_area(geometria), \
        .get_bounding_box = (box (*) (void*)) get_sphere_bounding_box \
    } 

    #define new_polygon_object(geometria, mat) (object) { \
        .geometry = geometria, \
        .material = mat, \
        .get_geometry_collition = (geometry_collition (*) (void*, ray)) get_face_collition, \
        .transform_geometry = (void (*) (matrix, void*))  transform_face_with_mutation, \
        .bounding_box  = get_face_bounding_box(geometria), \
        .surface_area = get_face_area(geometria), \
        .get_bounding_box = (box (*) (void*)) get_face_bounding_box \
    } 

    #define fill_allocated_sphere_object(o, geometria, mat) { \
        o->geometry = geometria; \
        o->material = mat; \
        o->get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition; \
        o->transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation; \
        o->bounding_box  = get_sphere_bounding_box(geometria); \
        o->surface_area = get_sphere_area(geometria); \
        o->get_bounding_box = (box (*) (void*)) get_sphere_bounding_box; \
    } 


#endif
