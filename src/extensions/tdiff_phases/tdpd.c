/*
 *   Time-Differenced Phases Displacement (TDPD) applications
 */

#include "tdpd.h"
#include "rtklib.h"
#include "robust_lsq.h"
#include "rtklib_math.h"

#define TDPD_N_UNKNOWNS                         4
#define TDPD_MAX_TIME_SPAN                      2.0    /* [s] */
#define TDPD_SMOOTHING_MAX_EXTRAPOLATION_TIME   5.0    /* [s] */
#define TDPD_SMOOTHING_MAX_EXTRAPOLATION_DISPL  10.0   /* [m] */
#define TDPD_SMOOTHING_MAX_PNTPOS_DELAY         10.0   /* [s] */
#define TDPD_SMOOTHING_MAX_RESIDUAL             20.0   /* [m] */

static void tdpd_problem_output_reset(tdpd_problem_output_t *tdpd_output)
{
    assert(tdpd_output != NULL);

    memset(tdpd_output, 0, sizeof(*tdpd_output));
}

typedef struct {

    int n_observations;
    const obsd_t *obs_data;

} observations_epoch_t;

static void observations_epoch_set(observations_epoch_t *obs_epoch, const obsd_t *obs_data, int n_observations)
{
    assert(obs_epoch != NULL);
    assert(n_observations >= 0);
    assert(obs_data != NULL);

    obs_epoch->n_observations = n_observations;
    obs_epoch->obs_data = obs_data;
}

typedef struct {

    observations_epoch_t *obs_curr;
    observations_epoch_t *obs_prev;
    rtk_t *rtk;
    const nav_t *nav;
    double approx_position[VECTOR_3D_SIZE];

} tdpd_consecutive_data_t;

static void tdpd_consecutive_data_set(tdpd_consecutive_data_t *consecutive_data,
    observations_epoch_t *obs_curr, observations_epoch_t *obs_prev,
    rtk_t *rtk, const nav_t *nav, const double approx_position[VECTOR_3D_SIZE])
{
    assert(consecutive_data != NULL);
    assert(obs_curr != NULL);
    assert(obs_prev != NULL);
    assert(rtk_is_valid(rtk));
    assert(nav != NULL);
    assert(approx_position != NULL);

    consecutive_data->obs_curr = obs_curr;
    consecutive_data->obs_prev = obs_prev;
    consecutive_data->rtk = rtk;
    consecutive_data->nav = nav;
    vector3_copy(approx_position, consecutive_data->approx_position);
}

typedef struct {

    double tdiff_phases[MAXSAT];
    bool   is_tdiff_phase_defined[MAXSAT];
    double los[MAXSAT][VECTOR_3D_SIZE];

} tdpd_problem_input_t;

static void tdpd_problem_input_reset(tdpd_problem_input_t *tdpd_input)
{
    assert(tdpd_input != NULL);

    memset(tdpd_input, 0, sizeof(*tdpd_input));
}

typedef struct {

    double sat_pos[MAXSAT][VECTOR_3D_SIZE];
    double sat_pos_prev[MAXSAT][VECTOR_3D_SIZE];
    double los[MAXSAT][VECTOR_3D_SIZE];
    double los_prev[MAXSAT][VECTOR_3D_SIZE];

} tdpd_satellites_geometry_t;

static void tdpd_satellites_geometry_reset(tdpd_satellites_geometry_t *sat_geom)
{
    assert(sat_geom != NULL);

    memset(sat_geom, 0, sizeof(*sat_geom));
}

static bool tdpd_consecutive_data_is_valid(const tdpd_consecutive_data_t *data)
{
    if (data == NULL) {
        return false;
    }

    if (data->obs_curr == NULL) {
        return false;
    }

    if (data->obs_prev == NULL) {
        return false;
    }

    if (data->rtk == NULL) {
        return false;
    }

    if (data->nav == NULL) {
        return false;
    }

    return true;
}

