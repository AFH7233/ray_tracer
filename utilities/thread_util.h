#ifndef _THREAD_UTIL_
    #define _THREAD_UTIL_
    #include <stdlib.h>
    #include <stdbool.h>
    #include "bvh.h"
    #include "image.h"

    typedef enum thread_status thread_status;
    enum thread_status { IDLE, IN_PROGRESS, FINISHED, PROCESSED};

    typedef struct ray_thread ray_thread;
    struct  ray_thread
    {
        size_t rays_per_pixel;
        size_t start_w;
        size_t end_w;
        size_t start_h;
        size_t end_h;
        size_t width;
        size_t height;
        size_t bounces;
        double aspect;
        double distance;
        bvh_tree* tree;
        image strip;
        volatile _Atomic( thread_status ) status ;
    };

#endif