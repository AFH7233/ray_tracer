#include "scene.h"

static scene to_scene(json_object* head, object_array* garbage);
static size_t get_integer(json_object* current);
static double get_double(json_object* current);
static double get_limit_double(json_object* current);
static double get_random_double(json_object* current);
static vector get_vector(json_object* current, bool is_normal);
static matrix get_transformaiton_matrix(json_object* current);
static vector get_vector_or_hemisphere(json_object* current, bool is_normal);
static color_RGB get_color(json_object* current);
static camera get_camera(json_object* current);
static properties get_material(json_object* current);
static list* get_objects(json_object* current, object_array* garbage);
static object* get_sphere(json_object* current, object_array* garbage);
static object* get_plane(json_object* current, object_array* garbage);
static mesh_container get_obj(json_object* current, object_array* garbage);
static file_type_surface get_file_type(char* file_name);

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
    if(empty.bounces < 0){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        empty.bounces = 1;       
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

    char* output_path = calloc(MAX_STRING_SIZE, sizeof(char));
    array_push(garbage, output_path);

    json_object* output = get_json_object(json, OUTPUT_TAG);
    if(output == NULL || output->type != JSON_OBJECT){
        fprintf(stderr, "Invalid path, defaulting to result.bmp\n");
        exit(EXIT_FAILURE);
    }

    json_object* file = get_json_object(output, FILE_PATH);
    if(file == NULL || file->type != JSON_STRING){
        fprintf(stderr, "Invalid path, defaulting to result.bmp\n");
        exit(EXIT_FAILURE);
    }  
    strncpy(output_path, file->value.string, MAX_STRING_SIZE); 

    empty.output_path = output_path;

    empty.ambient_color = get_color(get_json_object(json, AMBIENT_COLOR_TAG));

    empty.focus = get_vector_or_hemisphere(get_json_object(json, FOCUS),false);
    empty.camara = get_camera(get_json_object(json, CAMERA));
    empty.objects = get_objects(get_json_object(json, RAYTRACEABLE_OBJECTS), garbage);
    return empty;
}

static size_t get_integer(json_object* current){
    if(current != NULL && current->type == JSON_NUMBER){
        int real_number = (int) current->value.number;
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
    if(current != NULL && current->type == JSON_NUMBER){
        double real_number = current->value.number;
        return real_number;
    } else if(current->type == JSON_OBJECT) {
        return get_random_double(current);
    } else {
        fprintf(stderr, "Cannot parse double \n");
        exit(EXIT_FAILURE);
    }
}

//To avoid recursion with double
static double get_limit_double(json_object* current){
    if(current != NULL && current->type == JSON_NUMBER){
        double real_number =  current->value.number;
        return real_number;
    }  else {
        fprintf(stderr, "Cannot parse double \n");
        exit(EXIT_FAILURE);
    }
}

static double get_random_double(json_object* current){
    if(current != NULL && current->type == JSON_OBJECT){
        double min = get_limit_double(get_json_object(current, MIN_TAG));
        double max = get_limit_double(get_json_object(current, MAX_TAG));
        if(min > max){
            fprintf(stderr, "Max limit is smaller than min limit\n");
            return 0.0;
        }
        return RAND(min,max);
    } else {
        fprintf(stderr, "Cannot create double random\n");
        exit(EXIT_FAILURE);
    }
}

static vector get_vector_or_hemisphere(json_object* current, bool is_normal){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse vector \n");
        exit(EXIT_FAILURE);
    }
    json_object* hemisphere = get_json_object(current, HEMISPHERE_TAG);
    if(hemisphere != NULL){
        normal surface_normal = get_vector(get_json_object(hemisphere, NORMAL_TAG), true);
        vector center = get_vector(get_json_object(hemisphere, CENTER), false);
        double spread = get_double(get_json_object(hemisphere, SPREAD_TAG));
        if(spread < 0.0){
            fprintf(stderr, "Spread must be positive, defaulting to 0.0\n");
            spread = 0.0;
        }
        double radius = get_double(get_json_object(hemisphere, RADIUS));
        if(radius < 0.0){
            fprintf(stderr, "Radius must be positive, defaulting to 1.0\n");
            radius = 1.0;
        }
        vector point = random_sphere_direction(surface_normal, spread);
        point.w = 1.0;
        point = multiply(point, radius);
        point = add_vector(point, center);
        if(is_normal){
            return to_normal(point);
        } else {
            return point;
        }        

    } else {
        return get_vector(current, is_normal);
    }

   
}

