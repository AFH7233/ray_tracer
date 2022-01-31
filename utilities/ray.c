#include "ray.h"

ray diffuse_ray(normal surface_normal, vector surface_point){
    normal difracted_direction = random_sphere_direction(surface_normal, 180.0);
    return new_ray(
        surface_point,
        difracted_direction
    );
}