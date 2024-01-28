#include "stl_reader.h"

static void center_obj(vector *arr, size_t length);

stl_container read_stl_file(const char *file_name, double scale, properties material, matrix transformation) {
  FILE *file = fopen(file_name, "rb");
  if (file == NULL) {
    fprintf(stderr, "Cannot read file\n");
    exit(EXIT_FAILURE);
  }
  char ascii_header[6];
  fread(ascii_header, sizeof(char), 5, file);
  ascii_header[5] = '\0';
  if (strncmp(ascii_header, "solid", 5) == 0) {
    fclose(file);
    fprintf(stderr, "I only can read binary stl\n");
    exit(EXIT_FAILURE);
  }
  fseek(file, 80, SEEK_SET);

  uint32_t faces_size;
  fread(&faces_size, sizeof(uint32_t), 1, file);

  object *triangles = calloc(faces_size, sizeof(object));
  polygon *cloud = malloc(sizeof(polygon));
  cloud->num_vertices = faces_size * 3;
  cloud->vertices = calloc(faces_size * 3, sizeof(vector));
  cloud->normals = calloc(faces_size, sizeof(normal));
  size_t vertices_index = 0;
  size_t normals_index = 0;
  for (size_t i = 0; i < faces_size; i++) {
    float x, y, z;
    fread(&x, sizeof(float), 1, file);
    fread(&y, sizeof(float), 1, file);
    fread(&z, sizeof(float), 1, file);
    normal face_normal = new_normal(x, y, z);

    fread(&x, sizeof(float), 1, file);
    fread(&y, sizeof(float), 1, file);
    fread(&z, sizeof(float), 1, file);
    vector vertex_1 = new_vector(x * scale, y * scale, z * scale);

    fread(&x, sizeof(float), 1, file);
    fread(&y, sizeof(float), 1, file);
    fread(&z, sizeof(float), 1, file);
    vector vertex_2 = new_vector(x * scale, y * scale, z * scale);

    fread(&x, sizeof(float), 1, file);
    fread(&y, sizeof(float), 1, file);
    fread(&z, sizeof(float), 1, file);
    vector vertex_3 = new_vector(x * scale, y * scale, z * scale);

    face *surface = malloc(sizeof(face));
    surface->cloud = cloud;
    surface->indices_vertex[0] = vertices_index;
    surface->indices_vertex[1] = vertices_index + 1;
    surface->indices_vertex[2] = vertices_index + 2;
    surface->index_normal = normals_index;
    surface->has_surface_normal = true;

    cloud->vertices[vertices_index] = vertex_1;
    vertices_index++;
    cloud->vertices[vertices_index] = vertex_2;
    vertices_index++;
    cloud->vertices[vertices_index] = vertex_3;
    vertices_index++;
    cloud->normals[normals_index] = to_normal(face_normal);
    normals_index++;

    triangles[i].geometry = surface;
    triangles[i].bounding_box = get_face_bounding_box(surface);
    triangles[i].surface_area = get_face_area(surface);
    triangles[i].get_geometry_collition = (geometry_collition (*)(void *, ray)) get_face_collition_face_normal;
    triangles[i].get_bounding_box = (box (*)(void *)) get_face_bounding_box;
    triangles[i].transform_geometry = (void (*)(matrix, void *)) transform_face_with_mutation_face_normal;
    triangles[i].material = material;

    fseek(file, 2, SEEK_CUR);
  }

  center_obj(cloud->vertices, cloud->num_vertices);

  for (size_t i = 0; i < cloud->num_vertices; i++) {
    cloud->vertices[i] = transform(transformation, cloud->vertices[i]);
  }

  for (size_t i = 0; i < faces_size; i++) {
    cloud->normals[i] = transform(transformation, cloud->normals[i]);
  }

  fclose(file);
  stl_container container = {
      .length = faces_size,
      .triangles = triangles
  };
  return container;
}

static void center_obj(vector *arr, size_t length) {
  double min_x = INFINITY;
  double min_y = INFINITY;
  double min_z = INFINITY;
  double max_x = -INFINITY;
  double max_y = -INFINITY;
  double max_z = -INFINITY;
  for (size_t i = 0; i < length; i++) {
    min_x = fmin(min_x, arr[i].x);
    min_y = fmin(min_y, arr[i].y);
    min_z = fmin(min_z, arr[i].z);
    max_x = fmax(max_x, arr[i].x);
    max_y = fmax(max_y, arr[i].y);
    max_z = fmax(max_z, arr[i].z);
  }

  double c_x = min_x + (max_x - min_x) / 2.0;
  //double c_y = min_y + (max_y - min_y)/2.0;
  double c_z = min_z + (max_z - min_z) / 2.0;
  vector center = new_vector(c_x, min_y, c_z);

  for (size_t i = 0; i < length; i++) {
    arr[i] = sub_vector(arr[i], center);
  }
}