static lsq_input_t *lsq_input_init_tdpd(const tdpd_problem_input_t *input, int sat_ids[MAXSAT])
{
    assert(input != NULL);
    assert(sat_ids != NULL);

    lsq_input_t *lsq_input = lsq_input_init(TDPD_N_UNKNOWNS, MAXSAT);

    int n_meas = 0;
    double *design_matrix = lsq_input->design_matrix;
    double *measurements  = lsq_input->measurements;
    double *design_matrix_row;

    memset(sat_ids, 0, MAXSAT * sizeof(int));

    for (int sat_id = 0; sat_id < MAXSAT; sat_id++) {
        if (!input->is_tdiff_phase_defined[sat_id]) {
            continue;
        }

        if (vector3_norm(&input->los[sat_id][0]) <= 0.0) {
            continue;
        }

        design_matrix_row = &design_matrix[n_meas * TDPD_N_UNKNOWNS];

        /* position-related part */
        vector3_copy(&input->los[sat_id][0], design_matrix_row);
        vector3_multiply(-1.0, design_matrix_row);

        /* clock-related part */
        design_matrix_row[VECTOR_3D_SIZE] = 1.0;

        /* measurements vector */
        measurements[n_meas] = input->tdiff_phases[sat_id];
        sat_ids[n_meas] = sat_id;
        n_meas++;
    }

    lsq_input->n_measurements = n_meas;

    return lsq_input;
}

static lsq_robust_status_t lsq_robust_tdpd(const lsq_input_t *lsq_input, const int sat_ids[MAXSAT],
    tdpd_problem_output_t *output)
{
    assert(lsq_input_is_valid(lsq_input));
    assert(sat_ids != NULL);
    assert(output != NULL);

    int nm = lsq_input->n_measurements;

    lsq_sol_t *lsq_solution = lsq_sol_init(TDPD_N_UNKNOWNS, nm);
    lsq_robust_options_t options;
    lsq_robust_options_set(&options, 25, 50, 10, 0.1, 0.02, 0.001);
    lsq_robust_status_t success_status = lsq_robust(lsq_input, lsq_solution, &options);

    vector3_copy(lsq_solution->solution, output->displacement);
    output->clock_shift = lsq_solution->solution[TDPD_N_UNKNOWNS - 1];

    int sat_id;
    for (int meas_id = 0; meas_id < nm; meas_id++) {
        sat_id = sat_ids[meas_id];
        output->residuals[sat_id] = lsq_solution->residuals[meas_id];
    }

    lsq_sol_free(lsq_solution);
    return success_status;
}

/* estimate receiver displacement and clock drift by time-differenced phases */
static lsq_robust_status_t estimate_displacement_by_tdiff_phases(const tdpd_problem_input_t *input,
    tdpd_problem_output_t *output)
{
    assert(input != NULL);
    assert(output != NULL);

    int sat_ids[MAXSAT] = {0};
    lsq_robust_status_t success_status = LSQ_ROBUST_FAIL;

    tdpd_problem_output_reset(output);

    lsq_input_t *lsq_input = lsq_input_init_tdpd(input, sat_ids);

    if (lsq_input->n_measurements < (TDPD_N_UNKNOWNS + 1)) { /* lack of measurements; fail */
        success_status = LSQ_ROBUST_FAIL;
        goto deallocate_and_return;
    }

    success_status = lsq_robust_tdpd(lsq_input, sat_ids, output);

deallocate_and_return:
    lsq_input_free(lsq_input);
    return success_status;
}

static bool verify_obs_data(const obsd_t *obsd, int rover_id, double elevation, const prcopt_t *opt)
{
    assert(obsd != NULL);
    assert(opt != NULL);
    assert(rover_id == 0 || rover_id == 1); /* rover (0) or base (1) for the actual implementation */

    /* check if no pseudorange */
    if (obsd->P[0] == 0.0) {
        return false;
    }

    /* check if low SNR */
    if (testsnr(rover_id, 0, elevation, obsd->SNR[0] * 0.25, &opt->snrmask)) {
        return false;
    }

    /* ckeck if low elevation */
    if (elevation < opt->elmin) {
        return false;
    }

    return true;
}

static bool obs_data_is_valid_tdpd(const obsd_t *obsd, int rover_id, const rtk_t *rtk,
    const double sat_pos[VECTOR_3D_SIZE])
{
    assert(obsd != NULL);
    assert(rover_id == 0 || rover_id == 1); /* rover (0) or base (1) for the actual implementation */
    assert(rtk_is_valid(rtk));
    assert(sat_pos != NULL);

    int sat_id = obsd_get_sat_id(obsd);
    double elevation = rtk->ssat[sat_id].azel[1];
    double phase = obsd->L[0];

    bool is_obsd_valid = verify_obs_data(obsd, rover_id, elevation, &rtk->opt);
    bool is_phase_defined = (phase != 0.0);
    bool is_sat_pos_defined = (vector3_norm(sat_pos) > 0.0);
    bool is_cycle_slip_detected = (obsd->LLI[0] & 1);

    bool is_obsd_valid_tdpd = is_obsd_valid && is_phase_defined
                                 && is_sat_pos_defined && (!is_cycle_slip_detected);

    return is_obsd_valid_tdpd;
}

