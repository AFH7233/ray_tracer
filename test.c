#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities/json.h"

int main(){
    /*char garbage[100];
    garbage[0] = '\0';
    char* test = ": .34";
    char open[2];
    char close[2];
    //int h = sscanf(test, "%*[\"]%[a-z]%[\"]", garbage, t);
    int read_a = sscanf(test, "%*[:]%*[ ]%[\"]%[0-9a-zA-Z\\:./ ]%[\"]", open, garbage, close);
    read_a += sscanf(test, "%*[:]%[\"]%[0-9a-zA-Z\\:./ ]%[\"]", open, garbage, close);
    read_a += sscanf(test, "%*[:]%[0-9.]", garbage);
    read_a += sscanf(test, "%*[:]%*[ ]%[0-9.]", garbage);
    if( read_a < 1 || (1 < read_a && read_a < 3)){
        printf("Error\n");
    }
    
    fprintf(stderr, "%s\n",garbage);
    fprintf(stderr, "%s\n", test);*/
    json_object* test = read_json("assets/test.json");
    json_object* width = get_object(test, "width");
    fprintf(stdout, "%s\n", width->value);
    free_json(test);


    return EXIT_SUCCESS;
}