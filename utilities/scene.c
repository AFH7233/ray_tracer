#include "scene.h"

static scene to_scene(json_object* head, object_array* garbage);
static size_t get_integer(json_object* current);
static double get_double(json_object* current);
static vector get_vector(json_object* current, bool is_normal);
static color_RGB get_color(json_object* current);
static camera get_camera(json_object* current);
static properties get_material(json_object* current);
static list* get_objects(json_object* current, object_array* garbage);
static object* get_sphere(json_object* current, object_array* garbage);

scene read_scene(char* const file_name, object_array* garbage){
    json_object* json = read_json(file_name);
    scene escnea =  to_scene(json, garbage);
    free_json(json);
    return escnea;
}


static scene to_scene(json_object* json, object_array* garbage){
    scene empty = {
        .rays_per_pixel = 10,
        .bounces = 1,
        .focus = new_vector(0.0, 0.0, 100.0),
        .width = 640,
        .height = 480,
        .threads = 10,
    };

    empty.width = get_integer(get_json_object(json, WIDTH));
    if(empty.width < 1){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.width = 640;       
    }

    empty.height = get_integer(get_json_object(json, HEIGHT));
    if(empty.height < 1){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.height = 480;       
    }

    empty.bounces = get_integer(get_json_object(json, NUMBER_OF_BOUNCES));
    if(empty.bounces < 1){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.bounces = 2;       
    }

    empty.threads = get_integer(get_json_object(json, NUMBER_OF_THREADS));
    if(empty.threads < 1){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.threads = 10;       
    }

    empty.rays_per_pixel = get_integer(get_json_object(json, RAYS_PER_PIXEL));
    if(empty.rays_per_pixel < 1){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.rays_per_pixel = 10;       
    }

    empty.focus = get_vector(get_json_object(json, FOCUS),false);
    empty.camara = get_camera(get_json_object(json, CAMERA));
    empty.objects = get_objects(get_json_object(json, RAYTRACEABLE_OBJECTS), garbage);
    return empty;
}

static size_t get_integer(json_object* current){
    if(current != NULL && current->type == JSON_VALUE){
        int real_number = atoi(current->value);
        if(real_number < 0){
            fprintf(stderr, "Invalid number for the ray tracer\n");
            exit(EXIT_FAILURE);        
        }
        return real_number;
    } else {
        fprintf(stderr, "Cannot parse integer \n");
        exit(EXIT_FAILURE);
    }

}

static double get_double(json_object* current){
    if(current != NULL && current->type == JSON_VALUE){
        double real_number = atof(current->value);
        return real_number;
    } else {
        fprintf(stderr, "Cannot parse double \n");
        exit(EXIT_FAILURE);
    }
}

static vector get_vector(json_object* current, bool is_normal){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse vector \n");
        exit(EXIT_FAILURE);
    }

    vector point = {};

    if(is_normal) {
        point = new_normal(0.0,1.0,0.0);
    } else {
        point = new_vector(0.0, 0.0, 0.0);
    }

    point.x = get_double(get_json_object(current, VECTOR_X));
    point.y = get_double(get_json_object(current, VECTOR_Y));
    point.z = get_double(get_json_object(current, VECTOR_Z));

    if(is_normal){
        return to_normal(point);
    } else {
        return point;
    }
}

static color_RGB get_color(json_object* current){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse color \n");
        exit(EXIT_FAILURE);
    }
    color_RGB color = {};

    color.red = get_double(get_json_object(current, COLOR_R));
    if(color.red > 255.0){
        fprintf(stderr, "Limiting to 255 \n");
        color.red = 255.0;
    } else if(color.red < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.red = COLOR_ERROR;  
    }
    color.red = color.red/255.0;

    color.green = get_double(get_json_object(current, COLOR_G));
    if(color.green > 255.0){
        fprintf(stderr, "Limiting to 255 \n");
        color.green = 255.0;
    } else if(color.green < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.green = COLOR_ERROR;  
    }
    color.green = color.green/255.0;

    color.blue = get_double(get_json_object(current, COLOR_B));
    if(color.blue > 255.0){
        fprintf(stderr, "Limiting to 255 \n");
        color.blue = 255.0;
    } else if(color.blue < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.blue = COLOR_ERROR;  
    }
    color.blue = color.blue/255.0;
    return color;
}