static matrix get_transformaiton_matrix(json_object* current){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse transformation \n");
        exit(EXIT_FAILURE);
    }

    json_object* rotation_x = get_json_object(current, ROTATION_X_TAG);
    double rx = 0.0;
    if(rotation_x != NULL){
        rx = get_double(rotation_x);
    } 

    json_object* rotation_y = get_json_object(current, ROTATION_Y_TAG);
    double ry = 0.0;
    if(rotation_y != NULL){
        ry = get_double(rotation_y);
    } 

    json_object* rotation_z = get_json_object(current, ROTATION_Z_TAG);
    double rz = 0.0;
    if(rotation_z != NULL){
        rz = get_double(rotation_z);
    } 

    json_object* translation_x = get_json_object(current, TRANSLATION_X_TAG);
    double tx = 0.0;
    if(translation_x != NULL){
        tx = get_double(translation_x);
    } 

    json_object* translation_y = get_json_object(current, TRANSLATION_Y_TAG);
    double ty = 0.0;
    if(translation_y != NULL){
        ty = get_double(translation_y);
    } 

    json_object* translation_z = get_json_object(current, TRANSLATION_Z_TAG);
    double tz = 0.0;
    if(translation_z != NULL){
        tz = get_double(translation_z);
    } 

    return get_transformation(rx, ry, rz, tx, ty, tz);
}

static vector get_vector(json_object* current, bool is_normal){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse vector \n");
        exit(EXIT_FAILURE);
    }

    vector point = origin_vector;

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
    color_RGB color = {
        .red = 0,
        .green = 0,
        .blue = 0
    };

    color.red = get_double(get_json_object(current, COLOR_R));
    if(color.red > (255.0)){
        fprintf(stderr, "Limiting to 255 \n");
        color.red = 255.0 - (255.0*COLOR_ERROR);
    } else if(color.red < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.red = 0.0;  
    }
    color.red = color.red/255.0;

    color.green = get_double(get_json_object(current, COLOR_G));
    if(color.green >  (255.0 )){
        fprintf(stderr, "Limiting to 255 \n");
        color.green = 255.0 - (255.0*COLOR_ERROR);
    } else if(color.green < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.green = 0.0;  
    }
    color.green = color.green/254.0;

    color.blue = get_double(get_json_object(current, COLOR_B));
    if(color.blue >  (255.0 )){
        fprintf(stderr, "Limiting to 255 \n");
        color.blue = 255.0 - (255.0*COLOR_ERROR);
    } else if(color.blue < 0.0){
        fprintf(stderr, "No negative colors \n");
        color.blue = 0.0;  
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
        .is_absorbing = false,
    };

    material.color = get_color(get_json_object(current, COLOR));
    material.emmitance = get_double(get_json_object(current, MATERIAL_EMMITANCE)) + COLOR_ERROR;
    material.p_diffract = get_double(get_json_object(current, MATERIAL_P_DIFF));
    material.angle_spread_reflect  = get_double(get_json_object(current, MATERIAL_SPREAD));
    material.refractive_index = NAN;
    material.is_dielectric = false;
    material.is_absorbing = false; 

    json_object* refractive_index = get_json_object(current, REFRACTIVE_INDEX_TAG);
    if(refractive_index != NULL && refractive_index->type == JSON_NUMBER){
        material.is_dielectric = true;
        material.refractive_index = refractive_index->value.number;
    }

    if(material.p_diffract < 0.0 || material.p_diffract > 1.0){
        fprintf(stderr, "Invalid probability, setting default \n");
        material.p_diffract = 0.5;
    } 

    if(material.angle_spread_reflect < 0.0 || material.angle_spread_reflect > 360.0){
        fprintf(stderr, "Invalid angle, setting default \n");
        material.angle_spread_reflect = ERROR;
    } 

    json_object* absorbing = get_json_object(current, ABSORBING_TAG);
    if(absorbing != NULL && absorbing->type == JSON_BOOLEAN){
        material.is_absorbing = absorbing->value.boolean;
    }

    return material;
}


