/*
 *   Robust Least Squares algorithm combining OLS, IRLS and RANSAC techniques
 */

#include "robust_lsq.h"
#include "lsq.h"
#include "ols.h"
#include "irls.h"
#include "ransac.h"
#include "rtklib_math.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define MAX(x, y)   (((x) >= (y)) ? (x) : (y))
#define MIN_INLIERS_PROPORTION_FOR_IRLS   0.80

bool lsq_robust_options_is_valid(const lsq_robust_options_t *options)
{
  if (options == NULL) {
    return false;
  }

  if (!IS_IN_BOUNDS(options->ransac_min_samples, 1, options->ransac_max_samples)) {
    return false;
  }

  if (options->irls_max_iter <= 0) {
    return false;
  }

  if (options->outlier_thres <= 0.0) {
    return false;
  }

  if (options->fine_thres <= 0.0) {
    return false;
  }

  if (options->precision < 0.0) {
    return false;
  }

  return true;
}

void lsq_robust_options_set(lsq_robust_options_t *options, int ransac_min_samples,
  int ransac_max_samples, int irls_max_iter, double outlier_thres, double fine_thres, double precision)
{
  assert(options != NULL);

  options->ransac_min_samples = ransac_min_samples;
  options->ransac_max_samples = ransac_max_samples;
  options->irls_max_iter = irls_max_iter;
  options->outlier_thres = outlier_thres;
  options->fine_thres = fine_thres;
  options->precision = precision;

  assert(lsq_robust_options_is_valid(options));
}

static bool lsq_robust_find_approximate_solution(const lsq_input_t *input_data,
  lsq_sol_t *solution, const lsq_robust_options_t *options)
{
  assert(lsq_input_is_ready_for_processing(input_data));
  assert(lsq_sol_is_valid(solution));
  assert(lsq_sol_is_in_agreement_with_lsq_input(input_data, solution));
  assert(lsq_robust_options_is_valid(options));

  int nu = input_data->n_unknowns;
  int nm = input_data->n_measurements;
  double residual_abs;

  if (nm <= nu) { /* there is no robust solution */
    return false;
  }

  bool succeed = false;
  lsq_reweighted_options_t irls_options;
  lsq_ransac_options_t ransac_options;

  while (1) {
    /* try OLS */
    lsq_ols_standard(input_data, solution);

    succeed = true;
    for (int i = 0; i < nm; i++) {
      residual_abs = fabs(solution->residuals[i]);
      if (residual_abs > options->outlier_thres) {

        succeed = false;
        break;
      }
    }

    if (succeed) { /* OLS solution is good */
      break;
    }

    if (nm <= (nu + 1)) { /* we have nothing to do in such case if we failed with OLS */
      break;
    }

    /* try IRLS */
    lsq_reweighted_options_set(&irls_options, options->irls_max_iter, options->outlier_thres,
      MIN_INLIERS_PROPORTION_FOR_IRLS, options->precision);
    succeed = lsq_reweighted(input_data, solution, &irls_options);
    lsq_find_residuals(input_data, solution->solution, solution->residuals);

    if (succeed) {
      break;
    }

    /* try moderate-sized RANSAC */
    lsq_ransac_options_set(&ransac_options, MAX((nm + 1) / 2, nu), (nm + nu + 1) / 2,
      options->ransac_min_samples, options->ransac_max_samples, options->outlier_thres);
    succeed = lsq_ransac(input_data, solution, &ransac_options);

    if (succeed) {
      break;
    }

    break;
  }

  /* return if we failed with finding an approximate solution */
  if (!succeed) {
    return false;
  }

  return true;
}

lsq_robust_status_t lsq_robust(const lsq_input_t *input_data, lsq_sol_t *solution,
                               const lsq_robust_options_t *options)
{
  assert(lsq_input_is_ready_for_processing(input_data));
  assert(lsq_sol_is_valid(solution));
  assert(lsq_sol_is_in_agreement_with_lsq_input(input_data, solution));
  assert(lsq_robust_options_is_valid(options));

  int nu = input_data->n_unknowns;
  int nm = input_data->n_measurements;
  double residual_abs;

  bool succeed = false;
  lsq_reweighted_options_t irls_options;
  lsq_ransac_options_t ransac_options;

  if (!lsq_robust_find_approximate_solution(input_data, solution, options)) {
    return LSQ_ROBUST_FAIL;
  }

  /* check if there are messy measurements and further processing required */
  succeed = true;
  for (int i = 0; i < nm; i++) {
    residual_abs = fabs(solution->residuals[i]);
    if ((residual_abs >= options->fine_thres)
        && (residual_abs < options->outlier_thres)) {
      succeed = false;
    }
  }

  if (succeed) { /* approximate solution is good enough */
    return LSQ_ROBUST_SUCCEED;
  }

  lsq_robust_status_t result = LSQ_ROBUST_FAIL;

  /* store approximate solution */
  double *solution_approx = (double *) malloc(nu * sizeof(double));
  vector_copy(solution->solution, solution_approx, nu);

  /* init consensus */
  lsq_input_t * input_data_consensus = lsq_input_init_consensus(input_data,
    solution->residuals, options->outlier_thres);
  int nm_consensus = input_data_consensus->n_measurements;

  if (nm_consensus <= (nu + 3)) {
    result = LSQ_ROBUST_NOISY;
    goto deallocate_and_return;
  }

  /* produce IRLS */
  lsq_reweighted_options_set(&irls_options, options->irls_max_iter, options->fine_thres,
    MIN_INLIERS_PROPORTION_FOR_IRLS, options->precision);
  succeed = lsq_reweighted(input_data_consensus, solution, &irls_options);
  lsq_find_residuals(input_data, solution->solution, solution->residuals);

  if (succeed) {
    result = LSQ_ROBUST_SUCCEED;
    goto deallocate_and_return;
  }

  /* produce moderate-sized RANSAC */
  lsq_ransac_options_set(&ransac_options, MAX((nm_consensus + 1) / 2, nu + 1), (nm_consensus + nu + 1) / 2,
    options->ransac_min_samples, options->ransac_max_samples, options->fine_thres);
  succeed = lsq_ransac(input_data, solution, &ransac_options);

  if (succeed) {
    result = LSQ_ROBUST_SUCCEED;
    goto deallocate_and_return;
  }
  else {
    /* restore the approximate solution and return with the 'noisy solution' status */
    vector_copy(solution_approx, solution->solution, nu);
    result = LSQ_ROBUST_NOISY;
    goto deallocate_and_return;
  }

deallocate_and_return:
  free(solution_approx);
  lsq_input_free(input_data_consensus);
  return result;
}