static properties get_material(json_object* current){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse material \n");
        exit(EXIT_FAILURE);
    }
    properties material = {
        .color = new_color_RGB(1.0,1.0,1.0),
        .emmitance = COLOR_ERROR,
        .p_diffract = 1.0,
        .angle_spread_reflect = 0.0
    };

    material.color = get_color(get_json_object(current, COLOR));
    material.emmitance = get_double(get_json_object(current, MATERIAL_EMMITANCE)) + COLOR_ERROR;
    material.p_diffract = get_double(get_json_object(current, MATERIAL_P_DIFF));
    material.angle_spread_reflect  = get_double(get_json_object(current, MATERIAL_SPREAD));

    if(material.p_diffract < 0.0 || material.p_diffract > 1.0){
        fprintf(stderr, "Invalid probability, setting default \n");
        material.p_diffract = 0.5;
    } 

    if(material.angle_spread_reflect < 0.0 || material.angle_spread_reflect > 360.0){
        fprintf(stderr, "Invalid angle, setting default \n");
        material.angle_spread_reflect = ERROR;
    } 

    return material;
}


static camera get_camera(json_object* current){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse camera \n");
        exit(EXIT_FAILURE);
    }
    camera camara = {};

    camara.fov = get_double(get_json_object(current, CAMERA_FOV));
    camara.position = get_vector(get_json_object(current, CAMERA_POSITION), false);  
    camara.up = get_vector(get_json_object(current, CAMERA_UP), true);  


    if(camara.fov < 0.0){
        fprintf(stderr, "Invalid fov, setting default \n");
        camara.fov = 75;
    } 

    if(norma(camara.up) < ERROR){
        fprintf(stderr, "Invalid up vector, setting default \n");
        camara.up = new_normal(0.0,1.0,0.0);
    } 

    return camara;
}

static list* get_objects(json_object* current, object_array* garbage){
    if(current == NULL || current->type != JSON_ARRAY){
        fprintf(stderr, "Cannot parse objects \n");
        exit(EXIT_FAILURE);
    }
    list* objects = new_list();
    for(size_t i=0; i<current->length; i++){
        
        json_object* object_raytraceable = get_json_element(current, i);
        if(object_raytraceable == NULL || object_raytraceable->type != JSON_OBJECT){
            fprintf(stderr, "Invalid internal object \n");
            exit(EXIT_FAILURE);
        }

        json_object* tag = get_json_object(object_raytraceable, RAYTRACEABLE_TYPE);
        if(tag == NULL || tag->type != JSON_VALUE){
            fprintf(stderr, "Invalid tag object \n");
            exit(EXIT_FAILURE);
        }

        if(strncmp(tag->value, SPHERE, MAX_STRING_SIZE) == 0){
            
            object* sphere_object = get_sphere(object_raytraceable, garbage);
            objects = add_node(objects, sphere_object);
        } else {
            fprintf(stderr, "Unkown object \n");
            exit(EXIT_FAILURE);            
        }
    }
    return objects;
}

static object* get_sphere(json_object* current, object_array* garbage){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse objects \n");
        exit(EXIT_FAILURE);
    }
    sphere* sphere_geometry = malloc(sizeof(sphere));
    array_push(garbage, sphere_geometry);
    sphere_geometry->radio = get_double(get_json_object(current, RADIUS));
    if(sphere_geometry->radio < 0.0){
        fprintf(stdout, "Ilgeal radius setting up default \n");
        sphere_geometry->radio= 2.0;
    }

    sphere_geometry->center = get_vector(get_json_object(current, CENTER), false);

    properties sphere_material = get_material(get_json_object(current, MATERIAL));

    object* sphere_object = malloc(sizeof(object));
    array_push(garbage, sphere_object);
    sphere_object->geometry = sphere_geometry; 
    sphere_object->material = sphere_material; 
    sphere_object->get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition; 
    sphere_object->transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation; 
    sphere_object->get_bounding_box = (box (*) (void*)) get_sphere_bounding_box; 

    sphere_object->bounding_box  = get_sphere_bounding_box(sphere_geometry); 
    sphere_object->surface_area = get_sphere_area(sphere_geometry); 
    return sphere_object;
}
