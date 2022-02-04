#include "scene_reader.h"

static scene to_scene(list* head);
static size_t get_integer(list* current);
static double get_double(list* current);
static vector get_vector(list* current, bool is_normal);
static color_RGB get_color(list* current);
static camera get_camera(list* current);
static properties get_material(list* current);
static bool isnumber(char* line, size_t index);
static bool ispositive(char* line);
static bool iswhole(char* value);
static bool isexponent(char* value);
static bool issigned(char* value);
static object* get_sphere(list* current);


scene read_scene(char* const file_name){
    FILE *file = fopen(file_name, "r");
    if(file == NULL){
        fprintf(stderr, "No pude leer el archivo\n");
        exit(EXIT_FAILURE); 
    }
    const size_t MAX_LINE = 1000;
    char* line = calloc(MAX_LINE, sizeof(char));
    parse_state state = JSON_SEARCH;
    list* stack = new_list();
    list* head = stack;

    while(fgets(line, MAX_LINE, file) != NULL){
        size_t line_size = strnlen(line, MAX_LINE);
        size_t index = 0;

        while(line[index] != '\0' && index < line_size){
        fprintf(stderr,"size: %I64d \t line: %s",line_size, line);
            switch (state)
                {
                case JSON_SEARCH:{
                    if((line[index]) == OBJECT_OPEN){
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = OBJECT_BEGIN;
                        current->value = calloc(2,sizeof(char));
                        current->value[0] = '{';
                        current->value[1] = '\0';
                        stack = push_node(stack, current);
                        state = TAG_SEARCH;
                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "JSON_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
                case ARRAY_SEARCH:{
                    if((line[index]) == ARRAY_OPEN){
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = ARRAY_BEGIN;
                        current->value = calloc(2,sizeof(char));
                        current->value[0] = '[';
                        current->value[1] = '\0';
                        stack = push_node(stack, current);
                        state = VALUE_SEARCH;
                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "ARRAY_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
                case TAG_SEARCH:{

                    if((line[index]) == TAG_DELIMITER ){
                        char* tag = calloc(MAX_STRING_SIZE,sizeof(char));
                        tag[0] = '\0';
                        char open[2];
                        char close[2];
                        int read = sscanf(&line[index], "%[\"]%[a-zA-Z ]%[\"]", open, tag, close);
                        if(read < 3){
                            fprintf(stderr, "TAG SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                            size_t len = strnlen(tag, MAX_STRING_SIZE);
                            index += (len+2) - 1;
                            if(index > line_size){
                                index = line_size;
                            }
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = TAG;
                            current->value = tag;
                            stack = push_node(stack, current);
                            state = VALUE_SEPARATOR_SEARCH;
                        }

                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "TAG_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
                case VALUE_SEPARATOR_SEARCH: {
                    if(line[index] == DEFINITION_START ){ 
                        state = VALUE_SEARCH;
                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "VALUE_SEPARATOR_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }      
                    break;
                }
                case VALUE_SEARCH:{
                    if(line[index] == TAG_DELIMITER)  {
                        char* value = calloc(MAX_STRING_SIZE,sizeof(char));
                        value[0] = '\0';
                        char open[MAX_STRING_SIZE];
                        open[0] = '\0';
                        char close[MAX_STRING_SIZE];
                        close[0] = '\0';

                        //int read = sscanf(&line[index], "%%[\"]%[0-9a-zA-Z\\:./ ]%[\"]", open, value, close);
                        int read = sscanf(&line[index], "%[\"]%[0-9a-zA-Z:./~ ]%[\"]", open, value, close);

                        if(read < 3 || strnlen(open, MAX_STRING_SIZE) != 1 || strnlen(close, MAX_STRING_SIZE) != 1 ){
                            printf("%s\n", value);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                        
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) + 2 - 1;
                            state = SEPARATOR_SEARCH;
                        }
                    } else if(isnumber(line,index)){
                        char* value = calloc(30,sizeof(char));
                        value[0] = '\0';
                        int read = sscanf(&line[index], "%[0-9.eE+-]", value);
                        if(read < 1){
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) - 1;
                            state = SEPARATOR_SEARCH;
                        }
                    } else if((line[index]) == OBJECT_OPEN){
                        index--;
                        state = JSON_SEARCH;
                    }  else if((line[index]) == ARRAY_OPEN){
                        index--;
                        state = ARRAY_SEARCH;
                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
                case SEPARATOR_SEARCH:{
                    if(line[index] == SEPARATOR ){ 
                        state = TAG_SEARCH;
                    } else if(line[index] == OBJECT_CLOSE ) {
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = OBJECT_END;
                        current->value = calloc(2,sizeof(char));
                        current->value[0] = '}';
                        current->value[1] = '\0';
                        stack = push_node(stack, current);
                        state = SEPARATOR_SEARCH;                   
                    } 
                else if(line[index] == ARRAY_CLOSE ) {
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = ARRAY_END;
                        current->value = calloc(2,sizeof(char));
                        current->value[0] = ']';
                        current->value[1] = '\0';
                        stack = push_node(stack, current);
                        state = SEPARATOR_SEARCH;                   
                    } else {
                        if(!isspace(line[index] )){
                            fprintf(stderr, "%d\n", line[index]);
                            fprintf(stderr, "SEPARATOR_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    }              
                    break;
                }
                default:
                    exit(EXIT_FAILURE);
                    break;
                }
                index++;
            }

    }
    fprintf(stdout, "Finished creating parsing tree\n");

    list* c = head;
    //Clean strings
    while(c != NULL){
        if(c->value != NULL){
            parse_tree* node = (parse_tree*) c->value;
            fprintf(stderr, "%s\n", node->value);
        }
        c = c->next;
    }
    
    scene result = {};//to_scene(head);



    list* current = head;
    //Clean strings
    while(current != NULL){
        if(current->value != NULL){
            parse_tree* node = (parse_tree*) current->value;
            if((node->type == TAG || node->type == VALUE) && node->value != NULL){
                free(node->value);
                node->value = NULL;
            }
        }
        current = current->next;
    }
    free_list(head);
    free(line);
    fclose(file);
    fprintf(stdout, "Finished creating scene\n");
    return result;
}


static bool isnumber(char* line, size_t index){

    if( isdigit(line[index]) || line[index]=='-') {
        char value[MAX_STRING_SIZE];
        value[0] = '\0';
        char minus[MAX_STRING_SIZE];
        minus[0] = '\0';
        char digit[MAX_STRING_SIZE];
        digit[0] = '\0';
        sscanf(&line[index], "%[-]%[1-9]%[0-9.eE+-]", minus, digit, value);
        if( strnlen(minus, MAX_STRING_SIZE) == 1 && strnlen(digit, MAX_STRING_SIZE) == 1){
            if(value[0] !='\0'){
                return ispositive(value);
            } else {
                return true;
            }
        } else {
            value[0] = '\0';
            sscanf(&line[index], "%[0-9.eE+-]", value);
            if(value[0] != '\0'){
                return ispositive(value);
            }
        }


    }
    return false;
}

static bool ispositive(char* value){
    char right[MAX_STRING_SIZE];
    right[0] = '\0';
    char left[MAX_STRING_SIZE];
    left[0] = '\0';
    char point[MAX_STRING_SIZE];
    point[0] = '\0';
    sscanf(value, "%[0-9eE+-]%[.]%[0-9.eE+-]", left, point, right);
    if(strnlen(point, MAX_STRING_SIZE) == 1){
        return iswhole(left) && isexponent(right);
    } else {
        return iswhole(left);                
    }
    return false;
}

static bool iswhole(char* value){
    char right[MAX_STRING_SIZE];
    right[0] = '\0';
    char left[MAX_STRING_SIZE];
    left[0] = '\0';
    sscanf(value, "%[0-9]%[0-9.eE+-]", left, right);
    return strnlen(left, MAX_STRING_SIZE) > 0 && (right[0] == '\0');
}

static bool isexponent(char* value){
    char right[MAX_STRING_SIZE];
    right[0] = '\0';
    char left[MAX_STRING_SIZE];
    left[0] = '\0';
    char exponent[MAX_STRING_SIZE];
    exponent[0] = '\0';
    sscanf(value, "%[0-9.+-]%[eE]%[0-9.eE+-]", left, exponent, right);
    if(strnlen(exponent, MAX_STRING_SIZE) == 1){
        return iswhole(left) && issigned(right);
    } else {
        return iswhole(left);                
    }
    return false;
}

static bool issigned(char* value){
    char right[MAX_STRING_SIZE];
    right[0] = '\0';
    char left[MAX_STRING_SIZE];
    left[0] = '\0';
    char sign[MAX_STRING_SIZE];
    sign[0] = '\0';
    sscanf(value, "%[+-]%[0-9.eE+-]", sign, right);
    if(strnlen(sign, MAX_STRING_SIZE) == 1){
        return iswhole(right);
    } else {
        sscanf(value, "%[0-9.eE]", left);
        return iswhole(left);                
    }
    return false;
}

static scene to_scene(list* head){
    scene empty = {
        .rays_per_pixel = 10,
        .bounces = 1,
        .focus = new_vector(0.0, 0.0, 100.0),
        .width = 640,
        .height = 480,
        .threads = 10,
    };
    list* current = head;
    empty.objects = new_list();
    size_t inner = 0;
    while(current != NULL){
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG){
                    
            if(strncmp(node->value, WIDTH, strlen(WIDTH)) == 0 ){
                current = current->next;
                empty.width = get_integer(current);
            } else if( strncmp(node->value, HEIGHT, strlen(HEIGHT)) == 0 ){
                current = current->next;
                empty.height = get_integer(current);            
            } else if( strncmp(node->value, NUMBER_OF_BOUNCES, strlen(NUMBER_OF_BOUNCES)) == 0 ){
                current = current->next;
                empty.bounces = get_integer(current);           
            } else if( strncmp(node->value, NUMBER_OF_THREADS, strlen(NUMBER_OF_THREADS)) == 0 ){
                current = current->next;
                empty.threads = get_integer(current);           
            } else if( strncmp(node->value, RAYS_PER_PIXEL, strlen(RAYS_PER_PIXEL)) == 0 ){
                current = current->next;
                empty.rays_per_pixel = get_integer(current);           
            } else if( strncmp(node->value, CAMERA, strlen(CAMERA)) == 0 ){
                inner++;
                current = current->next;
                empty.camara = get_camera(current);
            } else if( strncmp(node->value, FOCUS, strlen(FOCUS)) == 0 ){
                inner++;
                current = current->next;
                empty.focus = get_vector(current, false);
            } else if( strncmp(node->value, SPHERE, strlen(SPHERE)) == 0 ){
                inner++;
                current = current->next;
                empty.objects = push_node(empty.objects, get_sphere(current));
            } 
            
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }
        if(current == NULL){
            break;
        }
        current = current->next;
    }
    return empty;
}

static size_t get_integer(list* current){
    if(current == NULL){
        fprintf(stderr, "Cannot parse integer \n");
        exit(EXIT_FAILURE);
    }
    parse_tree* node = (parse_tree*) current->value;
    int real_number = atoi(node->value);
    if(real_number < 0){
        fprintf(stderr, "Invalid number for the ray tracer\n");
        exit(EXIT_FAILURE);        
    }
    return real_number;
}

static double get_double(list* current){
    if(current == NULL){
        fprintf(stderr, "Cannot parse width \n");
        exit(EXIT_FAILURE);
    }
    parse_tree* node = (parse_tree*) current->value;
    double real_number = atof(node->value);
    return real_number;
}

static vector get_vector(list* head, bool is_normal){
    vector point = {};

    if(is_normal) {
        point = new_normal(0.0,1.0,0.0);
    } else {
        point = new_vector(0.0, 0.0, 0.0);
    }

    list* current = head;
    size_t inner = 0;
    while(current != NULL){
        
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG){

            if(strncmp(node->value, VECTOR_X, strlen(VECTOR_X)) == 0 ){
                current = current->next;
                point.x = get_double(current);
            } else if( strncmp(node->value, VECTOR_Y, strlen(VECTOR_Y)) == 0){
                current = current->next;
                point.y = get_double(current);            
            } else if( strncmp(node->value, VECTOR_Z, strlen(VECTOR_Z)) == 0){
                current = current->next;
                point.z = get_double(current);               
            }
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }

        if(current == NULL){
            break;
        }

        current = current->next;
    }

    if(is_normal){
        return to_normal(point);
    } else {
        return point;
    }
}

static color_RGB get_color(list* head){
    color_RGB color = new_color_RGB(1.0, 1.0, 1.0);

    list* current = head;
    size_t inner = 0;
    while(current != NULL){
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG){
            if(strncmp(node->value, COLOR_R, strlen(COLOR_R)) == 0 ){
                current = current->next;
                int c = get_integer(current);
                c = c > 255? 255 : c;
                color.red = (c/255.0) + COLOR_ERROR;
            } else if( strncmp(node->value, COLOR_G, strlen(COLOR_G)) == 0){
                current = current->next;
                int c = get_integer(current);
                c = c > 255? 255 : c;
                color.green = (c/255.0) + COLOR_ERROR;  
            } else if( strncmp(node->value, COLOR_B, strlen(COLOR_B)) == 0){
                current = current->next;
                int c = get_integer(current);
                c = c > 255? 255 : c;
                color.blue = (c/255.0) + COLOR_ERROR;               
            }
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }
        if(current == NULL){
            break;
        }
        current = current->next;
    }
    return color;
}

static properties get_material(list* head){
    properties material = {
        .color = new_color_RGB(1.0,1.0,1.0),
        .emmitance = COLOR_ERROR,
        .p_diffract = 1.0,
        .angle_spread_reflect = 0.0
    };
    list* current = head;
    size_t inner=0;
    while(current != NULL) {
        parse_tree* node = (parse_tree*) current->value;
                                fprintf(stderr, "%s\n", node->value);
        if(node->type == TAG){
            if(strncmp(node->value, COLOR, strlen(COLOR)) == 0 ){
                inner++;
                current = current->next;
                material.color = get_color(current);
            } else if( strncmp(node->value, MATERIAL_EMMITANCE, strlen(MATERIAL_EMMITANCE)) == 0 ){
                current = current->next;
                material.emmitance = get_double(current) + COLOR_ERROR;
            } else if( strncmp(node->value, MATERIAL_P_DIFF, strlen(MATERIAL_P_DIFF)) == 0 ){
                current = current->next;
                material.p_diffract = get_double(current);
                if(material.p_diffract < 0.0 || material.p_diffract > 1.0){
                    fprintf(stderr, "Invalid probability, setting default \n");
                    material.p_diffract = 0.5;
                } 
            } else if( strncmp(node->value, MATERIAL_SPREAD, strlen(MATERIAL_SPREAD)) == 0 ){
                current = current->next;
                material.angle_spread_reflect = get_double(current);
                if(material.angle_spread_reflect < 0.0 || material.angle_spread_reflect > 360.0){
                    fprintf(stderr, "Invalid angle, setting default \n");
                    material.angle_spread_reflect = ERROR;
                } 
            } 
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }
        if(current == NULL){
            break;
        }
        current = current->next;
    }
    return material;
}


static camera get_camera(list* head){
    camera camara = {
        .fov = 45,
        .position = new_vector(0.0, 15.0, -60.0),
        .up = new_normal(0.0, 1.0, 0.0)
    };
    list* current = head;
    size_t inner = 0;
    while(current != NULL) {
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG){
            if(strncmp(node->value, CAMERA_FOV, strlen(CAMERA_FOV)) == 0 ){
                current = current->next;
                camara.fov = get_double(current);
            } else if( strncmp(node->value, CAMERA_POSITION, strlen(CAMERA_POSITION)) == 0 ){
                inner++;
                current = current->next;
                camara.position = get_vector(current, false);          
            } else if( strncmp(node->value, CAMERA_UP, strlen(CAMERA_UP)) == 0 ){
                inner++;
                current = current->next;
                camara.up = get_vector(current, true);          
            }
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }
        if(current == NULL){
            break;
        }
        current = current->next;
    }
    fprintf(stderr, "%s\n", "sali");
    return camara;
}

static object* get_sphere(list* head){
    sphere* sphere_geometry = malloc(sizeof(sphere));
    sphere_geometry->radio = 2.0;
    sphere_geometry->center = new_vector(0.0, 0.0,0.0);

    properties sphere_material = {
        .color = new_color_RGB(1.0,1.0,1.0),
        .emmitance = 10.0,
        .p_diffract = 1.0,
        .angle_spread_reflect = 0.0
    };

    object* sphere_object = malloc(sizeof(object));
    sphere_object->geometry = sphere_geometry; 
    sphere_object->material = sphere_material; 
    sphere_object->get_geometry_collition = (geometry_collition (*) (void*, ray)) get_sphere_collition; 
    sphere_object->transform_geometry = (void (*) (matrix, void*))  transform_sphere_with_mutation; 
    sphere_object->get_bounding_box = (box (*) (void*)) get_sphere_bounding_box; 

    list* current = head;
    size_t inner = 0;
    while(current != NULL) {
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG){
            if(strncmp(node->value, RADIUS, strlen(RADIUS)) == 0 ){
                current = current->next;
                sphere_geometry->radio = get_double(current);
                if(sphere_geometry->radio < ERROR){
                    fprintf(stderr, "Invalid negative radius, goingt to default 1.0 \n");
                    sphere_geometry->radio = 1.0;
                }
            } else if( strncmp(node->value, CENTER, strlen(CENTER)) == 0 ){
                inner++;
                current = current->next;
                sphere_geometry->center = get_vector(current, false);          
            } else if( strncmp(node->value, MATERIAL, strlen(MATERIAL)) == 0 ){
                inner++;
                current = current->next;
                sphere_object->material = get_material(current);        
            }
        } else if(node->type == OBJECT_END){
            inner--;
            if(inner == 0){
                break;
            } 
        } else if(node->type == OBJECT_BEGIN){
            inner++;
        }
        if(current == NULL){
            break;
        }
        current = current->next;
    }

    sphere_object->bounding_box  = get_sphere_bounding_box(sphere_geometry); 
    sphere_object->surface_area = get_sphere_area(sphere_geometry); 
    return sphere_object;
}




