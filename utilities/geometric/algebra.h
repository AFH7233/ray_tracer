#ifndef _ALGEBRA_
#define _ALGEBRA_
#include <tgmath.h>
#include <stdlib.h>

#define RAND(a, b)  ( a  + ( b + (-1.0 * a )) * (rand() / (double) RAND_MAX) )
#define RAND_INT(a, b)  ( a + ( b + (-1 * a )) * rand())

#define divide(a, k) scale_vector(a, (1.0/k))
#define multiply(a, k) scale_vector(a, k)
#define new_vector(a, b, c) (vector) {.x=a, .y=b, .z=c, .w=1.0}
#define new_normal(a, b, c) to_normal( (vector) {.x=a, .y=b, .z=c, .w=0.0} )
#define new_matrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)  (matrix) { .elements = { {a, b, c, d}, {e, f, g, h}, {i, j, k, l}, {m, n, o, p}} }
#define identity_matrix (matrix) { .elements = { {1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}} }
#define origin_vector (vector) {.x=0.0, .y=0.0, .z=0.0, .w=0.0}
#define ERROR 0.000001

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct vector vector;
struct vector {
  double x;
  double y;
  double z;
  double w;
};
typedef vector normal;

typedef struct matrix matrix;
struct matrix {
  double elements[4][4];
};

double dot(vector a, vector b);
vector cross(vector a, vector b);
vector normalize(vector a);
vector scale_vector(vector a, double k);
vector add_vector(vector a, vector b);
vector sub_vector(vector a, vector b);
vector to_normal(vector a);
double norma(vector a);

vector transform(matrix matriz, vector a);
matrix mul_matrix(matrix a, matrix b);

matrix get_transformation(double rx, double ry, double rz, double tx, double ty, double tz);

normal random_sphere_direction(normal direction, double cone_angle_degree);

#endif
