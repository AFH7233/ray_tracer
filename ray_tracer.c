#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "utilities/definitions.h"
#include "utilities/logging.h"

#ifndef NUM_RAYS
    #define NUM_RAYS 100
#endif

color_RGB render_pixel(ray pixel_ray, bvh_tree* root);

int main(int argc, char* argv[]){
    
    srand(time(NULL));

    size_t width = 640UL;
    size_t height = 480UL;
    image screen = new_image(width, height);

    // Setup camera
    camera camara = {
        .fov = 90,
        .position = new_vector(0.0, 0.0, -20.0),
        .up = new_normal(0.0, 1.0, 0.0)
    };

    vector to = new_vector(0.0, 0.0, 0.0);
    matrix look_at = get_look_at(camara, to);

    //Setup distance
    double aspect = width / (double) height;
    double d = sqrt(aspect*aspect + 1.0)/(2.0*tan(M_PI*camara.fov/360.0));

    // Setup objects
    list* head = new_list();
    bvh_tree* tree = new_bvh_tree(X);
    for(size_t i=0; i< 10; i++){
        sphere* sphere_geometry = malloc(sizeof(sphere));
        head = add_node(head, sphere_geometry);
        fill_allocated_sphere(
            sphere_geometry,
            RAND(1.0,3.0), 
            new_vector(RAND(-5.0,5.0), RAND(-5.0,5.0), RAND(-5.0,5.0))
        );

        properties material = {
            .color = new_color_RGB(RAND(0.0,1.0),RAND(0.0,1.0),RAND(0.0,1.0))
        };

        object* bola = malloc(sizeof(object));
        head = add_node(head, bola);
        fill_allocated_sphere_object(
            bola,
            sphere_geometry,
            material
        );

        transform_object(look_at, bola);
        add_object(tree, bola);
    }

    distribute_bvh(tree);

    for(size_t i = 0; i<width; i++){
        for(size_t j = 0; j<height; j++){
            color_RGB ray_color = new_color_RGB(0.0,0.0,0.0);
            for(size_t k = 0; k<NUM_RAYS; k++){
                double rand_x = 1.0  - sqrt(2.0 - RAND(0.0,2.0));
                double rand_y = 1.0  - sqrt(2.0 - RAND(0.0,2.0));
                double x =  2.0*((i + rand_x)/ (double) height) - aspect;
                double y = -2.0*((j + rand_y)/ (double) height) + 1.0;
                ray pixel_ray = new_ray(
                    origin_vector,
                    new_normal(x, y, d)
                );

                ray_color = add_color(ray_color, render_pixel(pixel_ray, tree));
            }

            
            ray_color = divide_color(ray_color, NUM_RAYS);
            pixel_color final_color = to_pixel_color(ray_color);

            put_pixel(screen, i, j, final_color);
        }
    }

    write_bmp("Resultado.bmp", screen);
    free_image(screen);
    free_bvh_tree(tree);
    free_list(head);
    return EXIT_SUCCESS;
}


color_RGB render_pixel(ray pixel_ray, bvh_tree* root){
    collition hitted_object = get_bvh_collition(root, pixel_ray);
    if(hitted_object.is_hit){
        //normal surface_normal = hitted_object.normal;
        //color_RGB sufrace_color = new_color_RGB( surface_normal.x, surface_normal.y, surface_normal.z);
        return hitted_object.material.color;
    } else {
        return new_color_RGB( 0.0, 0.0, 0.0);
    }
}