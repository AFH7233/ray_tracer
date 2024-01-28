#ifndef _SPHERE_
#define _SPHERE_

#include "algebra.h"
#include "ray.h"
#include "box.h"

#define new_sphere(r, o) (sphere) { .radio = r , .center = o }
#define fill_allocated_sphere(s, r, o) { s->radio = r ; s->center = o; }

typedef struct sphere sphere;
struct sphere {
  double radio;
  vector center;
};

geometry_collition get_sphere_collition(sphere *restrict bola, ray pixel_ray);
void transform_sphere_with_mutation(matrix transformation, sphere *bola);
box get_sphere_bounding_box(sphere *restrict  bola);
double get_sphere_area(sphere *restrict  bola);
//    sphere transform_sphere(matrix transformation, sphere bola);

#endif
