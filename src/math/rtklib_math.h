#ifndef RTKLIB_MATH_H
#define RTKLIB_MATH_H

#include <math.h>

#define LOGIC_XOR(x, y) (((x) && (!(y))) || ((!(x)) && (y)))
#define IS_IN_BOUNDS(x, lo, hi) (((x) >= (lo)) && ((x) <= (hi)))
#define VECTOR_3D_SIZE   3

/* basic vector operations */

void vector_copy(const double vec_src[], double vec_dst[], int length);
void vector3_copy(const double vec3_src[], double vec3_dst[]);

void vector_add(double vec[], const double vec_added[], int length);
void vector3_add(double vec3[], const double vec3_added[]);

void vector_subtract(double vec[], const double vec_subtr[], int length);
void vector3_subtract(double vec3[], const double vec3_subtr[]);

void vector_sum(const double vec_1[], const double vec_2[], double vec_result[], int length);
void vector3_sum(const double vec3_1[], const double vec3_2[], double vec3_result[]);

void vector_diff(const double vec_1[], const double vec_2[], double vec_result[], int length);
void vector3_diff(const double vec3_1[], const double vec3_2[], double vec3_result[]);

double vector_scalar_product(const double vec_1[], const double vec_2[], int length);
double vector3_scalar_product(const double vec3_1[], const double vec3_2[]);

void vector_multiply(double multiplier, double vec[], int length);
void vector3_multiply(double multiplier, double vec3[]);

void vector_linear_combination(double weight_1, const double vec_1[],
                               double weight_2, const double vec_2[],
                               double vec_result[], int length);

void vector3_linear_combination(double weight_1, const double vec3_1[],
                                double weight_2, const double vec3_2[],
                                double vec3_result[]);

double vector_norm(const double vec[], int length);
double vector3_norm(const double vec3[]);

double vector_rms(const double vec[], int length);
double vector3_rms(const double vec3[]);

#endif /* #ifndef RTKLIB_MATH_H */
