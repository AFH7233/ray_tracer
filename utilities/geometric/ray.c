#include "ray.h"

ray diffuse_ray(normal surface_normal, vector surface_point){
    normal difracted_direction = random_sphere_direction(surface_normal, 180.0);
    return new_ray(
        surface_point,
        difracted_direction
    );
}

ray specular_ray(normal surface_normal, vector surface_point, ray incoming_ray, double angle_spread){
    normal specular_direction = to_normal(sub_vector(incoming_ray.direction, multiply(surface_normal,2.0*dot(incoming_ray.direction, surface_normal))));
    if(angle_spread < ERROR ){
        return new_ray(
            surface_point,
            specular_direction
        );
    } else {
        normal specular_direction_spreaded = random_sphere_direction(specular_direction, angle_spread);
        return new_ray(
            surface_point,
            specular_direction_spreaded
        );
    }

}

ray refract_ray(normal surface_normal, vector surface_point, ray incoming_ray, double angle_spread, double cosT, double n){
    normal refracted_direction = to_normal(sub_vector(multiply(incoming_ray.direction, n), multiply(surface_normal, cosT )));
    if(angle_spread < ERROR ){
        return new_ray(
            surface_point,
            refracted_direction
        );
    } else {
        normal refracted_direction_spreaded = random_sphere_direction(refracted_direction, angle_spread);
        return new_ray(
            surface_point,
            refracted_direction_spreaded
        );
    }

}
