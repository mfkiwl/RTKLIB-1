/*
 *   Ordinary Least Squares (OLS) solver
 */

#ifndef RTKLIB_SRC_MATH_LSQ_OLS_H
#define RTKLIB_SRC_MATH_LSQ_OLS_H

#include "lsq.h"

void lsq_ols_standard(const lsq_input_t *input_data, lsq_sol_t *solution);

#endif /* #ifndef RTKLIB_SRC_MATH_LSQ_OLS_H */
