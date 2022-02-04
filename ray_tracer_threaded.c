#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "utilities/scene_reader.h"
#include "utilities/definitions.h"
#include "utilities/logging.h"

#ifndef NUM_RAYS
    #define NUM_RAYS 40000
#endif

#ifndef NUM_BOUNCES
    #define NUM_BOUNCES 3
#endif

#ifndef NUM_THREADS
    #define NUM_THREADS 10
#endif

#ifndef REGION_SIZE
    #define REGION_SIZE 10
#endif

color_RGB render_pixel(ray pixel_ray, bvh_tree* root, size_t bounces);
void* render_thread_pixel(void* thread_data);

int main(int argc, char* argv[]){
    
    srand (time(0));
    scene escena = read_scene(argv[1]);

    size_t width = escena.width;
    size_t height = escena.height;
    image screen = new_image(width, height);

    // Setup camera
    camera camara = escena.camara;
    vector to = escena.focus;
    matrix look_at = get_look_at(camara, to);

    //Setup distance
    double aspect = width / (double) height;
    double d = sqrt(aspect*aspect + 1.0)/(2.0*tan(M_PI*camara.fov/360.0));

    // Setup objects
    list* head = new_list();
    bvh_tree* tree = new_bvh_tree(X);

    list* current = escena.objects;
    while (current != NULL)
    {
        if(current->value != NULL){
            object* thing = current->value;
            transform_object(look_at, thing);
            add_object(tree, thing);
        }

        current = current->next;
    }


    /*obj_container container = read_obj_file("teapot.obj", 7.0, material);

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
            .emmitance = COLOR_ERROR,
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
    }*/

    printf("Preparing bvh tree\n");
    distribute_bvh(tree);

    /// I'll divide the image in squares of REGION_SIZE and the threads will work on them once one is freed 
    printf("Creating tasks\n");
    size_t squares_width = (width/REGION_SIZE);
    squares_width = (width%REGION_SIZE) > 0 ? squares_width + 1 : squares_width;

    size_t squares_height = (height/REGION_SIZE);
    squares_height = (height%REGION_SIZE) > 0 ? squares_height + 1 : squares_height;

    size_t squares_total = squares_width*squares_height;

    image* strips = calloc(squares_total,sizeof(image));
    ray_thread* data = calloc(squares_total, sizeof(ray_thread));
    pthread_t* hilos = calloc(squares_total, sizeof(pthread_t));
    size_t index_w = 0;
    size_t index_h = 0;
    size_t index = 0;
    for(size_t i=0; i < squares_width; i++){
        for(size_t j=0; j < squares_height; j++){
            data[index].aspect = aspect;
            data[index].distance = d;
            if((index_w + REGION_SIZE) >= width){
                if((index_h + REGION_SIZE) >= height){
                    strips[index] = new_image((width-index_w), (height-index_h));
                    data[index].end_w = width;
                    data[index].end_h = height;
                } else {
                    strips[index] = new_image((width-index_w), REGION_SIZE);
                    data[index].end_w = width;
                    data[index].end_h = index_h + REGION_SIZE;
                }
            } else {
                if((index_h + REGION_SIZE) >= height){
                    strips[index] = new_image(REGION_SIZE, (height-index_h));
                    data[index].end_w = index_w + REGION_SIZE;
                    data[index].end_h = height;
                } else {
                    strips[index] = new_image(REGION_SIZE, REGION_SIZE);
                    data[index].end_w = index_w + REGION_SIZE;
                    data[index].end_h = index_h + REGION_SIZE;
                }
            }
            data[index].height = height;
            data[index].start_w = index_w;
            data[index].start_h = index_h;
            data[index].strip = strips[index];
            data[index].tree = tree;
            data[index].width = width;
            data[index].rays_per_pixel = escena.rays_per_pixel;
            data[index].status = IDLE;
            index++;

            index_h += REGION_SIZE;
            if(index_h >= height){
                index_h = 0;
            }
        }
        index_w += REGION_SIZE;
    }

    printf("Starting rendering\n");
    size_t available = escena.threads;
    size_t finished_tasks = 0;
    size_t k = 0;
    while(true){
        if(data[k].status == IDLE && available > 0){
            data[k].status = IN_PROGRESS;
            pthread_create(&hilos[k], NULL, render_thread_pixel, &data[k]);
            available--;
        } else if(data[k].status == FINISHED){
            data[k].status = PROCESSED;
            size_t m = 0;
            for(size_t i=data[k].start_w; i<data[k].end_w; i++){
                size_t n = 0;
                for(size_t j=data[k].start_h; j<data[k].end_h; j++){
                    pixel_color color = get_pixel(data[k].strip,m,n);
                    put_pixel(screen, i, j, color);
                    n++;
                }
                m++;
            }
            available = available >= escena.threads? escena.threads:available+1;
            finished_tasks++;
            printf("Finished rendering %I64d regions of %I64d\n", finished_tasks, squares_total);
            if(finished_tasks == squares_total){
                break;
            }
        } 
        k = (k >= squares_total - 1)? 0 : k+1;
    }
    write_bmp("resultado_threads.bmp", screen);
    for(size_t index=0; index < squares_total; index++){
        free_image(data[index].strip);
    }
    free(data);
    free(hilos);
    free(strips);
    free_image(screen);
    free_bvh_tree(tree);
    //free_obj(container);
    free_list(head);
    return EXIT_SUCCESS;
}

