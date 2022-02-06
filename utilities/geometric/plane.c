
#include "plane.h"

geometry_collition get_plane_collition(plane *restrict circle, ray pixel_ray){
    double p_dot_n = dot(circle->surface_normal ,pixel_ray.direction);
    if(fabs(p_dot_n) < ERROR) {
        geometry_collition result = {.is_hit = false};
        return result;
    }
    double t = dot(sub_vector(circle->center, pixel_ray.origin), circle->surface_normal)/p_dot_n;

    if(t < ERROR) {
        geometry_collition result = {.is_hit = false};
        return result;
    }

    vector point = get_ray_point(pixel_ray, t);
    vector point_to_center = sub_vector(point, circle->center);
    double distance_to_center = norma(point_to_center);

    if(distance_to_center > circle->radio){
        geometry_collition result = {.is_hit = false};
        return result;
    }

    geometry_collition result = {.is_hit = true};
    result.point = point;
    result.distance = t;
    result.surface_normal = circle->surface_normal;
    return result;
}


void transform_plane_with_mutation(matrix transformation, plane* circle){
    circle->center = trasnform(transformation, circle->center);
    circle->surface_normal = trasnform(transformation, circle->surface_normal);
}


box get_plane_bounding_box(plane *restrict  circle){
    //This is an overkill but easier, besides intersection with plane si fast
    box aabb = {
        .max_x = circle->center.x + circle->radio,
        .max_y = circle->center.y + circle->radio,
        .max_z = circle->center.z + circle->radio,
        .min_x = circle->center.x - circle->radio,
        .min_y = circle->center.y - circle->radio,
        .min_z = circle->center.z - circle->radio,
        .center = circle->center
    };
    return aabb;
}

double get_plane_area(plane *restrict  circle){
    return M_PI*circle->radio*circle->radio;
}