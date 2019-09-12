/*
 *   Iteratively Reveighted Least Squares (IRLS) solver
 */

#include "irls.h"
#include "lsq.h"
#include "ols.h"
#include "rtklib_math.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

#define MAX(x, y)   (((x) >= (y)) ? (x) : (y))

bool lsq_reweighted_options_is_valid(const lsq_reweighted_options_t *options)
{
  if (options == NULL) {
    return false;
  }

  if (options->max_iter < 1) {
    return false;
  }

  if (options->downweight_thres <= 0.0) {
    return false;
  }

  if (!IS_IN_BOUNDS(options->min_inliers_proportion, 0.0, 1.0)) {
    return false;
  }

  if (options->precision <= 0.0) {
    return false;
  }

  return true;
}

void lsq_reweighted_options_set(lsq_reweighted_options_t *options, int max_iter,
  double downweight_thres, double min_inliers_proportion, double precision)
{
  assert(options != NULL);

  options->max_iter = max_iter;
  options->downweight_thres = downweight_thres;
  options->min_inliers_proportion = min_inliers_proportion;
  options->precision = precision;

  assert(lsq_reweighted_options_is_valid(options));
}

/* return: 0: fail, 1: succeed */
int lsq_reweighted(const lsq_input_t *input_data, lsq_sol_t *solution,
                   const lsq_reweighted_options_t *options)
{
  assert(lsq_input_is_ready_for_processing(input_data));
  assert(lsq_sol_is_valid(solution));
  assert(lsq_sol_is_in_agreement_with_lsq_input(input_data, solution));
  assert(lsq_reweighted_options_is_valid(options));

  int nu = input_data->n_unknowns;
  int nm = input_data->n_measurements;

  lsq_input_t *input_data_weighted = lsq_input_init(nu, input_data->max_measurements);
  lsq_input_copy(input_data, input_data_weighted);

  double *solution_delta = (double *) malloc(nu * sizeof(double));
  double dw_thres_inverted = 1.0 / options->downweight_thres;
  double residual_abs, weight;
  double *design_matrix_row;
  double *design_matrix_weighted_row;
  int iter, n_good_residuals = 0;

  for (iter = 0; iter < options->max_iter; iter++) {
    /* store previous solution */
    vector_copy(solution->solution, solution_delta, nu);

    /* solve OLS */
    lsq_ols_standard(input_data_weighted, solution);
    lsq_find_residuals(input_data, solution->solution, solution->residuals);

    n_good_residuals = 0;
    for (int i = 0; i < nm; i++) {
      residual_abs = fabs(solution->residuals[i]);
      if (residual_abs < options->downweight_thres) {
        n_good_residuals++;
      }
    }

    /* early out conditions */
    if (n_good_residuals == nm) {
      break;
    }

    if (iter > 0) {
      vector_subtract(solution_delta, solution->solution, nu);

      if (vector_norm(solution_delta, nu) < options->precision) {
        break;
      }
    }

    /* reweighting */
    for (int i = 0; i < nm; i++) {
      residual_abs = fabs(solution->residuals[i]);
      if (residual_abs > options->downweight_thres) {
        weight = exp(-(residual_abs * dw_thres_inverted + 1.0) * 0.5);
      }
      else {
        weight = 1.0;
      }

      design_matrix_row = &input_data->design_matrix[i * nu];
      design_matrix_weighted_row = &input_data_weighted->design_matrix[i * nu];
      vector_copy(design_matrix_row, design_matrix_weighted_row, nu);
      vector_multiply(weight, design_matrix_weighted_row, nu);

      input_data_weighted->measurements[i] = input_data->measurements[i] * weight;
    }
  }

  lsq_input_free(input_data_weighted);
  free(solution_delta);

  /* lack of good residuals means instability */
  if ((n_good_residuals < MAX(options->min_inliers_proportion * nm, nu + 1))
      || (iter >= options->max_iter)) {
    return 0;
  }

  return 1;
}
