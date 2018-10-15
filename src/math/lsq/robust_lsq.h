/*
 *   Robust Least Squares algorithm combining OLS, IRLS and RANSAC techniques
 */

#ifndef RTKLIB_SRC_MATH_LSQ_ROBUST_H
#define RTKLIB_SRC_MATH_LSQ_ROBUST_H

#include "lsq.h"
#include <stdbool.h>

typedef struct {

  int ransac_min_samples;
  int ransac_max_samples;
  int irls_max_iter;
  double outlier_thres;
  double fine_thres;
  double precision;

} lsq_robust_options_t;

typedef enum {

  LSQ_ROBUST_FAIL,
  LSQ_ROBUST_SUCCEED,
  LSQ_ROBUST_NOISY

} lsq_robust_status_t;

lsq_robust_status_t lsq_robust(const lsq_input_t *input_data, lsq_sol_t *solution,
                               const lsq_robust_options_t *options);
bool lsq_robust_options_is_valid(const lsq_robust_options_t *options);
void lsq_robust_options_set(lsq_robust_options_t *options, int ransac_min_samples,
  int ransac_max_samples, int irls_max_iter, double outlier_thres, double fine_thres, double precision);

#endif /* #ifndef RTKLIB_SRC_MATH_LSQ_ROBUST_H */
