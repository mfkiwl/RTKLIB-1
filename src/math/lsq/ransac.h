/*
 *   RANdom SAmple Consensus (RANSAC) Least Squares algorithm
 */

#ifndef RTKLIB_SRC_MATH_LSQ_RANSAC_H
#define RTKLIB_SRC_MATH_LSQ_RANSAC_H

#include "lsq.h"
#include <stdbool.h>

typedef struct {

  int sample_size;
  int min_consensus_size;
  int min_samples;
  int max_samples;
  double outlier_thres;

} lsq_ransac_options_t;

int lsq_ransac(const lsq_input_t *input_data, lsq_sol_t *solution,
               const lsq_ransac_options_t *options);
bool lsq_ransac_options_is_valid(const lsq_input_t *input_data,
                                 const lsq_ransac_options_t *options);
void lsq_ransac_options_set(lsq_ransac_options_t *options, int sample_size,
  int min_consensus_size, int min_samples, int max_samples, double outlier_thres);
lsq_input_t *lsq_input_init_consensus(const lsq_input_t *lsq_input, const double residuals[],
                                      double outlier_thres);

#endif /* #ifndef RTKLIB_SRC_MATH_LSQ_RANSAC_H */
