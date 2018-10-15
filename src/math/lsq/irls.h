/*
 *   Iteratively Reveighted Least Squares (IRLS) solver
 */

#ifndef RTKLIB_SRC_MATH_LSQ_IRLS_H
#define RTKLIB_SRC_MATH_LSQ_IRLS_H

#include "lsq.h"

typedef struct {

  int max_iter;
  double downweight_thres;
  double min_inliers_proportion;
  double precision;

} lsq_reweighted_options_t;

int lsq_reweighted(const lsq_input_t *input_data, lsq_sol_t *solution,
                   const lsq_reweighted_options_t *options);
bool lsq_reweighted_options_is_valid(const lsq_reweighted_options_t *options);
void lsq_reweighted_options_set(lsq_reweighted_options_t *options, int max_iter,
  double downweight_thres, double min_inliers_proportion, double precision);

#endif /* #ifndef RTKLIB_SRC_MATH_LSQ_IRLS_H */
