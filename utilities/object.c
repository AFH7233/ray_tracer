#include "object.h"

collition get_collition(object* object_3D, ray pixel_ray){
    geometry_collition collision_data = object_3D->get_geometry_collition(object_3D->geometry, pixel_ray);
    collition result = {
        .is_hit = collision_data.is_hit,
        .distance = collision_data.distance,
        .normal = collision_data.normal,
        .point = collision_data.point,
        .material = object_3D->material
    };
    return result;
}