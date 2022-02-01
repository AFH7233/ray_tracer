#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "utilities/definitions.h"
#include "utilities/logging.h"

#ifndef NUM_RAYS
    #define NUM_RAYS 1000
#endif

#ifndef NUM_BOUNCES
    #define NUM_BOUNCES 3
#endif

#ifndef NUM_THREADS
    #define NUM_THREADS 50
#endif

color_RGB render_pixel(ray pixel_ray, bvh_tree* root, size_t bounces);
void* render_thread_pixel(void* thread_data);

int main(int argc, char* argv[]){
    
    srand (time(0));

    size_t width = 640UL;
    size_t height = 480UL;
    image screen = new_image(width, height);

    // Setup camera
    camera camara = {
        .fov = 45,
        .position = new_vector(0.0, 15.0, -60.0),
        .up = new_normal(0.0, 1.0, 0.0)
    };

    vector to = new_vector(0.0, 0.0, 100.0);
    matrix look_at = get_look_at(camara, to);

    //Setup distance
    double aspect = width / (double) height;
    double d = sqrt(aspect*aspect + 1.0)/(2.0*tan(M_PI*camara.fov/360.0));

    // Setup objects
    list* head = new_list();
    bvh_tree* tree = new_bvh_tree(X);

    sphere light_geometry = new_sphere(
        25.0, 
        new_vector(0.0, 100.0,0.0)
    );

    properties light_material = {
        .color = new_color_RGB(1.0,1.0,1.0),
        .emmitance = new_color_RGB(10.0, 10.0, 10.0),
        .p_diffract = 1.0,
        .angle_spread_reflect = 0.0
    };

    object light_bola = new_sphere_object(
        &light_geometry,
        light_material
    );       

    transform_object(look_at, &light_bola);
    add_object(tree, &light_bola);

    
    properties material = {
        .color = new_color_RGB(0.722,0.451,0.20),
        .emmitance = new_color_RGB(COLOR_ERROR, COLOR_ERROR, COLOR_ERROR),
        .p_diffract = 0.1,
        .angle_spread_reflect = 5.0
    };

    obj_container container = read_obj_file("teapot.obj", 7.0, material);

    for(size_t i=0; i<container.length; i++){
        transform_object(look_at, &container.triangles[i]);
        add_object(tree, &container.triangles[i]);
    }
    
    normal axis_y = new_normal(0.0,0.1,0.0);
    for(size_t i=0; i< 20; i++){
        normal position = random_sphere_direction(axis_y, 200);
        position.w = 1.0;
        position = multiply(position, 25.0);

        sphere* sphere_geometry = malloc(sizeof(sphere));
        head = add_node(head, sphere_geometry);
        fill_allocated_sphere(
            sphere_geometry,
            RAND(1.0,2.0), 
            position
        );

        color_RGB sphere_color = new_color_RGB(RAND(0.1,1.0), RAND(0.1,1.0), RAND(0.1,1.0));
        
        properties material = {
            .color = sphere_color,
            .emmitance = new_color_RGB(COLOR_ERROR, COLOR_ERROR, COLOR_ERROR),
            .p_diffract = 1.0,
            .angle_spread_reflect = 20.0
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

    printf("Starting rendering\n");
    image strips[NUM_THREADS] = {};
    ray_thread data[NUM_THREADS] = {};
    pthread_t hilos[NUM_THREADS] = {};
    size_t step = (width/NUM_THREADS);
    size_t nose = 0;
    for(size_t index=0; index < NUM_THREADS; index++){
        data[index].aspect = aspect;
        data[index].distance = d;
        if(index == NUM_THREADS-1){
            strips[index] = new_image((width-nose), height);
            data[index].end_w = width;
        } else {
            strips[index] = new_image(step, height);
            data[index].end_w = nose + step;
        }
        data[index].end_h = height;
        data[index].height = height;
        data[index].start_w = nose;
        data[index].start_h = 0;
        data[index].strip = strips[index];
        data[index].tree = tree;
        data[index].width = width;
        pthread_create(&hilos[index], NULL, render_thread_pixel, &data[index]);
        nose += step;
    }

    for(size_t index=0; index < NUM_THREADS; index++){
        pthread_join(hilos[index], NULL);
    }

    for(size_t index=0; index < NUM_THREADS; index++){
        size_t m = 0;
        for(size_t i=data[index].start_w; i<data[index].end_w; i++){
            size_t n = 0;
            for(size_t j=data[index].start_h; j<data[index].end_h; j++){
                pixel_color color = get_pixel(data[index].strip,m,n);
                put_pixel(screen, i, j, color);
                n++;
            }
            m++;
        }
    }


    write_bmp("resultado_threads.bmp", screen);
    for(size_t index=0; index < NUM_THREADS; index++){
        free_image(data[index].strip);
    }
    free_image(screen);
    free_bvh_tree(tree);
    //free_obj(container);
    free_list(head);
    return EXIT_SUCCESS;
}

void* render_thread_pixel(void* thread_data){
    ray_thread* data = (ray_thread*) thread_data;
    size_t m = 0;
    for(size_t i = data->start_w; i<data->end_w; i++){
        size_t n =0;
        for(size_t j = data->start_h; j<data->end_h; j++){
            color_RGB ray_color = new_color_RGB(0.0,0.0,0.0);
            for(size_t k = 0; k<NUM_RAYS; k++){
                double rand_x = 1.0  - sqrt(2.0 - RAND(0.0,2.0));
                double rand_y = 1.0  - sqrt(2.0 - RAND(0.0,2.0));
                double x =  2.0*((i + rand_x)/ (double) data->height) - data->aspect;
                double y = -2.0*((j + rand_y)/ (double) data->height) + 1.0;
                ray pixel_ray = new_ray(
                    origin_vector,
                    new_normal(x, y, data->distance)
                );

                ray_color = add_color(ray_color, render_pixel(pixel_ray, data->tree, 0));
            }

            
            ray_color = divide_color(ray_color, NUM_RAYS);
            pixel_color final_color = to_pixel_color(ray_color);

            put_pixel(data->strip, m, n, final_color);
            n++;
        }
        m++;
    }
    return data;
}

color_RGB render_pixel(ray pixel_ray, bvh_tree* root, size_t bounces){

    collition hitted_object = get_bvh_collition(root, pixel_ray);
    if(hitted_object.is_hit && bounces < NUM_BOUNCES){
        normal surface_normal = hitted_object.normal;
        vector surface_point = hitted_object.point;

        ray generated_pixel_ray = {};
        if(hitted_object.material.p_diffract < RAND(0.0,1.0)){
            generated_pixel_ray = specular_ray(surface_normal, surface_point, pixel_ray, hitted_object.material.angle_spread_reflect);
        } else {
            generated_pixel_ray = diffuse_ray(surface_normal, surface_point);
        }
        
        color_RGB incoming_color = render_pixel(generated_pixel_ray, root, (bounces+1));
        incoming_color = scale_color(incoming_color, dot(surface_normal, generated_pixel_ray.direction));

        color_RGB surface_color = mix_color(hitted_object.material.color, incoming_color);
        color_RGB brdf = add_color(hitted_object.material.emmitance, surface_color);

        return brdf;
    } else if(hitted_object.is_hit){
        return hitted_object.material.emmitance;
    } else {
        //return new_color_RGB( 0.5, 0.7, 1.0);
       return  new_color_RGB(COLOR_ERROR, COLOR_ERROR, COLOR_ERROR); //new_color_RGB( 0.5, 0.7, 1.0);
    } 

}