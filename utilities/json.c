#include "json.h"


static bool isnumber(char* line);
static bool isfraction(char* value);
static bool ispositive(char* line);
static bool iswhole(char* value);
static bool isexponent(char* value);
static bool issigned(char* value);
static size_t hash(char* str);
static bool put(json_object** map, char* key, json_object* value);
static json_object* get(json_object** map, char* key);
static json_object* to_json_object(list* head);
static json_object* to_json_array(list* head);
static bool set_json_value(json_object* object, list* current);

json_object* read_json(char* const file_name){
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
    //Dirty hack cus i forgot to consider arrays
    char inside_array = 'a';
    char inside_object = 'o';
    object_array inside_stack = new_array();
    while(fgets(line, MAX_LINE, file) != NULL){
        size_t line_size = strnlen(line, MAX_LINE);
        size_t index = 0;

        while(line[index] != '\0' && index < line_size){
        //fprintf(stderr,"size: %I64d \t line: %s",line_size, line);
            switch (state)
                {
                case JSON_SEARCH:{
                    if((line[index]) == OBJECT_OPEN){
                        array_push(&inside_stack, &inside_object);
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
                        array_push(&inside_stack, &inside_array);
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
                        int read = sscanf(&line[index], "%[\"]%[^\"\n\r]%[\"]", open, tag, close);
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
                        int read = sscanf(&line[index], "%[\"]%[^\"\n\r]%[\"]", open, value, close);

                        if(read < 3 || strnlen(open, MAX_STRING_SIZE) != 1 || strnlen(close, MAX_STRING_SIZE) != 1 ){
                            free(value);
                            printf("%s\n", value);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                        
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE_STRING;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) + 2 - 1;
                            state = SEPARATOR_SEARCH;
                        }
                    } else if(isnumber(&line[index])){
                        char* value = calloc(MAX_STRING_SIZE,sizeof(char));
                        value[0] = '\0';
                        int read = sscanf(&line[index], "%[0-9.eE+-]", value);
                        if(read < 1){
                            free(value);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE_NUMBER;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) - 1;
                            state = SEPARATOR_SEARCH;
                        }
                    } else if ((line[index])== 't' || (line[index])== 'f' ){
                        char* value = calloc(MAX_STRING_SIZE,sizeof(char));
                        sscanf(&line[index], "%[truefals]", value);
                        bool is_valid = strncmp(value, "true", MAX_STRING_SIZE) == 0 ||
                        strncmp(value, "false", MAX_STRING_SIZE) == 0;

                        if(is_valid){
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE_BOOLEAN;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) - 1;
                            state = SEPARATOR_SEARCH;
                        } else {
                            free(value);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    } else if(line[index]== 'n'){
                        char* value = calloc(MAX_STRING_SIZE,sizeof(char));
                        sscanf(&line[index], "%[nul]", value);
                        bool is_valid = strncmp(value, "null", MAX_STRING_SIZE) == 0;

                        if(is_valid){
                            parse_tree* current = malloc(sizeof(parse_tree));
                            current->type = VALUE_NULL;
                            current->value = value;
                            stack = push_node(stack, current);
                            index += strnlen(value, MAX_STRING_SIZE) - 1;
                            state = SEPARATOR_SEARCH;
                        } else {
                            free(value);
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        }
                    } 
                    else if((line[index]) == OBJECT_OPEN){
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
                        
                        char* inside_what = array_peek(&inside_stack);

                        if(inside_what == NULL){
                            fprintf(stderr, "Not balanced braces\n");
                            exit(EXIT_FAILURE);
                        }

                        if(*inside_what == 'a'){
                            state = VALUE_SEARCH;
                        } else {
                            state = TAG_SEARCH;
                        }

                    } else if(line[index] == OBJECT_CLOSE ) {
                        char* inside_what = array_pop(&inside_stack);

                        if(inside_what == NULL || *inside_what == inside_array){
                            fprintf(stderr, "Not balanced braces\n");
                            exit(EXIT_FAILURE);
                        }

                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = OBJECT_END;
                        current->value = calloc(2,sizeof(char));
                        current->value[0] = '}';
                        current->value[1] = '\0';
                        stack = push_node(stack, current);
                        state = SEPARATOR_SEARCH;                   
                    } 
                else if(line[index] == ARRAY_CLOSE ) {
                        char* inside_what = array_pop(&inside_stack);

                        if(inside_what == NULL || *inside_what == inside_object){
                            fprintf(stderr, "Not balanced braces\n");
                            exit(EXIT_FAILURE);
                        }

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

    json_object* root = to_json_object(head);
    list* current = head;
    //Clean strings
    while(current != NULL){
        if(current->value != NULL){
            parse_tree* node = (parse_tree*) current->value;
            if((node->type == TAG ||  node->type == VALUE_STRING ) && node->value != NULL){
                free(node->value);
                node->value = NULL;
            }
        }
        current = current->next;
    }
    free_list(head);
    free(line);
    fclose(file);
    free(inside_stack.elements);
    fprintf(stdout, "Finished creating json\n");
    return root;
}

json_object* get_json_object(json_object root[1], char* key){
    if(root->type == JSON_OBJECT){
       json_object* result = get(root->map, key);
       if(result == NULL){
           return NULL;
       }
       return result;
    } else {
        fprintf(stderr, "This element is not the desired type\n");
        return NULL;
    }
}
json_object* get_json_element(json_object root[1], size_t index){
    if(root->type == JSON_ARRAY){
        if(index < root->length){
            return root->map[index];
        } else {
           return NULL;
        }
    } else {
        fprintf(stderr, "This element is not the desired type\n");
        return NULL;
    } 
}



void free_json(json_object root[1]){
    
    if(root == NULL){
        return;
    }

    if(root->type == JSON_OBJECT || root->type == JSON_ARRAY || root->type == JSON_COLLISION){
        for(size_t i=0; i<root->length; i++){
            free_json(root->map[i]);
            root->map[i]=NULL;
        }
        if(root->tag != NULL){
            free(root->tag);
        }
        free(root);
    } else if(root->type == JSON_BOOLEAN || root->type == JSON_NUMBER || root->type == JSON_NULL  ){
        if(root->tag != NULL){
            free(root->tag);
        }
        free(root);
    } else if(root->type == JSON_STRING) {
        free(root->value.string);
        if(root->tag != NULL){
            free(root->tag);
        }
        free(root);
    }
}


static bool isnumber(char* line){

    if( isdigit(line[0]) || line[0]=='-') {
        char value[MAX_STRING_SIZE];
        value[0] = '\0';
        char zeros[MAX_STRING_SIZE];
        zeros[0] = '\0';
        bool is_negative = line[0] =='-';
        char* start = is_negative? &line[1] : &line[0];
        sscanf(start, "%[0]%[0-9.eE+-]", zeros, value);
        if(zeros[0]  == '\0'){
            value[0] = '\0';
            sscanf(start, "%[0-9.eE+-]", value);
            if(value[0] != '\0'){
                return ispositive(value); //This covers ddddd.44334
            }
        } else if(strnlen(zeros, MAX_STRING_SIZE) == 1){
            if(value[0] != '\0'){
                return isfraction(value); //This covers 0.2916727
            } else {
                return !is_negative; //This covers 0
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
    } else if(strnlen(point, MAX_STRING_SIZE) == 0){
        return iswhole(left);                
    } else {
        return false;
    }
    return false;
}

static bool isfraction(char* value){
    char right[MAX_STRING_SIZE];
    right[0] = '\0';
    char point[MAX_STRING_SIZE];
    point[0] = '\0';
    sscanf(value, "%[.]%[0-9.eE+-]", point, right);
    if(strnlen(point, MAX_STRING_SIZE) == 1){
        return isexponent(right);            
    } else {
        return false;
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
    } else if(strnlen(exponent, MAX_STRING_SIZE) == 0) {
        return iswhole(left);                 
    } else {
        return false;
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
    } else if(strnlen(sign, MAX_STRING_SIZE) == 0){
        sscanf(value, "%[0-9.eE]", left);
        return iswhole(left);  
    } else {
        return false;
    }
    return false;
}


static size_t hash(char* str){
    size_t length = strnlen(str, MAX_STRING_SIZE);
    size_t mul = 1;
    size_t hash = 0;
    for(size_t i=length; i>0; i--){
        hash += (str[i-1])*mul;
        mul *= 32;
    }
    return hash % MAP_SIZE;
}

static bool put(json_object** map, char* key, json_object* value){
    size_t index = hash(key);
    if(map[index] != NULL){
        fprintf(stdout, "Warning: There is a collision\n");
        if(map[index]->type != JSON_COLLISION){
            json_object* collision = malloc(sizeof(json_object));
            collision->type = JSON_COLLISION;
            collision->cap = 10;
            collision->length = 2;
            collision->map = calloc(10, sizeof(json_object*));
            collision->tag = NULL;
            json_object* temp = map[index];
            collision->map[0] = temp;
            collision->map[1] = value;
            map[index] = collision;
        } else {
            for(size_t i=0; i<map[index]->length; i++){
                if(strncmp(map[index]->tag, key, MAX_STRING_SIZE) == 0){
                    return false;
                }
            }
            if(map[index]->length < map[index]->cap){
                map[index]->map[map[index]->length] = value;
                map[index]->length++;
            } else {
                json_object** arr = map[index]->map;
                map[index]->map = calloc(2*map[index]->cap, sizeof(json_object*));
                for(size_t i=0; i<map[index]->length; i++){
                    map[index]->map[i] = arr[i];
                }
                free(arr);
                map[index]->cap = 2*map[index]->cap;
                map[index]->map[map[index]->length] = value;
                map[index]->length++;               
            }
        }
    } else {
        map[index] = value;
    }

    return true;
}

static json_object* get(json_object** map, char* key){
    size_t index = hash(key);
    if(map[index] != NULL){
        if(map[index]->type == JSON_COLLISION){
            for(size_t i=0; i<map[index]->length; i++){
                if(strncmp(map[index]->tag, key, MAX_STRING_SIZE) == 0){
                    return map[index]->map[i];
                }
            }
        } else {
            return map[index];
        }
    } 

    return NULL;
}


static json_object* to_json_object(list* head){
    list* current = head;
    json_object* root = NULL;
    while(current != NULL){
        if(current->value != NULL && !current->is_visited){
            parse_tree* node = (parse_tree*) current->value;
            if(node->type == OBJECT_BEGIN && root == NULL){
                root = malloc(sizeof(json_object));
                root->type = JSON_OBJECT;
                root->tag = NULL;
                current->is_visited = true;
                root->cap = MAP_SIZE;
                root->length = MAP_SIZE;
                root->map = calloc(MAP_SIZE, sizeof(json_object*));
            } else if (node->type == TAG){
                json_object* object = malloc(sizeof(json_object));
                object->tag = calloc(strnlen(node->value, MAX_STRING_SIZE)+1, sizeof(char));
                strncpy(object->tag, node->value, strnlen(node->value, MAX_STRING_SIZE));                 
                current->is_visited = true;
                current = current->next;
                bool is_set = set_json_value(object, current);

                if(is_set){
                    put(root->map, object->tag, object);
                } else {
                    fprintf(stderr, "Bad value\n");
                    exit(EXIT_FAILURE);
                }
            } else if(node->type == ARRAY_END){
                fprintf(stderr, "Unbalanced object\n");
                exit(EXIT_FAILURE);
            }  else if(node->type == OBJECT_END){
                current->is_visited = true;
                break;
            }
        }
        if(current==NULL){
            break;
        }
        current = current->next;
    }
    return root;
}

static json_object* to_json_array(list* head){
    list* current = head;
    json_object* root = NULL;
    while(current != NULL){
        if(current->value != NULL && !current->is_visited){
            parse_tree* node = (parse_tree*) current->value;
            if(node->type == ARRAY_BEGIN && root == NULL){
                root = malloc(sizeof(json_object));
                root->type = JSON_ARRAY;
                current->is_visited = true;
                root->cap = 10;
                root->length = 0;
                root->tag = NULL;
                root->map = calloc(10, sizeof(json_object*));
            } else if (node->type == VALUE_BOOLEAN || 
             node->type == VALUE_NULL || 
             node->type == VALUE_STRING || 
             node->type == VALUE_NUMBER|| 
             node->type == ARRAY_BEGIN || 
             node->type == OBJECT_BEGIN){
                json_object* object = malloc(sizeof(json_object));
                object->tag = NULL;
                bool is_set = set_json_value(object, current);
                if(is_set){
                    if(root->length < root->cap){
                        root->map[root->length] = object;
                        root->length++;  
                    } else {
                        json_object** arr = root->map;
                        root->map = calloc(2*root->cap, sizeof(json_object*));
                        
                        for(size_t i=0; i<root->length; i++){
                            root->map[i] = arr[i];
                        }
                        free(arr);
                        root->cap = 2*root->cap;
                        root->map[root->length] = object;
                        root->length++;  
                    }
                } else {
                    fprintf(stderr, "Bad value\n");
                    exit(EXIT_FAILURE);
                }
            } else if(node->type == OBJECT_END){
                fprintf(stderr, "Unbalanced array\n");
                exit(EXIT_FAILURE);
            } else if(node->type == ARRAY_END){
                current->is_visited = true;
                break;
            }
        }
        if(current==NULL){
            break;
        }
        current = current->next;
    }
    return root;
}

static bool set_json_value(json_object* object, list* current){
    if(current != NULL && current->value != NULL){
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == VALUE_STRING){
            current->is_visited = true;
            object->type = JSON_STRING;
            object->value.string = calloc(strnlen(node->value, MAX_STRING_SIZE)+1, sizeof(char));
            strncpy(object->value.string, node->value, strnlen(node->value, MAX_STRING_SIZE));
            return true;
        } else if(node->type == VALUE_NUMBER){
            current->is_visited = true;
            object->type = JSON_NUMBER;
            object->value.number = strtod(node->value, NULL);
            return true;
        } else if(node->type == VALUE_BOOLEAN){
            current->is_visited = true;
            object->type = JSON_BOOLEAN;
            object->value.boolean = strncmp(node->value, "true", MAX_STRING_SIZE) == 0;
            return true;
        } else if(node->type == VALUE_NULL){
            current->is_visited = true;
            object->type = JSON_NULL;
            object->value.string = NULL;
            return true;
        } else if(node->type == OBJECT_BEGIN) {
            json_object* root = to_json_object(current);
            object->type = root->type;
            object->length = root->length;
            object->cap = root->cap;
            object->map = root->map;
            free(root);
            return true;
        } else if(node->type == ARRAY_BEGIN) {
            json_object* root = to_json_array(current);
            object->type = root->type;
            object->length = root->length;
            object->cap = root->cap;
            object->map = root->map;
            free(root);
            return true;
        }
    }
    return false;
}   
