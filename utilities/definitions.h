#ifndef _RAY_TRACING_DEFINITIONS_
    #define _RAY_TRACING_DEFINITIONS_

    #include <stdlib.h>
    #include <stdio.h>

    #include "image.h"
    #include "camera.h"
    #include "algebra.h"
    #include "ray.h"
    #include "object.h"
    #include "sphere.h"

    #define RAND(a, b)  ( a + ERROR + ( b + (-1.0 * a )) * (rand() / (double) RAND_MAX) )

    #define new_sphere_object(geometria, material) (object) { \
        .geometry = geometria, \
        .material = material, \
        .get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition, \
        .transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation \
    } 

#endif