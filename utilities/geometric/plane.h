#ifndef _PLANE_
    #define _PLANE_

    #include "algebra.h"
    #include "ray.h"
    #include "box.h"

    typedef struct plane plane;
    struct plane
    {
        double radio;
        normal surface_normal;
        vector center;
    };

    geometry_collition get_plane_collition(plane *restrict circle, ray pixel_ray);
    void transform_plane_with_mutation(matrix transformation, plane* circle);
    box get_plane_bounding_box(plane *restrict  circle);
    double get_plane_area(plane *restrict  circle);

#endif