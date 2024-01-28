#include "object.h"

collition get_collition(object *restrict object_3D, ray pixel_ray) {
  geometry_collition collision_data = object_3D->get_geometry_collition(object_3D->geometry, pixel_ray);
  collition result = {
      .is_hit = collision_data.is_hit,
      .distance = collision_data.distance,
      .surface_normal = collision_data.surface_normal,
      .point = collision_data.point,
      .material = object_3D->material,
      .id = object_3D->id
  };
  return result;
}

void transform_object(matrix transformation, object *object_3D) {
  object_3D->transform_geometry(transformation, object_3D->geometry);
  object_3D->bounding_box = object_3D->get_bounding_box(object_3D->geometry);
  return;
}
