/*
 *   GLONASS INTER-FREQUENCY PHASE BIASES (IFB) CORRECTION MODULE
 */

#ifndef GLONASS_IFB_CORRECTION_H
#define GLONASS_IFB_CORRECTION_H

#include "rtklib.h"

/* -------------------------------------------------------------------------------------------------------------- */
/* basic types */

typedef enum {

  GLO_IFB_SEARCH_MODE,
  GLO_IFB_ADJUSTMENT_MODE,
  GLO_IFB_FROZEN_MODE

} glo_IFB_mode_t;

typedef struct glo_IFB_t {

  glo_IFB_mode_t mode;
  int            adjustment_count;
  int            fix_outage;

  double         glo_dt;            /* current glo_dt value (estimated parameter) */

  double         glo_dt_initial;    /* glo_dt value at the first adjustment step */
  double         delta_glo_dt;      /* glo_dt increment at the last step */

  int            signal_to_reset;   /* external signal to reset state (0: no actions; 1: to reset) */

} glo_IFB_t;

/* -------------------------------------------------------------------------------------------------------------- */
/* API */

glo_IFB_t *glo_IFB_init();
int        glo_IFB_is_valid(const glo_IFB_t *glo_IFB);
void       glo_IFB_free(glo_IFB_t *glo_IFB);
void       glo_IFB_copy(const glo_IFB_t *glo_IFB_src, glo_IFB_t *glo_IFB_dst);

void   glo_IFB_reset(glo_IFB_t *glo_IFB);
int    glo_IFB_is_enough_sats(const rtk_t *rtk);
void   glo_IFB_process(glo_IFB_t *glo_IFB, rtk_t *rtk);    /* one step of timewise alteration (main function) */
double glo_IFB_get_glo_dt(const glo_IFB_t *glo_IFB);
double glo_IFB_get_delta_glo_dt(const glo_IFB_t *glo_IFB);
void   glo_IFB_send_signal_to_reset(glo_IFB_t *glo_IFB);

/* -------------------------------------------------------------------------------------------------------------- */

#endif /* #ifndef GLONASS_IFB_CORRECTION_H */
