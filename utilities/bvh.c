#include "bvh.h"

static int compare_by_x(const void * a, const void * b);
static int compare_by_y(const void * a, const void * b);
static int compare_by_z(const void * a, const void * b);
static bool is_collition_dected(box bounding_box, ray pixel_ray);


bvh_tree* new_bvh_tree(axis eje){
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
    root->eje = eje;
    root->bounding_box = (box) {
        .min_x = DBL_MAX,
        .min_y = DBL_MAX,
        .min_z = DBL_MAX,
        .max_x = DBL_MIN,
        .max_y = DBL_MIN,
        .max_z = DBL_MIN

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
    if(root->num_of_objects < 5){
        return;
    }

    root->is_leaf = false;

    switch (root->eje)
    {
    case X:
        root->left = new_bvh_tree(Y);
        root->right = new_bvh_tree(Y);
        qsort(root->object_array, root->num_of_objects, sizeof(object*), compare_by_x);

        break;
    case Y:
        root->left = new_bvh_tree(Z);
        root->right = new_bvh_tree(Z);
        qsort(root->object_array, root->num_of_objects, sizeof(object*), compare_by_y);
        break;
    case Z:
        root->left = new_bvh_tree(X);
        root->right = new_bvh_tree(X);
        qsort(root->object_array, root->num_of_objects, sizeof(object*), compare_by_z);
        break;
    default:
        break;
    }
 

    size_t length = root->num_of_objects;
    double* areas = calloc(length, sizeof(double));
    object** array = root->object_array;

    areas[0] = (array[0])->surface_area;
    for(size_t i=1; i < length; i++){
        areas[i] = (array[i])->surface_area;
        areas[i] += areas[i-1];
    }

    double min_area = DBL_MAX;
    size_t cut = 0;
    for(size_t i=0; i < length; i++){
        double left_area = areas[i];
        double right_area = areas[length-1] - areas[i];

        double thing = (i + 1.0)*(left_area) + (length-i-1.0)*(right_area);
        if(thing < min_area){
            cut = i;
            min_area = thing;
        }
    }

    for(size_t i=0; i < cut+1; i++){
        add_object(root->left, array[i]);
    }

    for(size_t i=cut+1; i < length; i++){
        add_object(root->right, array[i]);
    }

    free_node_object_list(root);
    free(areas);

    distribute_bvh(root->left);
    distribute_bvh(root->right);


    return;
}
  

void free_node_object_list(bvh_tree* root){
    free(root->object_array);
    root->num_of_objects = 0;
    root->cap_of_objects = 0;
    root->object_array = NULL;
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

collition get_bvh_collition(bvh_tree* root, ray pixel_ray){
    collition result = {.is_hit = false};
    if(root == NULL){
        return result;
    }

    bool is_collition = is_collition_dected(root->bounding_box, pixel_ray);
    if (is_collition && root->is_leaf)
    {
        size_t length = root->num_of_objects;
        object** array = root->object_array;
        double distance = DBL_MAX;
        for(size_t i=0; i < length; i++){
            collition object_collition = get_collition(array[i], pixel_ray);
            if(object_collition.is_hit && object_collition.distance < distance){
                distance = object_collition.distance;
                result = object_collition;
            }
        }
        return result;
    } else if(is_collition && !root->is_leaf){
        collition left_collition = get_bvh_collition(root->left, pixel_ray);
        collition right_collition = get_bvh_collition(root->right, pixel_ray);
        if(left_collition.is_hit && right_collition.is_hit){
            if(left_collition.distance < right_collition.distance){
                return left_collition;
            } else {
                return right_collition;
            }
        } else if(left_collition.is_hit){
             return left_collition;
        } else if(right_collition.is_hit){
            return right_collition;
        } else {
            return result;
        }
        
    } else {
        return result;
    }
    
}


//It was copied from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
static bool is_collition_dected(box bounding_box , ray pixel_ray){
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
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        return false;
    }

    return true;
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