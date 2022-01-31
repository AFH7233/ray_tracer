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

void print_bvh(bvh_tree* tree, size_t ident){

    if(tree == NULL){
        return;
    }

    for(size_t i=0; i<ident; i++){
        fprintf(stderr, "   "); 
    }
    fprintf(stderr, "<Tree> \n"); 
    if(tree->is_leaf) {
        for(size_t i=0; i<ident; i++){
            fprintf(stderr, "   "); 
        }
        fprintf(stderr, "min corner: %f %f %f \n", tree->bounding_box.min_x, tree->bounding_box.min_y, tree->bounding_box.min_z);   

        for(size_t i=0; i<ident; i++){
            fprintf(stderr, "   "); 
        }
        fprintf(stderr, "max corner: %f %f %f \n", tree->bounding_box.max_x, tree->bounding_box.max_y, tree->bounding_box.max_z); 

        for(size_t i=0; i<ident; i++){
            fprintf(stderr, "   "); 
        }
        fprintf(stderr, "num: %I64u \n", tree->num_of_objects); 
    } else {
        for(size_t i=0; i<ident; i++){
            fprintf(stderr, "   "); 
        }
        fprintf(stderr, "min corner: %f %f %f \n", tree->bounding_box.min_x, tree->bounding_box.min_y, tree->bounding_box.min_z);   

        for(size_t i=0; i<ident; i++){
            fprintf(stderr, "   "); 
        }
        fprintf(stderr, "max corner: %f %f %f \n", tree->bounding_box.max_x, tree->bounding_box.max_y, tree->bounding_box.max_z); 

        print_bvh(tree->left, ident + 1);
        print_bvh(tree->right, ident + 1);
    }
    for(size_t i=0; i<ident; i++){
        fprintf(stderr, "   "); 
    }
    fprintf(stderr, "</Tree> \n"); 
}