void* render_thread_pixel(void* thread_data){
    ray_thread* data = (ray_thread*) thread_data;
    data->status = IN_PROGRESS;
    size_t m = 0;
    for(size_t i = data->start_w; i<data->end_w; i++){
        size_t n =0;
        for(size_t j = data->start_h; j<data->end_h; j++){
            color_RGB ray_color = new_color_RGB(0.0,0.0,0.0);
            for(size_t k = 0; k<data->rays_per_pixel; k++){
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

            
            ray_color = divide_color(ray_color, data->rays_per_pixel);
            pixel_color final_color = to_pixel_color(ray_color);

            put_pixel(data->strip, m, n, final_color);
            n++;
        }
        m++;
    }
    data->status = FINISHED;
    return data;
}

color_RGB render_pixel(ray pixel_ray, bvh_tree* root, size_t bounces){

    collition hitted_object = get_bvh_collition(root, pixel_ray);
    if(hitted_object.is_hit && bounces > 0){
        normal surface_normal = hitted_object.normal;
        vector surface_point = hitted_object.point;

        ray generated_pixel_ray = {};
        if(hitted_object.material.p_diffract < RAND(0.0,1.0)){
            generated_pixel_ray = specular_ray(surface_normal, surface_point, pixel_ray, hitted_object.material.angle_spread_reflect);
        } else {
            generated_pixel_ray = diffuse_ray(surface_normal, surface_point);
        }
        
        color_RGB incoming_color = render_pixel(generated_pixel_ray, root, (bounces-1));
        incoming_color = scale_color(incoming_color, dot(surface_normal, generated_pixel_ray.direction));

        color_RGB surface_color = mix_color(hitted_object.material.color, incoming_color);
        color_RGB emmitance = scale_color(hitted_object.material.color, hitted_object.material.emmitance);
        color_RGB brdf = add_color(emmitance, surface_color);

        return brdf;
    } else if(hitted_object.is_hit){
        color_RGB emmitance = scale_color(hitted_object.material.color, hitted_object.material.emmitance);
        return emmitance;
    } else {
        //return new_color_RGB( 0.5, 0.7, 1.0);
       return  new_color_RGB(COLOR_ERROR, COLOR_ERROR, COLOR_ERROR); //new_color_RGB( 0.5, 0.7, 1.0);
    } 

}