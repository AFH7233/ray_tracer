#ifndef _LOGGING_
    #define _LOGGING_

    #include <stdlib.h>
    #include <stdio.h>
    #include "algebra.h"
    #include "bvh.h"

    void print_vector(vector vector);
    void print_matrix(matrix matriz);
    void print_bvh(bvh_tree* tree, size_t ident);

#endif