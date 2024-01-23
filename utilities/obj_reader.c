#include "obj_reader.h"

static list_face* triangulate_convex_faces(list_face* head_face);
static normal* create_vertex_normals(object* triangles, polygon* cloud, size_t vertex_count, size_t face_count, object_array* garbage);
static void center_obj(vector* arr, size_t length);

obj_container read_obj_file(char fileName[1], double scale, properties material, matrix transformation, object_array* garbage){
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
    fprintf(stderr, "No pude leer el archivo\n");
        exit(EXIT_FAILURE); 
    }
    const int MAX_LINE = 1000;
    char* line = calloc(MAX_LINE, sizeof(char));
    list_vector* head_vertex = malloc(sizeof(list_vector));
    head_vertex->next = NULL;
    list_vector* current_vertex = head_vertex;
    list_face* head_face = malloc(sizeof(list_face));
    head_face->next = NULL;
    list_face* current_face = head_face;
    size_t vertex_count = 0;
    size_t face_count = 0;
    printf("Empezando a leer el archivo %s\n", fileName);
    while(fgets(line, MAX_LINE, file) != NULL){
        if(line[0] == '#'){
            continue;
        }
        if(line[0] == 'v'){
            if(line[1] != 't' && line[1] != 'n' ){
                double x;
                double y;
                double z;
                sscanf((line+1), "%lf %lf %lf", &x, &y, &z);
                current_vertex->value = new_vector(x,y,z); //mirror in z
                
                vertex_count++;
                current_vertex->next = malloc(sizeof(list_vector));
                current_vertex = current_vertex->next;
                current_vertex->next = NULL;
            }
        }
        if(line[0] == 'f'){
            size_t index = 1;
            char state = 'S';
            while(line[index] != '\0'){
                switch (state)
                {
                case 'S':
                    if(line[index] == '/'){
                        line[index] = ' ';
                        state ='T';
                    } 
                break;
                case 'T':
                    if(line[index] == '/'){
                        state ='N';
                    } else if(line[index] == ' '){
                        state ='S';
                    }
                    line[index] = ' ';
                break;
                case 'N':
                    if(line[index] == ' '){
                        state ='S';
                    } 
                    line[index] = line[index] == '\n'? line[index] : ' ';
                break;
                default:
                    break;
                }
                index++;
            }
            int vertex[4]; 
            int successful = sscanf((line+1), "%d %d %d %d", &vertex[0], &vertex[1], &vertex[2], &vertex[3]);
            if(successful == 4){ 
                face_count += 2;
                current_face->indices_vertex[0] = vertex[0] < 0? (size_t)(vertex[0]+vertex_count):(size_t) (vertex[0]-1); 
                current_face->indices_vertex[1] = vertex[1] < 0? (size_t)(vertex[1]+vertex_count):(size_t)(vertex[1]-1); 
                current_face->indices_vertex[2] = vertex[2] < 0? (size_t)(vertex[2]+vertex_count):(size_t)(vertex[2]-1); 
                current_face->indices_vertex[3] = vertex[3] < 0? (size_t)(vertex[3]+vertex_count):(size_t)(vertex[3]-1); 
                current_face->length = 4;
                current_face->next = malloc(sizeof(list_face));
                current_face = current_face->next;
                current_face->next = NULL;
            } else {
                face_count += 1;
                current_face->indices_vertex[0] = vertex[0] < 0? (size_t)(vertex[0]+vertex_count):(size_t) (vertex[0]-1); 
                current_face->indices_vertex[1] = vertex[1] < 0? (size_t)(vertex[1]+vertex_count):(size_t)(vertex[1]-1); 
                current_face->indices_vertex[2] = vertex[2] < 0? (size_t)(vertex[2]+vertex_count):(size_t)(vertex[2]-1); 
                current_face->length = 3;

                current_face->next = malloc(sizeof(list_face));
                current_face = current_face->next;
                current_face->next = NULL;
            }
        }
    }

    #ifdef _WIN32
        printf("Vertices agregados: %I64d\n", vertex_count);
        printf("Caras agregadas: %I64d\n", face_count);
    #else
        printf("Vertices agregados: %ld\n", vertex_count);
        printf("Caras agregadas: %ld\n", face_count);
    #endif


    list_face* triangulated_faces = triangulate_convex_faces(head_face);
    

    current_face = head_face;
    while (current_face != NULL)
    {
        list_face* temp = current_face->next;
        free(current_face);
        current_face = temp;
    }
    head_face = NULL;

    vector* vertices = calloc(vertex_count, sizeof(vector));
    array_push(garbage, vertices);
    current_vertex = head_vertex;
    size_t index = 0;
    while (current_vertex != NULL)
    {
        if(index < vertex_count){
            vertices[index] = multiply(current_vertex->value, scale);
            index++;
        }
        list_vector* temp = current_vertex->next;
        free(current_vertex);
        current_vertex = temp;
    }
    head_vertex = NULL;

    polygon* cloud = malloc(sizeof(polygon));
    array_push(garbage, cloud);
    cloud->num_vertices = vertex_count;
    cloud->is_transformed = false;
    cloud->vertices = vertices;

    object* triangles = calloc(face_count, sizeof(object));
    current_face = triangulated_faces;
    index = 0;
    while (current_face != NULL)
    {
        list_face* temp = current_face->next;
        if(index < face_count){
            face* surface = malloc(sizeof(face));
            array_push(garbage, surface);
            surface->cloud = cloud;
            surface->indices_vertex[0] = current_face->indices_vertex[0];
            surface->indices_vertex[1] = current_face->indices_vertex[1];
            surface->indices_vertex[2] = current_face->indices_vertex[2];
    
            triangles[index].geometry = surface;
            triangles[index].bounding_box = get_face_bounding_box(surface);
            triangles[index].surface_area = get_face_area(surface);
            triangles[index].get_geometry_collition = (geometry_collition (*) (void*, ray)) get_face_collition;
            triangles[index].get_bounding_box = (box (*) (void*)) get_face_bounding_box;
            triangles[index].transform_geometry = (void (*) (matrix, void*)) transform_face_with_mutation;
            triangles[index].material = material;
            index++;     
        }

        free(current_face);
        current_face = temp;
    }

    triangulated_faces = NULL;
    
    cloud->normals = create_vertex_normals(triangles, cloud, vertex_count, face_count, garbage);
    center_obj(vertices, vertex_count);

    for(size_t i=0; i< vertex_count; i++){
        cloud->vertices[i] = transform(transformation, cloud->vertices[i]);
        cloud->normals[i] = transform(transformation, cloud->normals[i]);
    } 
    
    printf("Added normals\n");
    obj_container result = {
        .length = face_count,
        .triangles = triangles
    };
    free(line);
    fclose(file);
    return result;
}

