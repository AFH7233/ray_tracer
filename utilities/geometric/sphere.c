#include "sphere.h"


 geometry_collition get_sphere_collition(sphere* bola, ray pixel_ray){

    vector ray_sphere = sub_vector(pixel_ray.origin, bola->center);
    double b = dot(pixel_ray.direction, ray_sphere);
    double discriminante = b*b - (dot(ray_sphere,ray_sphere) - bola->radio*bola->radio); 
    if(discriminante < ERROR){
        geometry_collition result = {.is_hit = false};
        return result;
    } else {
        double c = sqrt(discriminante);
        double t = (-b-c) > ERROR? -b-c: ((-b+c) > ERROR ? (-b+c):0.0);
        if(t < ERROR){
            geometry_collition result = {.is_hit = false};
            return result;
        }
        geometry_collition result = {.is_hit = true};
        result.point  = get_ray_point(pixel_ray, t);
        result.surface_normal = to_normal(sub_vector(bola->center, result.point));
        result.distance = t;
        return result;
    }
    
 }


void transform_sphere_with_mutation(matrix transformation, sphere *restrict  bola){
    bola->center = trasnform(transformation, bola->center);
}


box get_sphere_bounding_box(sphere *restrict  bola){
    box aabb = {
        .max_x = bola->center.x + bola->radio,
        .max_y = bola->center.y + bola->radio,
        .max_z = bola->center.z + bola->radio,
        .min_x = bola->center.x - bola->radio,
        .min_y = bola->center.y - bola->radio,
        .min_z = bola->center.z - bola->radio,
        .center = bola->center
    };
    return aabb;
}

double get_sphere_area(sphere *restrict  bola){
    return 4*M_PI*bola->radio*bola->radio;
}
