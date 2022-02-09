
#include "plane.h"
#include <stdio.h>

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
    normal axis_x = new_normal(1.0, 0.0, 0.0);
    normal axis_y = new_normal(0.0, 1.0, 0.0);
    normal axis_z = new_normal(0.0, 0.0, 1.0);
    
    double aligned_x = fabs(dot(circle->surface_normal, axis_x));
    if(fabs(aligned_x - 1.0) < ERROR){
        box aabb = {
            .max_x = circle->center.x + 0.01,
            .max_y = circle->center.y + circle->radio,
            .max_z = circle->center.z + circle->radio,
            .min_x = circle->center.x - 0.01,
            .min_y = circle->center.y - circle->radio,
            .min_z = circle->center.z - circle->radio,
            .center = circle->center
        };
        return aabb;
    }

    double aligned_y = fabs(dot(circle->surface_normal, axis_y));
    
    if(fabs(aligned_y - 1.0) < ERROR){
        box aabb = {
            .max_x = circle->center.x + circle->radio,
            .max_y = circle->center.y + 0.01,
            .max_z = circle->center.z + circle->radio,
            .min_x = circle->center.x - circle->radio,
            .min_y = circle->center.y - 0.01,
            .min_z = circle->center.z - circle->radio,
            .center = circle->center
        };
        return aabb;
    }

    double aligned_z = fabs(dot(circle->surface_normal, axis_z));
    if(fabs(aligned_z - 1.0) < ERROR){
        box aabb = {
            .max_x = circle->center.x + circle->radio,
            .max_y = circle->center.y + circle->radio,
            .max_z = circle->center.z + 0.01,
            .min_x = circle->center.x - circle->radio,
            .min_y = circle->center.y - circle->radio,
            .min_z = circle->center.z - 0.01,
            .center = circle->center
        };
        return aabb;
    }
    //Create a vector basis to find the edges of the circle
    normal axis_temp = new_normal(0.0, 0.0, 1.0);
    if(fabs(aligned_z) < ERROR){
        axis_temp = new_normal(1.0, 0.0, 0.0);
    }

    normal axis_m = multiply(to_normal(cross(circle->surface_normal, axis_temp)), circle->radio);
    axis_m.w = 1.0;
    normal axis_p = multiply(to_normal(cross(circle->surface_normal, axis_m)), circle->radio);
    axis_p.w = 1.0;

    vector up = add_vector(circle->center, axis_m);
    vector down = add_vector(circle->center, multiply(axis_m, -1.0));
    vector left = add_vector(circle->center, axis_p);
    vector right = add_vector(circle->center, multiply(axis_p, -1.0));

    box aabb = {
        .max_x = fmax(fmax(up.x, down.x), fmax(left.x, right.x)),
        .max_y = fmax(fmax(up.y, down.y), fmax(left.y, right.y)),
        .max_z = fmax(fmax(up.z, down.z), fmax(left.z, right.z)),
        .min_x = fmin(fmin(up.x, down.x), fmin(left.x, right.x)),
        .min_y = fmin(fmin(up.y, down.y), fmin(left.y, right.y)),
        .min_z = fmin(fmin(up.x, down.x), fmin(left.x, right.x)),
        .center = circle->center
    };

    return aabb;
}

double get_plane_area(plane *restrict  circle){
    return M_PI*circle->radio*circle->radio;
}