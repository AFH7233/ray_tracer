#ifndef _BVH_
    #define _BVH_

    /*
        Bounding volume hirearchy class, is a tree that make faster to find a collision between
        an object and a ray
    */
    
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <tgmath.h>
    #include <float.h>
    #include "geometric/algebra.h"
    #include "geometric/box.h"
    #include "object.h"
    #include "dynamic_array.h"

    //I don't like it allocates memory, but Imma fix that some day

    typedef enum axis axis;
    enum axis {X, Y, Z};

    typedef struct bvh_tree bvh_tree;
    struct bvh_tree
    {
        bool is_leaf;
        size_t num_of_objects;
        size_t cap_of_objects;
        box bounding_box;
        object** object_array; // I think object is too heavy so I have an array of pointers
        bvh_tree* left;
        bvh_tree* right;
        axis eje;
    };

    bvh_tree* new_bvh_tree();
    void add_object(bvh_tree* root, object* object_3d);
    void free_node_object_list(bvh_tree* root);
    void free_bvh_tree(bvh_tree* root);
    void distribute_bvh(bvh_tree* root);
    collition get_bvh_collition(bvh_tree *restrict root, ray pixel_ray);
    
#endif