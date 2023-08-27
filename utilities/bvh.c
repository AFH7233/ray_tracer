#include "bvh.h"

static int compare_by_x(const void * a, const void * b);
static int compare_by_y(const void * a, const void * b);
static int compare_by_z(const void * a, const void * b);
static double is_collition_dected(box bounding_box, ray pixel_ray);
static collition get_bvh_collition_with_distance(bvh_tree *restrict root, ray pixel_ray, double distance);


bvh_tree* new_bvh_tree(void){
    bvh_tree* root = malloc(sizeof(bvh_tree));
    
    root->is_leaf = true;
    root->num_of_objects = 0; 
    root->cap_of_objects = 10;
    root->object_array = calloc(root->cap_of_objects, sizeof(object*));

    if(root->object_array == NULL){
        root->cap_of_objects = 0;
    }

    root->left = NULL;
    root->right = NULL;
    root->eje = X;
    root->bounding_box = (box) {
        .min_x = INFINITY,
        .min_y = INFINITY,
        .min_z = INFINITY,
        .max_x = -INFINITY,
        .max_y = -INFINITY,
        .max_z = -INFINITY

    };

    return root;
}


void add_object(bvh_tree* root, object* object_3d){
    size_t final_size = root->num_of_objects;
    if(final_size < root->cap_of_objects){
        root->object_array[final_size] = object_3d;
    } else {
        object** new_array = calloc(root->cap_of_objects*2, sizeof(object*));
        if(new_array == NULL){
            fprintf(stderr, "No memory to reallocate\n");
            exit(EXIT_FAILURE);
        }
        for(size_t i=0; i<root->num_of_objects; i++){
            new_array[i] = root->object_array[i];
        }
        free(root->object_array);
        root->object_array = new_array;
        root->object_array[final_size] = object_3d;
        root->cap_of_objects = root->cap_of_objects*2;
    }
    root->num_of_objects = final_size + 1;
    box bounding_box = {
        .min_x = fmin(object_3d->bounding_box.min_x, root->bounding_box.min_x),
        .min_y = fmin(object_3d->bounding_box.min_y, root->bounding_box.min_y),
        .min_z = fmin(object_3d->bounding_box.min_z, root->bounding_box.min_z),
        .max_x = fmax(object_3d->bounding_box.max_x, root->bounding_box.max_x),
        .max_y = fmax(object_3d->bounding_box.max_y, root->bounding_box.max_y),
        .max_z = fmax(object_3d->bounding_box.max_z, root->bounding_box.max_z),   
    };
    bounding_box.center = new_vector(
        bounding_box.min_x + (bounding_box.max_x - bounding_box.min_x)/2.0,
        bounding_box.min_y + (bounding_box.max_y - bounding_box.min_y)/2.0,
        bounding_box.min_z + (bounding_box.max_x - bounding_box.min_z)/2.0
    );
    root->bounding_box = bounding_box;
}

void distribute_bvh(bvh_tree* root){
    object_array stack = new_array_with_cap(1000);
    array_push(&stack, root); 
    axis ejes[] = {X, Y, Z};
    while (stack.length > 0)
    {
        bvh_tree* current = array_pop(&stack);
        if(current->num_of_objects <= 4){
            continue;
        }

        current->is_leaf = false;
        double min_area = INFINITY;
        size_t length = current->num_of_objects;
        double* areas = calloc(length, sizeof(double));
        axis winner = X;
        object** array = current->object_array;
        size_t cut = 0;
        for(size_t m = 0; m<3; m++){
            switch (ejes[m])
            {
            case X:
                qsort(array, current->num_of_objects, sizeof(object*), compare_by_x);

                break;
            case Y:
                qsort(array, current->num_of_objects, sizeof(object*), compare_by_y);
                break;
            case Z:
                qsort(array, current->num_of_objects, sizeof(object*), compare_by_z);
                break;
            default:
                break;
            }

            areas[0] = (array[0])->surface_area;
            for(size_t i=1; i < length; i++){
                areas[i] = (array[i])->surface_area;
                areas[i] += areas[i-1];
            }

            for(size_t i=0; i < length; i++){
                double left_area = areas[i];
                double right_area = areas[length-1] - areas[i];

                double thing = (i + 1.0)*(left_area) + (length-i-1.0)*(right_area);
                if(isless(thing, min_area)){
                    cut = i;
                    min_area = thing;
                    winner = ejes[m];
                }
            }
        }

        switch (winner)
        {
        case X:
            current->eje = X;
            qsort(array, current->num_of_objects, sizeof(object*), compare_by_x);
            break;
        case Y:
            current->eje = Y;
            qsort(array, current->num_of_objects, sizeof(object*), compare_by_y);
            break;
        case Z:
            current->eje = Z;
            qsort(array, current->num_of_objects, sizeof(object*), compare_by_z);
            break;
        default:
            break;
        }

        current->left = new_bvh_tree();
        for(size_t i=0; i < cut+1; i++){
            add_object(current->left, array[i]);
        }

        current->right = new_bvh_tree();
        for(size_t i=cut+1; i < length; i++){
            add_object(current->right, array[i]);
        }

        free_node_object_list(current);
        free(areas);

        array_push(&stack, current->left);
        array_push(&stack, current->right);
    }
    
    free(stack.elements);
    return;
}
  