static void center_obj(vector* arr, size_t length){
    double min_x = INFINITY;
    double min_y = INFINITY;
    double min_z = INFINITY;
    double max_x = -INFINITY;
    double max_y = -INFINITY;
    double max_z = -INFINITY;
    for(size_t i=0; i<length; i++){
        min_x = fmin(min_x, arr[i].x);
        min_y = fmin(min_y, arr[i].y);
        min_z = fmin(min_z, arr[i].z);
        max_x = fmax(max_x, arr[i].x);
        max_y = fmax(max_y, arr[i].y);
        max_z = fmax(max_z, arr[i].z);
    }

    double c_x = min_x + (max_x - min_x)/2.0;
    //double c_y = min_y + (max_y - min_y)/2.0;
    double c_z = min_z + (max_z - min_z)/2.0;
    vector center = new_vector(c_x, min_y, c_z);
    
    for(size_t i=0; i<length; i++){
       arr[i] = sub_vector(arr[i], center);
    } 
}

void free_obj(obj_container arr){
    polygon* cloud = NULL;
    for(size_t i=0; i<arr.length; i++){
        face* surface = (face*) (arr.triangles[i].geometry);
        cloud = surface->cloud;
        free(surface);
    }
    free(arr.triangles);
    free(cloud->normals);
    free(cloud->vertices);
    free(cloud);
}

