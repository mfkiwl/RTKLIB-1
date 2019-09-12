/*
 *   RANdom SAmple Consensus (RANSAC) Least Squares algorithm
 */

#include "ransac.h"
#include "lsq.h"
#include "ols.h"
#include "rtklib_math.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define SQR(x)      ((x) * (x))

/* init new input data with thinning the existent */
lsq_input_t *lsq_input_init_consensus(const lsq_input_t *lsq_input, const double residuals[],
                                             double outlier_thres)
{
  assert(lsq_input_is_ready_for_processing(lsq_input));
  assert(residuals != NULL);
  assert(outlier_thres > 0.0);

  int nu = lsq_input->n_unknowns;
  int nm = lsq_input->n_measurements;

  lsq_input_t *lsq_input_consensus = lsq_input_init(nu, lsq_input->max_measurements);

  /* fill input for lsq with no outliers */
  for (int i = 0; i < nm; i++) {
    if (fabs(residuals[i]) < outlier_thres) {
      lsq_input_add_measurement(lsq_input_consensus, &lsq_input->design_matrix[i * nu],
                                lsq_input->measurements[i]);
    }
  }

  assert(lsq_input_is_valid(lsq_input_consensus));

  return lsq_input_consensus;
}

bool lsq_ransac_options_is_valid(const lsq_input_t *input_data,
                                 const lsq_ransac_options_t *options)
{
  assert(lsq_input_is_ready_for_processing(input_data));

  if (options == NULL) {
    return false;
  }

  if (!IS_IN_BOUNDS(options->sample_size, input_data->n_unknowns, input_data->n_measurements)) {
    return false;
  }

  if (!IS_IN_BOUNDS(options->min_consensus_size, input_data->n_unknowns, input_data->n_measurements)) {
    return false;
  }

  if (!IS_IN_BOUNDS(options->min_samples, 1, options->max_samples)) {
    return false;
  }

  if (options->outlier_thres <= 0.0) {
    return false;
  }

  return true;
}

void lsq_ransac_options_set(lsq_ransac_options_t *options, int sample_size,
  int min_consensus_size, int min_samples, int max_samples, double outlier_thres)
{
  assert(options != NULL);

  options->sample_size = sample_size;
  options->min_consensus_size = min_consensus_size;
  options->min_samples = min_samples;
  options->max_samples = max_samples;
  options->outlier_thres = outlier_thres;
}

/* return: 0: fail, 1: succeed */
int lsq_ransac(const lsq_input_t *input_data, lsq_sol_t *solution,
               const lsq_ransac_options_t *options)
{
  assert(lsq_input_is_ready_for_processing(input_data));
  assert(lsq_sol_is_valid(solution));
  assert(lsq_sol_is_in_agreement_with_lsq_input(input_data, solution));
  assert(lsq_ransac_options_is_valid(input_data, options));

  int consensus_size, consensus_best_size = 0;
  int consensus_sqr,  consensus_best_sqr  = 0.0;
  int nu = input_data->n_unknowns;
  int nm = input_data->n_measurements;
  double *solution_sample_best = (double *) malloc(nu * sizeof(double));
  double *residuals = (double *) malloc(nm * sizeof(double));
  lsq_input_t *input_data_trimmed = lsq_input_init(nu, input_data->max_measurements);

  for (int i = 0; i < options->max_samples; i++) {
    lsq_generate_random_trimmed_problem(input_data, input_data_trimmed, options->sample_size);
    lsq_ols_standard(input_data_trimmed, solution);
    lsq_find_residuals(input_data, solution->solution, residuals);

    consensus_size = 0;
    consensus_sqr  = 0.0;
    for (int j = 0; j < nm; j++) {
      if (fabs(residuals[j]) < options->outlier_thres) {
        consensus_size++;
        consensus_sqr += SQR(residuals[j]);
      }
    }

    if ((consensus_best_size < consensus_size)
        || ((consensus_best_size == consensus_size)
          && (consensus_sqr < consensus_best_sqr) )) {
      consensus_best_size = consensus_size;
      consensus_best_sqr  = consensus_sqr;
      vector_copy(solution->solution, solution_sample_best, nu);
    }

    /* early out if we find a sufficient consensus already */
    if (((i + 1) >= options->min_samples)
        && (consensus_best_size >= options->min_consensus_size)) {
      break;
    }
  }

  lsq_find_residuals(input_data, solution_sample_best, residuals);

  /* return if we are failed to find a sufficient consensus */
  if (consensus_best_size < options->min_consensus_size) {
    free(solution_sample_best);
    free(residuals);
    lsq_input_free(input_data_trimmed);
    return 0;
  }

  /* calculate a solution after we find the best consensus */
  lsq_input_t *lsq_input_consensus = lsq_input_init_consensus(input_data, residuals,
                                                              options->outlier_thres);
  assert(lsq_input_is_ready_for_processing(lsq_input_consensus));

  lsq_ols_standard(lsq_input_consensus, solution);
  lsq_find_residuals(input_data, solution->solution, solution->residuals);

  /* redefine consensus with the new solution and calculate solution again */
  lsq_input_t *lsq_input_consensus_refined = lsq_input_init_consensus(input_data, solution->residuals,
                                                                      options->outlier_thres);
  assert(lsq_input_is_ready_for_processing(lsq_input_consensus_refined));

  if (lsq_input_consensus_refined->n_measurements < options->min_consensus_size) {
    free(solution_sample_best);
    free(residuals);
    lsq_input_free(input_data_trimmed);
    lsq_input_free(lsq_input_consensus);
    lsq_input_free(lsq_input_consensus_refined);
    return 0;
  }

  lsq_ols_standard(lsq_input_consensus_refined, solution);
  lsq_find_residuals(input_data, solution->solution, solution->residuals);

  free(solution_sample_best);
  free(residuals);
  lsq_input_free(input_data_trimmed);
  lsq_input_free(lsq_input_consensus);
  lsq_input_free(lsq_input_consensus_refined);

  return 1;
}
