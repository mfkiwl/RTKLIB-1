#ifndef RTKLIB_SRC_MATH_LSQ_LSQ_H
#define RTKLIB_SRC_MATH_LSQ_LSQ_H

#include <stdbool.h>

/* -------------------------------------------------------------------------------- */
/* basic types */

typedef struct {

  int    n_unknowns;
  int    n_measurements;
  int    max_measurements;
  double *design_matrix;
  double *measurements;

} lsq_input_t;

typedef struct {

  int    n_unknowns;
  int    n_measurements;
  int    max_measurements;
  double *solution;
  double *residuals;

} lsq_sol_t;

/* -------------------------------------------------------------------------------- */
/* API */

/* lsq_input */
bool lsq_input_is_valid(const lsq_input_t *lsq_input);
bool lsq_input_is_ready_for_processing(const lsq_input_t *lsq_input);
lsq_input_t *lsq_input_init(int n_unknowns, int max_measurements);
void lsq_input_free(lsq_input_t *lsq_input);
void lsq_input_copy(const lsq_input_t *lsq_input_src, lsq_input_t *lsq_input_dst);
void lsq_input_add_measurement(lsq_input_t *lsq_input,
                               const double design_matrix_row[], double measurement);
void lsq_input_swap_measurements(lsq_input_t *lsq_input, int i1, int i2);
void lsq_generate_random_trimmed_problem(const lsq_input_t *input_data,
                                         lsq_input_t *trimmed_data, int size_trimmed);
void lsq_find_residuals(const lsq_input_t *lsq_input, const double solution[], double residuals[]);

/* lsq_sol */
bool lsq_sol_is_valid(const lsq_sol_t *lsq_sol);
bool lsq_sol_is_in_agreement_with_lsq_input(const lsq_input_t *lsq_input, const lsq_sol_t *lsq_sol);
lsq_sol_t *lsq_sol_init(int n_unknowns, int max_measurements);
void lsq_sol_free(lsq_sol_t *lsq_sol);

#endif /* #ifndef RTKLIB_SRC_MATH_LSQ_LSQ_H */