static camera get_camera(json_object* current){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse camera \n");
        exit(EXIT_FAILURE);
    }
    camera camara = {
        .fov = 90,
        .position = origin_vector,
        .up = origin_vector
    };

    camara.fov = get_double(get_json_object(current, CAMERA_FOV));
    camara.position = get_vector_or_hemisphere(get_json_object(current, CAMERA_POSITION), false);  
    camara.up = get_vector_or_hemisphere(get_json_object(current, CAMERA_UP), true);  


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
        if(tag == NULL || tag->type != JSON_STRING){
            fprintf(stderr, "Invalid tag object \n");
            exit(EXIT_FAILURE);
        }

        if(strncmp(tag->value.string, SPHERE, MAX_STRING_SIZE) == 0){
           json_object* repeat =  get_json_object(object_raytraceable, RAYTRACEABLE_REPEAT);
           if(repeat != NULL){
                size_t repetitions = get_integer(repeat);
                for(size_t j=0; j<repetitions; j++){
                    object* sphere_object = get_sphere(object_raytraceable, garbage);
                    sphere_object->id = (i+1);
                    objects = add_node(objects, sphere_object);
                }

           } else {
                object* sphere_object = get_sphere(object_raytraceable, garbage);
                sphere_object->id = (i+1);
                objects = add_node(objects, sphere_object);
           }

        } else if(strncmp(tag->value.string, PLANE_TAG, MAX_STRING_SIZE) == 0){
            object* plane_object = get_plane(object_raytraceable, garbage);
            plane_object->id = (i+1);
            objects = add_node(objects, plane_object);
        } else if(strncmp(tag->value.string, OBJ, MAX_STRING_SIZE) == 0){
            mesh_container container = get_obj(object_raytraceable, garbage);
            for(size_t j=0; j<container.length; j++){
                object* triangle = malloc(sizeof(object));
                *triangle = container.triangles[j];
                triangle->id = (i+1);
                objects = add_node(objects, triangle);
                array_push(garbage, triangle);
            }
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

    sphere_geometry->center = get_vector_or_hemisphere(get_json_object(current, CENTER), false);

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

static object* get_plane(json_object* current, object_array* garbage){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse objects \n");
        exit(EXIT_FAILURE);
    }
    plane* plane_geometry = malloc(sizeof(plane));
    array_push(garbage, plane_geometry);
    plane_geometry->radio = get_double(get_json_object(current, RADIUS));
    if(plane_geometry->radio < 0.0){
        fprintf(stdout, "Ilgeal radius setting up default \n");
        plane_geometry->radio= 2.0;
    }

    plane_geometry->center = get_vector_or_hemisphere(get_json_object(current, CENTER), false);
    plane_geometry->surface_normal = get_vector_or_hemisphere(get_json_object(current, NORMAL_TAG), true);

    properties plane_material = get_material(get_json_object(current, MATERIAL));

    object* plane_object = malloc(sizeof(object));
    array_push(garbage, plane_object);
    plane_object->geometry = plane_geometry; 
    plane_object->material = plane_material; 
    plane_object->get_geometry_collition = (geometry_collition (*) (void*, ray)) get_plane_collition; 
    plane_object->transform_geometry = (void (*) (matrix, void*))  transform_plane_with_mutation; 
    plane_object->get_bounding_box = (box (*) (void*)) get_plane_bounding_box; 

    plane_object->bounding_box  = get_plane_bounding_box(plane_geometry); 
    plane_object->surface_area = get_plane_area(plane_geometry); 
    return plane_object;
}

static mesh_container get_obj(json_object* current, object_array* garbage){
    if(current == NULL || current->type != JSON_OBJECT){
        fprintf(stderr, "Cannot parse objects \n");
        exit(EXIT_FAILURE);
    }


    json_object* path = get_json_object(current,FILE_PATH);
    double scale = get_double(get_json_object(current, SCALE));
    properties polygon_material = get_material(get_json_object(current, MATERIAL));

    json_object* transformations = get_json_object(current, TRANSFORMATIONS_TAG);
    matrix transformation = identity_matrix;
    if(transformations != NULL && transformations->type == JSON_ARRAY){
        for(size_t i=0; i<transformations->length; i++){
            matrix element = get_transformaiton_matrix(get_json_element(transformations, i));
            transformation = mul_matrix(element, transformation);
        }
        
    }
    mesh_container container = {
        .length = 0,
        .triangles = NULL
    };
    if(path->type == JSON_STRING){
        file_type_surface file_type = get_file_type(path->value.string);
        switch(file_type) {
            case FILE_TYPE_SURFACE_OBJ: {
                obj_container obj_file = read_obj_file(path->value.string, scale, polygon_material, transformation, garbage);
                container.length = obj_file.length;
                container.triangles = obj_file.triangles;
                break;
            }
            case FILE_TYPE_SURFACE_STL: {
                stl_container stl_file = read_stl_file(path->value.string, scale, polygon_material, transformation);
                container.length = stl_file.length;
                container.triangles = stl_file.triangles;
                break;
            }
            default:
                fprintf(stderr, "Cannot parse path \n");
                exit(EXIT_FAILURE);
        }
        
    } else {
        fprintf(stderr, "Cannot parse path \n");
        exit(EXIT_FAILURE);
    }
    return container;
}


static file_type_surface get_file_type(char* file_name){
    size_t point = 0;
    size_t length = strnlen(file_name, MAX_STRING_SIZE);
    for(size_t i=0; i<length; i++){
        if(file_name[i] == '.') {
            point = i;
        }
    }
    
    if(length == MAX_STRING_SIZE){
        return FILE_TYPE_SURFACE_UNKNOWN;
    }

    if(strncmp(&file_name[point+1], "obj", 3) == 0) {
        return FILE_TYPE_SURFACE_OBJ;
    }

    if(strncmp(&file_name[point+1], "stl", 3) == 0) {
        return FILE_TYPE_SURFACE_STL;
    }
    return FILE_TYPE_SURFACE_UNKNOWN;
}
