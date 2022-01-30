#include "sphere.h"


 geometry_collition get_sphere_collition(sphere* bola, ray ray){

    vector ray_sphere = sub_vector(ray.origin, bola->center);
    double b = dot(ray.direction, ray_sphere);
    double discriminante = b*b - (dot(ray_sphere,ray_sphere) - bola->radio*bola->radio); 
    if(discriminante < 0){
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
        result.point  = get_ray_point(ray, t);
        result.normal = to_normal(sub_vector(result.point, bola->center));
        result.distance = t;
        return result;
    }
    
 }


void transform_sphere_with_mutation(matrix transformation, sphere* bola){
    bola->center = trasnform(transformation, bola->center);
}


/* sphere transform_sphere(matrix transformation, sphere bola){
    vector moved_center = trasnform(transformation, bola.center);
    sphere moved_sphere = new_sphere(
        bola.radio,
        moved_center
    );
    return moved_sphere;
 }*/