static normal* create_vertex_normals(object* triangles, polygon* cloud, size_t vertex_count, size_t face_count,  object_array* garbage){

    normal* normals = calloc(vertex_count, sizeof(normal));
    array_push(garbage, normals);
    normal* face_normals = calloc(face_count, sizeof(normal));

    for(size_t i=0; i < face_count; i++){
        face_normals[i] = (normal) {.x =0.0, .y=0.0, .z=0.0, .w=0.0};
    }

    for(size_t i=0; i < vertex_count; i++){
        normals[i] = (normal) {.x =0.0, .y=0.0, .z=0.0, .w=0.0};
    }

    for(size_t i=0; i < face_count; i++){
        face* surface = (face*) (triangles[i].geometry);
        vector vertex01 = cloud->vertices[surface->indices_vertex[0]];
        vector vertex02 = cloud->vertices[surface->indices_vertex[1]];
        vector vertex03 = cloud->vertices[surface->indices_vertex[2]];

        face_normals[i] = cross(normalize(sub_vector(vertex02, vertex01)),  normalize(sub_vector(vertex03, vertex01)));
        double angulos[3];
        angulos[0] = acos(dot(normalize(sub_vector(vertex02, vertex01)),normalize(sub_vector(vertex03, vertex01))));
        angulos[1] = acos(dot(normalize(sub_vector(vertex03, vertex02)),normalize(sub_vector(vertex01, vertex02))));
        angulos[2] = acos(dot(normalize(sub_vector(vertex01, vertex03)),normalize(sub_vector(vertex02, vertex03))));


        for(size_t j=0; j < 3; j++){
            normal result = multiply(face_normals[i], angulos[j]);
            normals[surface->indices_vertex[j]] = add_vector( result , normals[surface->indices_vertex[j]]);
        }

    }

    for(size_t i=0; i < vertex_count; i++){
        normals[i] = to_normal(normals[i]);
        if(isnan(normals[i].x)) {
            normals[i] = (normal) {.x = ERROR, .y=ERROR, .z=ERROR, .w=0.0};
        }
    }
    free(face_normals);
    return normals;
}

static list_face* triangulate_convex_faces(list_face* head_face){
    list_face* triangulated = malloc(sizeof(list_face));
    list_face* current_triangulated = triangulated;
    current_triangulated->next = NULL;

    list_face* current_face = head_face;
    while(current_face != NULL && current_face->next != NULL ){
        if(current_face->length == 4){
            current_triangulated->indices_vertex[0] = current_face->indices_vertex[0];
            current_triangulated->indices_vertex[1] = current_face->indices_vertex[1];
            current_triangulated->indices_vertex[2] = current_face->indices_vertex[2];

            current_triangulated->next = malloc(sizeof(list_face));
            current_triangulated->length = 3;

            current_triangulated = current_triangulated->next;

            current_triangulated->indices_vertex[0] = current_face->indices_vertex[0];
            current_triangulated->indices_vertex[1] = current_face->indices_vertex[2];
            current_triangulated->indices_vertex[2] = current_face->indices_vertex[3];

            current_triangulated->next = malloc(sizeof(list_face));
            current_triangulated->length = 3;
            current_triangulated = current_triangulated->next;
            current_triangulated->next = NULL;
        } else {
            current_triangulated->indices_vertex[0] = current_face->indices_vertex[0];
            current_triangulated->indices_vertex[1] = current_face->indices_vertex[1];
            current_triangulated->indices_vertex[2] = current_face->indices_vertex[2];

            current_triangulated->next = malloc(sizeof(list_face));
            current_triangulated->length = 3;
            current_triangulated = current_triangulated->next;
            current_triangulated->next = NULL;      
        }
        current_face = current_face->next;
    }
    return triangulated;
}
