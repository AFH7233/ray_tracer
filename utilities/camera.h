#ifndef _CAMERA_
    #define _CAMERA_

    #include <tgmath.h>
    #include "geometric/algebra.h"

    typedef struct camera camera;
    struct camera
    {
        double fov;
        normal up;
        vector position;
    };
    
    matrix get_look_at(camera camera, vector to);
#endif
