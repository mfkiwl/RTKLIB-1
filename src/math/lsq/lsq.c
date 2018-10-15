#include "lsq.h"
#include "rtklib_math.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------------- */
/* lsq_input */

bool lsq_input_is_valid(const lsq_input_t *lsq_input)
{
  if (lsq_input == NULL) {
    return false;
  }

  if (!IS_IN_BOUNDS(lsq_input->n_unknowns, 1, lsq_input->max_measurements)) {
    return false;
  }

  if (!IS_IN_BOUNDS(lsq_input->n_measurements, 0, lsq_input->max_measurements)) {
    return false;
  }

  if ((lsq_input->design_matrix == NULL)
      || (lsq_input->measurements == NULL)) {
    return false;
  }

  return true;
}

bool lsq_input_is_ready_for_processing(const lsq_input_t *lsq_input)
{
  if (!lsq_input_is_valid(lsq_input)) {
    return false;
  }

  if (lsq_input->n_measurements < lsq_input->n_unknowns) {
    return false;
  }

  return true;
}

lsq_input_t *lsq_input_init(int n_unknowns, int max_measurements)
{
  assert(IS_IN_BOUNDS(n_unknowns, 1, max_measurements));

  lsq_input_t *lsq_input = (lsq_input_t *) malloc(sizeof(lsq_input_t));

  lsq_input->n_unknowns       = n_unknowns;
  lsq_input->max_measurements = max_measurements;
  lsq_input->n_measurements   = 0;
  lsq_input->design_matrix = (double *) malloc(max_measurements * n_unknowns * sizeof(double));
  lsq_input->measurements  = (double *) malloc(max_measurements * sizeof(double));

  assert(lsq_input_is_valid(lsq_input));

  return lsq_input;
}

void lsq_input_free(lsq_input_t *lsq_input)
{
  assert(lsq_input_is_valid(lsq_input));

  free(lsq_input->design_matrix);
  free(lsq_input->measurements);

  free(lsq_input);
}

void lsq_input_copy(const lsq_input_t *lsq_input_src, lsq_input_t *lsq_input_dst)
{
  assert(lsq_input_is_valid(lsq_input_src));
  assert(lsq_input_dst != NULL);
  assert(lsq_input_src->n_unknowns == lsq_input_dst->n_unknowns);
  assert(lsq_input_src->n_measurements <= lsq_input_dst->max_measurements);

  int nu = lsq_input_src->n_unknowns;
  int nm = lsq_input_src->n_measurements;

  lsq_input_dst->n_measurements = nm;
  memcpy(lsq_input_dst->design_matrix, lsq_input_src->design_matrix, nu * nm * sizeof(double));
  memcpy(lsq_input_dst->measurements, lsq_input_src->measurements, nm * sizeof(double));
}

void lsq_input_add_measurement(lsq_input_t *lsq_input,
                               const double design_matrix_row[], double measurement)
{
  assert(lsq_input_is_valid(lsq_input));
  assert(lsq_input->n_measurements < lsq_input->max_measurements);

  int nu = lsq_input->n_unknowns;
  int nm = lsq_input->n_measurements;

  memcpy(&lsq_input->design_matrix[nm * nu], design_matrix_row, nu * sizeof(double));
  lsq_input->measurements[nm] = measurement;
  lsq_input->n_measurements++;
}

void lsq_input_swap_measurements(lsq_input_t *lsq_input, int i1, int i2)
{
  assert(lsq_input_is_valid(lsq_input));
  assert(IS_IN_BOUNDS(i1, 0, lsq_input->n_measurements - 1));
  assert(IS_IN_BOUNDS(i2, 0, lsq_input->n_measurements - 1));

  if (i1 == i2) {
    return;
  }

  int nu = lsq_input->n_unknowns;

  double *design_matrix_row = (double *) malloc(nu * sizeof(double));
  double meas;

  memcpy(design_matrix_row, &lsq_input->design_matrix[i1 * nu], nu * sizeof(double));
  meas = lsq_input->measurements[i1];

  memcpy(&lsq_input->design_matrix[i1 * nu], &lsq_input->design_matrix[i2 * nu], nu * sizeof(double));
  lsq_input->measurements[i1] = lsq_input->measurements[i2];

  memcpy(&lsq_input->design_matrix[i2 * nu], design_matrix_row, nu * sizeof(double));
  lsq_input->measurements[i2] = meas;

  free(design_matrix_row);
}

