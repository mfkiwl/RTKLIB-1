#include "rtklib_math.h"
#include <string.h>
#include <assert.h>
#include <math.h>

#define SQR(x)      ((x) * (x))

/* basic vector operations */

void vector_copy(const double vec_src[], double vec_dst[], int length)
{
  assert(vec_src != NULL);
  assert(vec_dst != NULL);
  assert(length > 0);

  memcpy(vec_dst, vec_src, length * sizeof(double));
}

void vector3_copy(const double vec3_src[], double vec3_dst[])
{
  assert(vec3_src != NULL);
  assert(vec3_dst != NULL);

  vector_copy(vec3_src, vec3_dst, VECTOR_3D_SIZE);
}

void vector_add(double vec[], const double vec_added[], int length)
{
  assert(vec != NULL);
  assert(vec_added != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec[i] += vec_added[i];
  }
}

void vector3_add(double vec3[], const double vec3_added[])
{
  assert(vec3 != NULL);
  assert(vec3_added != NULL);

  vector_add(vec3, vec3_added, VECTOR_3D_SIZE);
}

void vector_subtract(double vec[], const double vec_subtr[], int length)
{
  assert(vec != NULL);
  assert(vec_subtr != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec[i] -= vec_subtr[i];
  }
}

void vector3_subtract(double vec3[], const double vec3_subtr[])
{
  assert(vec3 != NULL);
  assert(vec3_subtr != NULL);

  vector_subtract(vec3, vec3_subtr, VECTOR_3D_SIZE);
}

void vector_sum(const double vec_1[], const double vec_2[], double vec_result[], int length)
{
  assert(vec_1 != NULL);
  assert(vec_2 != NULL);
  assert(vec_result != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec_result[i] = vec_1[i] + vec_2[i];
  }
}

void vector3_sum(const double vec3_1[], const double vec3_2[], double vec3_result[])
{
  assert(vec3_1 != NULL);
  assert(vec3_2 != NULL);
  assert(vec3_result != NULL);

  vector_sum(vec3_1, vec3_2, vec3_result, VECTOR_3D_SIZE);
}

void vector_diff(const double vec_1[], const double vec_2[], double vec_result[], int length)
{
  assert(vec_1 != NULL);
  assert(vec_2 != NULL);
  assert(vec_result != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec_result[i] = vec_1[i] - vec_2[i];
  }
}

void vector3_diff(const double vec3_1[], const double vec3_2[], double vec3_result[])
{
  assert(vec3_1 != NULL);
  assert(vec3_2 != NULL);
  assert(vec3_result != NULL);

  vector_diff(vec3_1, vec3_2, vec3_result, VECTOR_3D_SIZE);
}

double vector_scalar_product(const double vec_1[], const double vec_2[], int length)
{
  assert(vec_1 != NULL);
  assert(vec_2 != NULL);
  assert(length > 0);

  double scalar_product = 0.0;

  for (int i = 0; i < length; i++) {
    scalar_product += vec_1[i] * vec_2[i];
  }

  return scalar_product;
}

double vector3_scalar_product(const double vec3_1[], const double vec3_2[])
{
  assert(vec3_1 != NULL);
  assert(vec3_2 != NULL);

  return vector_scalar_product(vec3_1, vec3_2, VECTOR_3D_SIZE);
}

void vector_multiply(double multiplier, double vec[], int length)
{
  assert(vec != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec[i] *= multiplier;
  }
}

void vector3_multiply(double multiplier, double vec3[])
{
  assert(vec3 != NULL);

  vector_multiply(multiplier, vec3, VECTOR_3D_SIZE);
}

void vector_linear_combination(double weight_1, const double vec_1[],
                               double weight_2, const double vec_2[],
                               double vec_result[], int length)
{
  assert(vec_1 != NULL);
  assert(vec_2 != NULL);
  assert(vec_result != NULL);
  assert(length > 0);

  for (int i = 0; i < length; i++) {
    vec_result[i] = weight_1 * vec_1[i] + weight_2 * vec_2[i];
  }
}

void vector3_linear_combination(double weight_1, const double vec3_1[],
                                double weight_2, const double vec3_2[],
                                double vec3_result[])
{
  assert(vec3_1 != NULL);
  assert(vec3_2 != NULL);
  assert(vec3_result != NULL);

  vector_linear_combination(weight_1, vec3_1, weight_2, vec3_2, vec3_result, VECTOR_3D_SIZE);
}

double vector_norm(const double vec[], int length)
{
  assert(vec != NULL);
  assert(length > 0);

  double sum_sqr = 0.0;
  double norm;

  for (int i = 0; i < length; i++) {
    sum_sqr += SQR(vec[i]);
  }
  norm = sqrt(sum_sqr);

  return norm;
}

double vector3_norm(const double vec3[])
{
  assert(vec3 != NULL);

  return vector_norm(vec3, VECTOR_3D_SIZE);
}

double vector_rms(const double vec[], int length)
{
  assert(vec != NULL);
  assert(length > 0);

  double sum_sqr = 0.0;
  double rms;

  for (int i = 0; i < length; i++) {
    sum_sqr += SQR(vec[i]);
  }
  rms = sqrt(sum_sqr / length);

  return rms;
}

double vector3_rms(const double vec3[])
{
  assert(vec3 != NULL);

  return vector_rms(vec3, VECTOR_3D_SIZE);
}