static void find_sat_id_to_obs_data_map(const observations_epoch_t *obs_epoch, int map[MAXSAT])
{
    assert(obs_epoch != NULL);
    assert(map != NULL);

    for (int sat_id = 0; sat_id < MAXSAT; sat_id++) {
        map[sat_id] = -1;
    }

    int sat_id;
    for (int obsd_id = 0; obsd_id < obs_epoch->n_observations; obsd_id++) {
        sat_id = obsd_get_sat_id(&obs_epoch->obs_data[obsd_id]);
        map[sat_id] = obsd_id;
    }
}

static void define_satellites_geometry_tdpd(const tdpd_consecutive_data_t *consecutive_data,
    tdpd_satellites_geometry_t *sat_geom)
{
    assert(tdpd_consecutive_data_is_valid(consecutive_data));
    assert(sat_geom != NULL);

    const obsd_t *obsd      = consecutive_data->obs_curr->obs_data;
    const obsd_t *obsd_prev = consecutive_data->obs_prev->obs_data;
    int n_obsd      = consecutive_data->obs_curr->n_observations;
    int n_obsd_prev = consecutive_data->obs_prev->n_observations;
    rtk_t *rtk = consecutive_data->rtk;
    const nav_t *nav = consecutive_data->nav;

    gtime_t time_prev = obsd_prev[0].time;

    double sat_pos[2 * VECTOR_3D_SIZE * MAXSAT];
    double sat_pos_prev[2 * VECTOR_3D_SIZE * MAXSAT];

    double dts_no_use[2 * MAXSAT];
    double var_no_use[MAXSAT];
    int    svh_no_use[MAXSAT];

    int sat_id;

    tdpd_satellites_geometry_reset(sat_geom);

    /* calculate satellites positions for consequtive epochs with the same orbit parameters */
    satposs(time_prev, obsd, n_obsd, nav,
        rtk->opt.sateph, sat_pos, dts_no_use, var_no_use, svh_no_use);
    satposs(time_prev, obsd_prev, n_obsd_prev, nav,
        rtk->opt.sateph, sat_pos_prev, dts_no_use, var_no_use, svh_no_use);

    /* rearrange satellites positions arrays and find line-of-sight (los) vector */
    const double *approx_pos = consecutive_data->approx_position;
    for (int obsd_id = 0; obsd_id < n_obsd; obsd_id++) {
        sat_id = obsd_get_sat_id(&obsd[obsd_id]);

        vector3_copy(&sat_pos[2 * VECTOR_3D_SIZE * obsd_id],
                     &sat_geom->sat_pos[sat_id][0]);

        geodist(&sat_geom->sat_pos[sat_id][0],
                approx_pos, &sat_geom->los[sat_id][0]);
    }

    for (int obsd_id = 0; obsd_id < n_obsd_prev; obsd_id++) {
        sat_id = obsd_get_sat_id(&obsd_prev[obsd_id]);

        vector3_copy(&sat_pos_prev[2 * VECTOR_3D_SIZE * obsd_id],
                     &sat_geom->sat_pos_prev[sat_id][0]);
        geodist(&sat_geom->sat_pos_prev[sat_id][0],
                approx_pos, &sat_geom->los_prev[sat_id][0]);
    }
}

