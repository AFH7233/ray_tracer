#ifndef _SCENE_
    #define _SCENE_
    #include "json.h"
    #include "geometric/sphere.h"
    #include "geometric/plane.h"
    #include "geometric/algebra.h"
    #include "camera.h"
    #include "object.h"
    #include "dynamic_array.h"
    #include "obj_reader.h"
    #include "stl_reader.h"



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
    #define SCALE "scale"
    #define OBJ "OBJ"

    #define RAYTRACEABLE_OBJECTS  "objects"
    #define RAYTRACEABLE_REPEAT "repeat"
    #define FILE_PATH "path"

    #define OBJECT_REPETITIONS "repetitions"
    #define MIN_TAG "min"
    #define MAX_TAG "max"
    #define HEMISPHERE_TAG "hemisphere"
    #define NORMAL_TAG "normal"
    #define SPREAD_TAG "spread"
    #define AMBIENT_COLOR_TAG "ambient color"
    #define OUTPUT_TAG "output"
    #define PLANE_TAG "plane"

    #define ROTATION_X_TAG "rx"
    #define ROTATION_Y_TAG "ry"
    #define ROTATION_Z_TAG "rz"

    #define TRANSLATION_X_TAG "tx"
    #define TRANSLATION_Y_TAG "ty"
    #define TRANSLATION_Z_TAG "tz"

    #define TRANSFORMATIONS_TAG "transformations"
    #define REFRACTIVE_INDEX_TAG "refractive index"
    #define ABSORBING_TAG "absorbing"


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
        color_RGB ambient_color;
        list* objects;
        char* output_path;
    };

    typedef struct  mesh_container mesh_container;
    struct mesh_container
    {
        size_t length;
        object* triangles;
    };
    
    enum file_type_surface {FILE_TYPE_SURFACE_OBJ, FILE_TYPE_SURFACE_STL, FILE_TYPE_SURFACE_UNKNOWN};
    typedef enum file_type_surface file_type_surface;

    scene read_scene(char* const file_name, object_array* garbage);
#endif
