    
#include "camera.h"

matrix get_look_at(camera camara, vector to){
    normal forward = to_normal(sub_vector(to, camara.position));
    normal side = cross(camara.up, forward);
    normal up = cross(forward, side);

    matrix ejes =  new_matrix(
        side.x, side.y, side.z, 0.0,
        up.x, up.y, up.z, 0.0,
        forward.x, forward.y, forward.z, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

   matrix translacion = new_matrix(
        1.0, 0.0, 0.0, -camara.position.x,
        0.0, 1.0, 0.0, -camara.position.y,
        0.0, 0.0, 1.0, -camara.position.z,
        0.0, 0.0, 0.0, 1.0
    );

    matrix look_at = mul_matrix(ejes, translacion);
    return look_at;
}
