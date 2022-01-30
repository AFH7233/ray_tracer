#include "algebra.h"

double dot(vector a, vector b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

vector cross(vector a, vector b){
    vector c = {};
    c.x = (a.y*b.z) - (a.z*b.y);
    c.y = (a.z*b.x) - (a.x*b.z);
    c.z = (a.x*b.y) - (a.y*b.x);
    c.w = a.w*b.w;
    return c;
}

vector normalize(vector a){
    double norm = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
    vector c = {};
    c.x = a.x/norm;
    c.y = a.y/norm;
    c.z = a.z/norm;
    c.w = a.w;
    return c;
}

vector scale_vector(vector a, double k){
    vector c = {};
    c.x = a.x*k;
    c.y = a.y*k;
    c.z = a.z*k;
    c.w = a.w;
    return c;
}

vector add_vector(vector a, vector b){
    vector c = {};
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    c.w = a.w * b.w;
    return c;
}

vector sub_vector(vector a, vector b){
    vector c = {};
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    c.w = a.w * b.w;
    return c;
}


vector to_normal(vector a){
    vector c = normalize(a);
    c.w = 0.0;
    return c;
}


vector trasnform(matrix matriz, vector a){
    vector c = {};
    c.x = matriz.elements[0][0]*a.x + matriz.elements[0][1]*a.y + matriz.elements[0][2]*a.z + matriz.elements[0][3]*a.w;
    c.y = matriz.elements[1][0]*a.x + matriz.elements[1][1]*a.y + matriz.elements[1][2]*a.z + matriz.elements[1][3]*a.w;
    c.z = matriz.elements[2][0]*a.x + matriz.elements[2][1]*a.y + matriz.elements[2][2]*a.z + matriz.elements[2][3]*a.w;
    c.w = matriz.elements[3][0]*a.x + matriz.elements[3][1]*a.y + matriz.elements[3][2]*a.z + matriz.elements[3][3]*a.w;
    return c;
}


matrix mul_matrix(matrix a, matrix b){
    matrix c = { .elements = {} };
    for(size_t i=0; i<4; i++){
        for(size_t j=0; j<4; j++){
            for(size_t k=0; k<4; k++){
                c.elements[i][j] += (a.elements[i][k] * b.elements[k][j]);
            }
        }
    }
    return c;
}