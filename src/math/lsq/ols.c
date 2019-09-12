#include "ols.h"
#include "lsq.h"
#include "rtklib.h"
#include "rtklib_math.h"

#define SQR(x)      ((x) * (x))

/* OLS by normal equations */

void lsq_ols_standard(const lsq_input_t *input_data, lsq_sol_t *solution)
{
  assert(lsq_input_is_ready_for_processing(input_data));
  assert(lsq_sol_is_valid(solution));
  assert(lsq_sol_is_in_agreement_with_lsq_input(input_data, solution));

  int nu = input_data->n_unknowns;
  int nm = input_data->n_measurements;

  double *variances = (double *) malloc(SQR(nu) *sizeof(double));

  /* OLS */
  lsq(input_data->design_matrix, input_data->measurements, nu, nm,
      solution->solution, variances);
  lsq_find_residuals(input_data, solution->solution, solution->residuals);

  free(variances);
}