void lsq_generate_random_trimmed_problem(const lsq_input_t *input_data,
                                         lsq_input_t *trimmed_data, int size_trimmed)
{
  assert(lsq_input_is_valid(input_data));
  assert(trimmed_data != NULL);
  assert(IS_IN_BOUNDS(size_trimmed, input_data->n_unknowns, input_data->n_measurements));

  int nm = input_data->n_measurements;
  int index_to_include;

  lsq_input_copy(input_data, trimmed_data);

  for (int i = 0; i < size_trimmed; i++) {
      index_to_include = i + rand() % (nm - i);
      lsq_input_swap_measurements(trimmed_data, i, index_to_include);
  }

  trimmed_data->n_measurements = size_trimmed;
}

void lsq_find_residuals(const lsq_input_t *lsq_input, const double solution[], double residuals[])
{
  assert(lsq_input_is_valid(lsq_input));
  assert(solution  != NULL);
  assert(residuals != NULL);

  int nu = lsq_input->n_unknowns;
  int nm = lsq_input->n_measurements;
  double *design_matrix_row;

  for (int i = 0; i < nm; i++) {
    design_matrix_row = &lsq_input->design_matrix[i * nu];

    residuals[i] = lsq_input->measurements[i]
      - vector_scalar_product(design_matrix_row, solution, nu);
  }
}

/* -------------------------------------------------------------------------------- */
/* lsq_sol */

bool lsq_sol_is_valid(const lsq_sol_t *lsq_sol)
{
  if (lsq_sol == NULL) {
    return false;
  }

  if (!IS_IN_BOUNDS(lsq_sol->n_unknowns, 1, lsq_sol->n_measurements)) {
    return false;
  }

  if (lsq_sol->n_measurements > lsq_sol->max_measurements) {
    return false;
  }

  if ((lsq_sol->solution == NULL)
      || (lsq_sol->residuals == NULL)) {
    return false;
  }

  return true;
}

lsq_sol_t *lsq_sol_init(int n_unknowns, int max_measurements)
{
  assert(IS_IN_BOUNDS(n_unknowns, 1, max_measurements));

  lsq_sol_t *lsq_sol = (lsq_sol_t *) malloc(sizeof(lsq_sol_t));

  lsq_sol->n_unknowns = n_unknowns;
  lsq_sol->n_measurements = max_measurements;
  lsq_sol->max_measurements = max_measurements;
  lsq_sol->solution  = (double *) malloc(n_unknowns * sizeof(double));
  lsq_sol->residuals = (double *) malloc(max_measurements * sizeof(double));

  assert(lsq_sol_is_valid(lsq_sol));

  return lsq_sol;
}

void lsq_sol_free(lsq_sol_t *lsq_sol)
{
  assert(lsq_sol_is_valid(lsq_sol));

  free(lsq_sol->solution);
  free(lsq_sol->residuals);

  free(lsq_sol);
}

bool lsq_sol_is_in_agreement_with_lsq_input(const lsq_input_t *lsq_input, const lsq_sol_t *lsq_sol)
{
  if (!lsq_input_is_valid(lsq_input)) {
    return false;
  }

  if (!lsq_sol_is_valid(lsq_sol)) {
    return false;
  }

  if (lsq_input->n_unknowns != lsq_sol->n_unknowns) {
    return false;
  }

  if (lsq_input->n_measurements > lsq_sol->max_measurements) {
    return false;
  }

  return true;
}
