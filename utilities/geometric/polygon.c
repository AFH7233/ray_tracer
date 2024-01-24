#include "polygon.h"

geometry_collition get_face_collition(face *restrict surface, ray pixel_ray){
    geometry_collition result = {.is_hit=false};

    polygon* cloud = surface->cloud;
    vector vertex_a = cloud->vertices[surface->indices_vertex[0]];
    vector vertex_b = cloud->vertices[surface->indices_vertex[1]];
    vector vertex_c = cloud->vertices[surface->indices_vertex[2]];

    vector edge_ba = sub_vector(vertex_b, vertex_a);
    vector edge_ca = sub_vector(vertex_c, vertex_a);
    vector h = cross(pixel_ray.direction, edge_ca);
    double a = dot(edge_ba,h);
    if(fabs(a) < ERROR){
        return result;
    }

    double f = 1.0/a;
    vector s = sub_vector(pixel_ray.origin, vertex_a);
    double u = f * dot(s, h);
    if( u < 0.0 || u > 1.0){
        return result;
    }

    vector q = cross(s, edge_ba);
    double v = f * dot(pixel_ray.direction, q);
    if( v < 0.0 || (u + v) > 1.0){
        return result;
    }

    double distance = f * dot(edge_ca, q);
    if(distance > ERROR)
    {
        result.distance = distance;
        result.is_hit = true;
        result.point = get_ray_point(pixel_ray, distance);
    } else {
        return result;
    }

    normal normal_a = cloud->normals[surface->indices_vertex[0]];
    normal normal_b = cloud->normals[surface->indices_vertex[1]];
    normal normal_c = cloud->normals[surface->indices_vertex[2]];

    vector total = cross(edge_ba, edge_ca);
    double areaTotal = norma(total);

    vector line_a = sub_vector(vertex_a, result.point);
    vector line_b = sub_vector(vertex_b, result.point);
    vector line_c = sub_vector(vertex_c, result.point);

    vector t0 = cross(line_b, line_c);
    vector t1 = cross(line_c, line_a);
    vector t2 = cross(line_a, line_b);

    double c0 = norma(t0);
    double c1 = norma(t1);
    double c2 = norma(t2);


    vector surface_normal = divide(add_vector(add_vector(multiply(normal_a, c0), multiply(normal_b, c1)), multiply(normal_c, c2)), areaTotal);
    surface_normal = to_normal(surface_normal);
    result.surface_normal = surface_normal;
    
    return result;
}

void transform_face_with_mutation(matrix transformation, face* surface){
    polygon* cloud = surface->cloud;
    if(!cloud->is_transformed){
        for(size_t i=0; i< cloud->num_vertices; i++){
            cloud->vertices[i] = transform(transformation, cloud->vertices[i]);
            cloud->normals[i] = transform(transformation, cloud->normals[i]);
        }
        cloud->is_transformed = true;
    }
}

box get_face_bounding_box(face *restrict  surface){
    polygon* cloud = surface->cloud;
    vector vertex_a = cloud->vertices[surface->indices_vertex[0]];
    vector vertex_b = cloud->vertices[surface->indices_vertex[1]];
    vector vertex_c = cloud->vertices[surface->indices_vertex[2]];
    box aabb = {
        .max_x = fmax(fmax(vertex_a.x, vertex_b.x), vertex_c.x),
        .max_y = fmax(fmax(vertex_a.y, vertex_b.y), vertex_c.y),
        .max_z = fmax(fmax(vertex_a.z, vertex_b.z), vertex_c.z),
        .min_x = fmin(fmin(vertex_a.x, vertex_b.x), vertex_c.x),
        .min_y = fmin(fmin(vertex_a.y, vertex_b.y), vertex_c.y),
        .min_z = fmin(fmin(vertex_a.z, vertex_b.z), vertex_c.z)
    };

    aabb.center = new_vector(
        aabb.min_x + (aabb.max_x - aabb.min_x)/2.0,
        aabb.min_y + (aabb.max_y - aabb.min_y)/2.0,
        aabb.min_z + (aabb.max_x - aabb.min_z)/2.0
    );

    return aabb;
}

double get_face_area(face *restrict  surface){
    polygon* cloud = surface->cloud;
    vector vertex_a = cloud->vertices[surface->indices_vertex[0]];
    vector vertex_b = cloud->vertices[surface->indices_vertex[1]];
    vector vertex_c = cloud->vertices[surface->indices_vertex[2]];

    vector edge_ba = sub_vector(vertex_b, vertex_a);
    vector edge_ca = sub_vector(vertex_c, vertex_a);

    vector total = cross(edge_ba, edge_ca);
    double areaTotal = norma(total)/2.0;
    return areaTotal;
}

geometry_collition get_face_collition_face_normal(face *restrict surface, ray pixel_ray){
    geometry_collition result = {.is_hit=false};

    polygon* cloud = surface->cloud;
    vector vertex_a = cloud->vertices[surface->indices_vertex[0]];
    vector vertex_b = cloud->vertices[surface->indices_vertex[1]];
    vector vertex_c = cloud->vertices[surface->indices_vertex[2]];

    vector edge_ba = sub_vector(vertex_b, vertex_a);
    vector edge_ca = sub_vector(vertex_c, vertex_a);
    vector h = cross(pixel_ray.direction, edge_ca);
    double a = dot(edge_ba,h);
    if(fabs(a) < ERROR){
        return result;
    }

    double f = 1.0/a;
    vector s = sub_vector(pixel_ray.origin, vertex_a);
    double u = f * dot(s, h);
    if( u < 0.0 || u > 1.0){
        return result;
    }

    vector q = cross(s, edge_ba);
    double v = f * dot(pixel_ray.direction, q);
    if( v < 0.0 || (u + v) > 1.0){
        return result;
    }

    double distance = f * dot(edge_ca, q);
    if(distance > ERROR)
    {
        result.distance = distance;
        result.is_hit = true;
        result.point = get_ray_point(pixel_ray, distance);
    } else {
        return result;
    }

    result.surface_normal = cloud->normals[surface->index_normal];
    
    return result;  
}

void transform_face_with_mutation_face_normal(matrix transformation, face* surface){
    polygon* cloud = surface->cloud;
    if(!cloud->is_transformed){
        for(size_t i=0; i< cloud->num_vertices; i++){
            cloud->vertices[i] = transform(transformation, cloud->vertices[i]);
        }
        size_t num_faces = cloud->num_vertices/3;
        for(size_t i=0; i < num_faces; i++){
            cloud->normals[i] = transform(transformation, cloud->normals[i]);
        }
        cloud->is_transformed = true;
    }
}