static void calculate_tdiff_phases(const tdpd_consecutive_data_t *consecutive_data,
    const tdpd_satellites_geometry_t *sat_geom, tdpd_problem_input_t *tdpd_input)
{
    assert(tdpd_consecutive_data_is_valid(consecutive_data));
    assert(sat_geom != NULL);
    assert(tdpd_input != NULL);

    double *tdiff_phases = tdpd_input->tdiff_phases;
    bool *is_tdiff_phase_defined = tdpd_input->is_tdiff_phase_defined;

    /* find sat_id to obsd_id map */
    int obsd_ids[MAXSAT] = {-1};
    int obsd_ids_prev[MAXSAT] = {-1};

    find_sat_id_to_obs_data_map(consecutive_data->obs_curr, obsd_ids);
    find_sat_id_to_obs_data_map(consecutive_data->obs_prev, obsd_ids_prev);

    /* calculate tdiff phases */
    bool is_obsd_valid_tdpd, is_obsd_prev_valid_tdpd;
    int obsd_id, obsd_prev_id;
    const obsd_t *obsd_curr, *obsd_prev;
    double geometry_adjustment, range_adjustment;
    rtk_t *rtk = consecutive_data->rtk;

    for (int sat_id = 0; sat_id < MAXSAT; sat_id++) {
        obsd_id = obsd_ids[sat_id];
        obsd_prev_id = obsd_ids_prev[sat_id];

        /* skip sat_id if there is no obs data for this sat */
        if ((obsd_id < 0) || (obsd_prev_id < 0)) {
            continue;
        }

        obsd_curr = &consecutive_data->obs_curr->obs_data[obsd_id];
        obsd_prev = &consecutive_data->obs_prev->obs_data[obsd_prev_id];

        is_obsd_valid_tdpd      = obs_data_is_valid_tdpd(obsd_curr, 0, rtk,
            &sat_geom->sat_pos[sat_id][0]);
        is_obsd_prev_valid_tdpd = obs_data_is_valid_tdpd(obsd_prev, 0, rtk,
            &sat_geom->sat_pos_prev[sat_id][0]);

        if (is_obsd_valid_tdpd && is_obsd_prev_valid_tdpd) {
            tdiff_phases[sat_id]  = obsd_curr->L[0] - obsd_prev->L[0];
            tdiff_phases[sat_id] *= consecutive_data->nav->lam[sat_id][0];

            geometry_adjustment =
                vector3_scalar_product(consecutive_data->approx_position, &sat_geom->los[sat_id][0])
              - vector3_scalar_product(consecutive_data->approx_position, &sat_geom->los_prev[sat_id][0]);
            range_adjustment =
                vector3_scalar_product(&sat_geom->sat_pos[sat_id][0], &sat_geom->los[sat_id][0])
              - vector3_scalar_product(&sat_geom->sat_pos_prev[sat_id][0], &sat_geom->los_prev[sat_id][0]);

            tdiff_phases[sat_id] += geometry_adjustment - range_adjustment;
            is_tdiff_phase_defined[sat_id] = true;
        }
    }
}

static void prepare_tdpd_input(const tdpd_consecutive_data_t *consecutive_data,
    tdpd_problem_input_t *tdpd_input)
{
    assert(tdpd_consecutive_data_is_valid(consecutive_data));
    assert(tdpd_input != NULL);

    tdpd_problem_input_reset(tdpd_input);

    const obsd_t *obsd      = consecutive_data->obs_curr->obs_data;
    const obsd_t *obsd_prev = consecutive_data->obs_prev->obs_data;
    int n_obsd      = consecutive_data->obs_curr->n_observations;
    int n_obsd_prev = consecutive_data->obs_prev->n_observations;

    gtime_t time_curr = obsd[0].time;
    gtime_t time_prev = obsd_prev[0].time;

    /* return if time span between epochs is too big */
    if (timediff(time_curr, time_prev) > TDPD_MAX_TIME_SPAN) {
        return;
    }

    /* return if no obs data */
    if ((n_obsd) <= 0 || (n_obsd_prev <= 0)) {
        return;
    }

    tdpd_satellites_geometry_t satellites_geometry;

    define_satellites_geometry_tdpd(consecutive_data, &satellites_geometry);
    memcpy(tdpd_input->los, satellites_geometry.los, sizeof(tdpd_input->los));

    calculate_tdiff_phases(consecutive_data, &satellites_geometry, tdpd_input);
}