collition get_bvh_collition(bvh_tree *restrict root, ray pixel_ray){
    double distance = INFINITY;
    collition result = {.is_hit = false, .distance = INFINITY};
    double bvh_distance  = is_collition_dected(root->bounding_box, pixel_ray);
    if(bvh_distance < distance){
        return get_bvh_collition_with_distance(root, pixel_ray, distance);
    } else {
        return result;
    }

}

static collition get_bvh_collition_with_distance(bvh_tree *restrict root, ray pixel_ray, double distance){
    collition result = {.is_hit = false, .distance = INFINITY};
    bvh_tree* current = root;

    if(current == NULL){
        return result;
    }

    if (current->is_leaf) {
        size_t length = current->num_of_objects;
        object** array = current->object_array;
        for(size_t i=0; i < length; i++){
            collition object_collition = get_collition(array[i], pixel_ray);
            if(object_collition.is_hit && isless(object_collition.distance, distance)){
                result = object_collition;
            }
        }
        return result;
    } else {
        double distance_right = INFINITY;
        bool search_right = false;
        if(current->right != NULL){
            distance_right = is_collition_dected(current->right->bounding_box, pixel_ray);
            search_right = isless(distance_right, distance); 
        }

        double distance_left = INFINITY;
        bool search_left = false;
        if(current->left != NULL){
            distance_left = is_collition_dected(current->left->bounding_box, pixel_ray);
            search_left = isless(distance_left, distance); 
        }

        collition right = result;
        collition left = result;
        if(search_right && search_left){
            if(isless(distance_right, distance_left)){
                right = get_bvh_collition_with_distance(current->right, pixel_ray, distance);
                double next_distance = isless(right.distance, distance) ? right.distance : distance;
                left = get_bvh_collition_with_distance(current->left, pixel_ray, next_distance);
            } else {
                left = get_bvh_collition_with_distance(current->left, pixel_ray, distance);
                double next_distance = isless(left.distance, distance) ? left.distance : distance;
                right = get_bvh_collition_with_distance(current->right, pixel_ray, next_distance);         
            }

            if(isless(right.distance, left.distance)){
                return right;
            } else {
                return left;
            }
        } else if( search_right ){
            return get_bvh_collition_with_distance(current->right, pixel_ray, distance);
        } else if( search_left ){
            return get_bvh_collition_with_distance(current->left, pixel_ray, distance);
        }
    } 

    return result;
}


void free_node_object_list(bvh_tree* root){
    if(root->object_array != NULL){
        free(root->object_array);
        root->num_of_objects = 0;
        root->cap_of_objects = 0;
        root->object_array = NULL;
    }

}

void free_bvh_tree(bvh_tree* root){
    if(root == NULL){
        return;
    } else if(root->is_leaf) {
        free_node_object_list(root);
        free(root);
        return;
    } else {
        free_node_object_list(root);
        free_bvh_tree(root->left);
        free_bvh_tree(root->right);
        root->left = NULL;
        root->right = NULL;
        free(root);
    }
}

//It was copied from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
static double is_collition_dected(box bounding_box , ray pixel_ray){
    vector dirfrac = new_vector(
        1.0/pixel_ray.direction.x,
        1.0/pixel_ray.direction.y,
        1.0/pixel_ray.direction.z
    );
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    double t1 = (bounding_box.min_x - pixel_ray.origin.x)*dirfrac.x;
    double t2 = (bounding_box.max_x - pixel_ray.origin.x)*dirfrac.x;
    double t3 = (bounding_box.min_y - pixel_ray.origin.y)*dirfrac.y;
    double t4 = (bounding_box.max_y - pixel_ray.origin.y)*dirfrac.y;
    double t5 = (bounding_box.min_z - pixel_ray.origin.z)*dirfrac.z;
    double t6 = (bounding_box.max_z - pixel_ray.origin.z)*dirfrac.z;

    double tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
    double tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        return INFINITY;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        return INFINITY;
    }

    return tmin;
}

static int compare_by_x(const void * a, const void * b){
    object* object_01 = *((object**) a);
    object* object_02 = *((object**) b);
    double diff = object_01->bounding_box.center.x - object_02->bounding_box.center.x;
    if((diff - ERROR) < 0.0){
        return -1;
    } else {
        return 1;
    }
}

static int compare_by_y(const void * a, const void * b){
    object* object_01 = *((object**) a);
    object* object_02 = *((object**) b);
    double diff = object_01->bounding_box.center.y - object_02->bounding_box.center.y;
    if((diff - ERROR) < 0.0){
        return -1;
    } else {
        return 1;
    }
}

static int compare_by_z(const void * a, const void * b){
    object* object_01 = *((object**) a);
    object* object_02 = *((object**) b);
    double diff = object_01->bounding_box.center.z - object_02->bounding_box.center.z;
    if((diff - ERROR) < 0.0){
        return -1;
    } else {
        return 1;
    }
}
