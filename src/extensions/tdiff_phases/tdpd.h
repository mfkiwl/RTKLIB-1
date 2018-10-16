/*
 *   Time-Differenced Phases Displacement (TDPD) applications
 */

#ifndef TDPD_H
#define TDPD_H

#include "rtklib.h"
#include "robust_lsq.h"

typedef struct {

    double displacement[VECTOR_3D_SIZE];
    double clock_shift;
    double residuals[MAXSAT];

} tdpd_problem_output_t;

lsq_robust_status_t estimate_displacement_by_tdiff_phases_rtkpos_wrapper(rtk_t *rtk,
  const obsd_t *obsd, int n_obsd, const obsd_t *obsd_prev, int n_obsd_prev, const nav_t *nav,
  tdpd_problem_output_t *output);

void pntpos_position_domain_smoothing(rtk_t *rtk, double displacement_tdpd[VECTOR_3D_SIZE],
  lsq_robust_status_t tdpd_status);

#endif /* #ifndef TDPD_H */
