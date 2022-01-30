#ifndef _SPHERE_
    #define _SPHERE_

    #include "algebra.h"
    #include "ray.h"

    #define new_sphere(r, o) (sphere) { .radio = r , .center = o }

    typedef struct sphere sphere;
    struct sphere
    {
        double radio;
        vector center;
    };

    geometry_collition get_sphere_collition(sphere* bola, ray ray);
    void transform_sphere_with_mutation(matrix transformation, sphere* bola);
//    sphere transform_sphere(matrix transformation, sphere bola);
    
#endif