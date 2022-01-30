#include "logging.h"


void print_vector(vector vector){
    fprintf(stderr, "%f , %f, %f, %f \n", vector.x, vector.y, vector.z, vector.w);
}

void print_matrix(matrix matriz){
    fprintf(stderr, "<Matrix> \n"); 
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j< 4; j++){
            fprintf(stderr, "%f \t", matriz.elements[i][j]); 
        }
        fprintf(stderr, "\n"); 
    }
    fprintf(stderr, "</Matrix> \n"); 
}