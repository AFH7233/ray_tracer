#include "scene_reader.h"

static scene create_scene(list* head);

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
        size_t line_size = strlen(line);
        size_t index = 0;

        while(line[index] != '\0' && index <= line_size){
        //fprintf(stderr,"size: %I64d \t line: %s",line_size, line);
        switch (state)
            {
            case JSON_SEARCH:{
                if((line[index]) == OBJECT_OPEN){
                    parse_tree* current = malloc(sizeof(parse_tree));
                    current->type = OBJECT_BEGIN;
                    current->value = NULL;
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
            case TAG_SEARCH:{

                if((line[index]) == TAG_DELIMITER ){
                    char* tag = calloc(20,sizeof(char));
                    tag[0] = '\0';
                    char open[2];
                    char close[2];
                    int read = sscanf(&line[index], "%[\"]%[a-z]%[\"]", open, tag, close);
                    if(read < 3){
                        fprintf(stderr, "TAG SEARCH: I can't parse it\n");
                        exit(EXIT_FAILURE);
                    } else {
                        size_t len = strlen(tag);
                        index += (len+2);
                        if(index > line_size){
                            index = line_size;
                        }
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = TAG;
                        current->value = tag;
                        stack = push_node(stack, current);
                        state = VALUE_SEARCH;
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
            case VALUE_SEARCH:{
                if((line[index]) == DEFINITION_START ){
                    char* value = calloc(20,sizeof(char));
                    value[0] = '\0';
                    char open[2];
                    char close[2];

                    int read = sscanf(&line[index], "%*[:]%*[ ]%[\"]%[0-9a-zA-Z\\:./ ]%[\"]", open, value, close);
                    read += sscanf(&line[index], "%*[:]%[\"]%[0-9a-zA-Z\\:./ ]%[\"]", open, value, close);
                    read += sscanf(&line[index], "%*[:]%[0-9.]", value);
                    read += sscanf(&line[index], "%*[:]%*[ ]%[0-9.]", value);
                    if(read < 1 || (1 < read && read < 3)){
                        read = sscanf(&line[index], "%*[:]%[{]", open);
                        read += sscanf(&line[index], "%*[:]%*[ ]%[{]", open);
                        if(read < 1){
                            fprintf(stderr, "VALUE_SEARCH: I can't parse it\n");
                            exit(EXIT_FAILURE);
                        } else {
                            char spaces[100];
                            spaces[0] = '\0';
                            sscanf(line, "%*[:]%[ ]", spaces);
                            index += strlen(spaces);
                            state = JSON_SEARCH;
                        }
                    } else {
                     
                        parse_tree* current = malloc(sizeof(parse_tree));
                        current->type = VALUE;
                        current->value = value;
                        stack = push_node(stack, current);
                        // Dirty hack to advance line
                        char spaces[100];
                        spaces[0] = '\0';
                        sscanf(&line[index], "%*[:]%[ ]", spaces);
                        index += strlen(spaces) + 1;
                        if(index > line_size){
                            index = line_size;
                        }

                        index += sscanf(&line[index], "%[\"]", open);
                        if(index > line_size){
                            index = line_size;
                        }

                        spaces[0] = '\0';
                        sscanf(&line[index], "%[0-9a-zA-Z\\:./ ]", spaces);
                        index += strlen(spaces);
                        if(index > line_size){
                            index = line_size;
                        }

                        index += sscanf(&line[index], "%[\"]", open) - 1;
                        if(index > line_size){
                            index = line_size;
                        }

                        state = SEPARATOR_SEARCH;
                    }
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
                    current->value = NULL;
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
    
    scene result = create_scene( head);

    list* current = head;
    //Clean strings
    while(current != NULL){
        parse_tree* node = (parse_tree*) current->value;
        if(node->type == TAG || node->type == VALUE){
            free(node->value);
            node->value = NULL;
        }
        current = current->next;
    }
    free_list(head);
    free(line);
    fclose(file);
    return result;
}

static scene create_scene(list* head) {
    scene result = {};
    return result;
}