/* position-domain carrier-smoothed single solution */
void pntpos_position_domain_smoothing(rtk_t *rtk, double displacement_tdpd[VECTOR_3D_SIZE],
    lsq_robust_status_t tdpd_status)
{
    assert(rtk_is_valid(rtk));
    assert(displacement_tdpd != NULL);

    position_domain_smoothing_t *pds = &rtk->position_domain_smoothing_data;
    double age          = fabs(timediff(rtk->sol.time, pds->time_start));
    double dt           = timediff(rtk->sol.time, pds->time_previous_pntpos);
    double weight;
    bool pntpos_success_status = (rtk->sol.stat == SOLQ_SINGLE);
    bool tdpd_success_status   = (tdpd_status == LSQ_ROBUST_SUCCEED);

    bool is_filter_out_of_date = fabs(dt) > TDPD_SMOOTHING_MAX_PNTPOS_DELAY;

    /* calculate velocity via known displacement */
    if (tdpd_success_status && (rtk->tt != 0.0)) {
        pds->time_previous_tdpd = rtk->sol.time;

        vector3_copy(displacement_tdpd, pds->velocity_tdpd);
        vector3_multiply(1.0 / rtk->tt, pds->velocity_tdpd);
    }

    double dt_extrapolation    = timediff(rtk->sol.time, pds->time_previous_tdpd);
    double displ_extrapolation = vector3_norm(pds->velocity_tdpd) * dt_extrapolation;
    bool is_velocity_tdpd_out_of_date =
        (dt_extrapolation > TDPD_SMOOTHING_MAX_EXTRAPOLATION_TIME)
        || (displ_extrapolation > TDPD_SMOOTHING_MAX_EXTRAPOLATION_DISPL);

    /* repair solution with allowing to use a previous velocity */
    bool is_displacement_tdpd_available = tdpd_success_status;
    if ((!tdpd_success_status) && (!is_velocity_tdpd_out_of_date)) {
        vector3_copy(pds->velocity_tdpd, displacement_tdpd);
        vector3_multiply(rtk->tt, displacement_tdpd);

        is_displacement_tdpd_available = true;
    }

    double pos_extrapolated[VECTOR_3D_SIZE];
    vector3_sum(pds->position_smoothed, displacement_tdpd, pos_extrapolated);

    /* check pntpos residual and reject if it is too big */
    double pos_residual[VECTOR_3D_SIZE] = {0.0};
    if (is_displacement_tdpd_available && pntpos_success_status && (pds->count > 0)
        && (!is_filter_out_of_date) && (age > rtk->opt.smoothing_window)) {
        vector3_diff(rtk->sol.rr, pos_extrapolated, pos_residual);

        if (vector3_norm(pos_residual) > TDPD_SMOOTHING_MAX_RESIDUAL) {
            pntpos_success_status = false;
        }
    }

    bool is_initialization   = (pds->count == 0) && pntpos_success_status;
    bool is_reinitialization = (is_filter_out_of_date || (!is_displacement_tdpd_available))
                                && pntpos_success_status;

    bool actions_needed       = (fabs(dt) != 0.0) || (pds->count == 0);
    bool actions_not_possible = ((!is_displacement_tdpd_available) && (!pntpos_success_status));

    if (actions_needed && (!actions_not_possible)) {
        if (is_initialization || is_reinitialization) {
            pds->count = 1;
            vector3_copy(rtk->sol.rr, pds->position_smoothed);
            pds->time_start    = rtk->sol.time;
            pds->time_previous_pntpos = rtk->sol.time;
        }
        else if (pntpos_success_status) {
            if ((age <= rtk->opt.smoothing_window)
                    && tdpd_success_status) {
                pds->count++;
            }

            weight = 1.0 / (pds->count);

            vector3_linear_combination(weight, rtk->sol.rr,
                1 - weight, pos_extrapolated, pds->position_smoothed);

            pds->time_previous_pntpos = rtk->sol.time;
        }
        else if (is_displacement_tdpd_available) { /* pntpos failed and TDPD displ available */
            /* if pntpos failed repair solution with TDPD displacement */
            vector3_add(pds->position_smoothed, displacement_tdpd);
            rtk->sol.stat = SOLQ_SINGLE;
        }
    }

    /* copy position and velocity to rtk->sol */
    vector3_copy(pds->position_smoothed, rtk->sol.rr);
    if (is_displacement_tdpd_available) {
        vector3_copy(pds->velocity_tdpd, rtk->sol.rr + VECTOR_3D_SIZE);
    }
}

lsq_robust_status_t estimate_displacement_by_tdiff_phases_rtkpos_wrapper(rtk_t *rtk,
  const obsd_t *obsd, int n_obsd, const obsd_t *obsd_prev, int n_obsd_prev, const nav_t *nav,
  tdpd_problem_output_t *output)
{
  assert(rtk_is_valid(rtk));
  assert(obsd != NULL);
  assert(n_obsd >= 0);
  assert(obsd_prev != NULL);
  assert(n_obsd_prev >= 0);
  assert(nav != NULL);
  assert(output != NULL);

  bool pntpos_success_status = (rtk->sol.stat != SOLQ_NONE);

  /* try to use previous position if the current is not available */
  if (!pntpos_success_status) {
      vector3_copy(rtk->sol.pos_prev, rtk->sol.rr);
  }

  observations_epoch_t obs_curr, obs_prev;
  observations_epoch_set(&obs_curr, obsd, n_obsd);
  observations_epoch_set(&obs_prev, obsd_prev, n_obsd_prev);

  tdpd_consecutive_data_t consecutive_data;
  tdpd_consecutive_data_set(&consecutive_data, &obs_curr, &obs_prev, rtk, nav, rtk->sol.rr);

  tdpd_problem_input_t  input;
  prepare_tdpd_input(&consecutive_data, &input);
  lsq_robust_status_t tdpd_status = estimate_displacement_by_tdiff_phases(&input, output);

  return tdpd_status;
}
