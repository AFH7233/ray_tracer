#ifndef _SCENE_
    #define _SCENE_
    #include "json.h"
    #include "sphere.h"
    #include "camera.h"
    #include "object.h"
    #include "dynamic_array.h"

    #define RAYTRACEABLE_TYPE  "type"

    #define CAMERA  "camera"
    #define CAMERA_FOV  "fov"
    #define CAMERA_POSITION "position"
    #define CAMERA_UP  "up"

    #define VECTOR_X "x"
    #define VECTOR_Y "y"
    #define VECTOR_Z "z"

    #define COLOR "color"

    #define COLOR_R "r"
    #define COLOR_G "g"
    #define COLOR_B "b"

    #define FOCUS "focus"

    #define SPHERE  "sphere"
    #define RADIUS  "radius"
    #define CENTER  "center"

    #define MATERIAL "material"
    #define MATERIAL_EMMITANCE "emmitance"
    #define MATERIAL_SPREAD "angle of spread"
    #define MATERIAL_P_DIFF "probability diffuse"

    #define OBJ_FILE    "file"
    #define WIDTH   "width"
    #define HEIGHT  "height"
    #define NUMBER_OF_THREADS "threads"
    #define RAYS_PER_PIXEL "rays"
    #define NUMBER_OF_BOUNCES "bounces"

    #define RAYTRACEABLE_OBJECTS  "objects"


    typedef struct scene scene;
    struct scene
    {
        camera camara;
        vector focus;
        double width;
        double height;
        size_t threads;
        size_t rays_per_pixel;
        size_t bounces;
        list* objects;
    };
    


    scene read_scene(char* const file_name, object_array* garbage);
#endif