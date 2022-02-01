#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "utilities/definitions.h"
#include "utilities/logging.h"

#ifndef NUM_RAYS
    #define NUM_RAYS 10
#endif

#ifndef NUM_BOUNCES
    #define NUM_BOUNCES 3
#endif

color_RGB render_pixel(ray pixel_ray, bvh_tree* root, size_t bounces);

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

                ray_color = add_color(ray_color, render_pixel(pixel_ray, tree, 0));
            }

            
            ray_color = divide_color(ray_color, NUM_RAYS);
            pixel_color final_color = to_pixel_color(ray_color);

            put_pixel(screen, i, j, final_color);
        }
        if(width%10 == 0){
            printf("Progress: %I64d\n\r", ((i*100)/width));
        }
    }

    write_bmp("Resultado.bmp", screen);
    free_image(screen);
    free_bvh_tree(tree);
    //free_obj(container);
    free_list(head);
    return EXIT_SUCCESS;
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