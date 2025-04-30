/*
 * EVA_MIMOControlCodeGen.cpp
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "EVA_MIMOControlCodeGen".
 *
 * Model version              : 1.18
 * Simulink Coder version : 24.2 (R2024b) 21-Jun-2024
 * C++ source code generated on : Fri Apr  4 14:41:26 2025
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#include "EVA_MIMOControlCodeGen.h"
#include <emmintrin.h>
#include "rtwtypes.h"

/* Model step function */
void EVA_MIMOControlCodeGen::step()
{
  __m128d tmp_4;
  real_T tmp_0[6];
  real_T tmp[5];
  real_T rtb_Add[4];
  real_T rtb_ExtractRPZ[3];
  real_T rtb_Add_0;
  real_T tmp_1;
  real_T tmp_2;
  real_T tmp_3;
  int32_T i;
  int32_T i_0;
  real_T z_e_int_sat = 100.0;  //to be passed as parameters
  real_T pitch_e_int_sat = 100.0;
  real_T roll_e_int_sat = 100.0;

  /* Saturate: '<Root>/Saturation1' incorporates:
   *  DiscreteIntegrator: '<Root>/Discrete-Time Integrator'
   */
  if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[0] > z_e_int_sat) {
    tmp_1 = z_e_int_sat;
  } else if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[0] < -z_e_int_sat)
  {
    tmp_1 = -z_e_int_sat;
  } else {
    tmp_1 = EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[0];
  }

  if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[1] > pitch_e_int_sat) {
    tmp_2 = pitch_e_int_sat;
  } else if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[1] < -pitch_e_int_sat)
  {
    tmp_2 = -pitch_e_int_sat;
  } else {
    tmp_2 = EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[1];
  }

  if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[2] > roll_e_int_sat) {
    tmp_3 = roll_e_int_sat;
  } else if (EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[2] < -roll_e_int_sat)
  {
    tmp_3 = -roll_e_int_sat;
  } else {
    tmp_3 = EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[2];
  }

  /* End of Saturate: '<Root>/Saturation1' */
  for (i = 0; i < 4; i++) {
    /* Gain: '<Root>/StateGain' incorporates:
     *  UnitDelay: '<S1>/Unit Delay'
     */
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += EVA_MIMOControlCodeGen_ConstP.StateGain_Gain[(i_0 << 2) + i] *
        EVA_MIMOControlCodeGen_DW.UnitDelay_DSTATE[i_0];
    }

    /* Sum: '<Root>/Add' incorporates:
     *  Gain: '<Root>/InegratorGain'
     *  Gain: '<Root>/StateGain'
     */
    rtb_Add_0 += (EVA_MIMOControlCodeGen_ConstP.InegratorGain_Gain[i + 4] *
                  tmp_2 + EVA_MIMOControlCodeGen_ConstP.InegratorGain_Gain[i] *
                  tmp_1) + EVA_MIMOControlCodeGen_ConstP.InegratorGain_Gain[i +
      8] * tmp_3;
    rtb_Add[i] = rtb_Add_0;

    /* Outport: '<Root>/u' incorporates:
     *  Sum: '<Root>/Add'
     */
    EVA_MIMOControlCodeGen_Y.u[i] = rtb_Add_0;
  }

  /* Gain: '<Root>/ExtractRPZ' incorporates:
   *  UnitDelay: '<S1>/Unit Delay'
   */
  for (i_0 = 0; i_0 < 3; i_0++) {
    /* Gain: '<Root>/ExtractRPZ' */
    tmp_1 = 0.0;
    for (i = 0; i < 6; i++) {
      tmp_1 += EVA_MIMOControlCodeGen_ConstP.ExtractRPZ_Gain[3 * i + i_0] *
        EVA_MIMOControlCodeGen_DW.UnitDelay_DSTATE[i];
    }

    /* Gain: '<Root>/ExtractRPZ' incorporates:
     *  UnitDelay: '<S1>/Unit Delay'
     */
    rtb_ExtractRPZ[i_0] = tmp_1;
  }

  /* End of Gain: '<Root>/ExtractRPZ' */
  for (i_0 = 0; i_0 < 5; i_0++) {
    /* Gain: '<S1>/Cd matrix' incorporates:
     *  UnitDelay: '<S1>/Unit Delay'
     */
    rtb_Add_0 = 0.0;
    for (i = 0; i < 6; i++) {
      rtb_Add_0 += EVA_MIMOControlCodeGen_ConstP.Cdmatrix_Gain[5 * i + i_0] *
        EVA_MIMOControlCodeGen_DW.UnitDelay_DSTATE[i];
    }

    /* Sum: '<S1>/Sum2' incorporates:
     *  Gain: '<S1>/Cd matrix'
     *  Gain: '<S1>/Dd matrix'
     *  Inport: '<Root>/y_measurement'
     *  Sum: '<Root>/Add'
     *  Sum: '<S1>/Sum'
     */
    tmp[i_0] = EVA_MIMOControlCodeGen_U.y_measurement[i_0] - ((((0.0 * rtb_Add[0]
      + 0.0 * rtb_Add[1]) + 0.0 * rtb_Add[2]) + 0.0 * rtb_Add[3]) + rtb_Add_0);
  }

  for (i_0 = 0; i_0 < 6; i_0++) {
    /* Gain: '<S1>/Ld matrix' */
    rtb_Add_0 = 0.0;
    for (i = 0; i < 5; i++) {
      rtb_Add_0 += EVA_MIMOControlCodeGen_ConstP.Ldmatrix_Gain[6 * i + i_0] *
        tmp[i];
    }

    /* Sum: '<S1>/Sum3' incorporates:
     *  Gain: '<S1>/Ad matrix'
     *  Gain: '<S1>/Bd matrix'
     *  Gain: '<S1>/Ld matrix'
     *  Sum: '<Root>/Add'
     *  Sum: '<S1>/Sum1'
     *  UnitDelay: '<S1>/Unit Delay'
     */
    tmp_1 = 0.0;
    for (i = 0; i < 6; i++) {
      tmp_1 += EVA_MIMOControlCodeGen_ConstP.Admatrix_Gain[6 * i + i_0] *
        EVA_MIMOControlCodeGen_DW.UnitDelay_DSTATE[i];
    }

    tmp_0[i_0] = ((((EVA_MIMOControlCodeGen_ConstP.Bdmatrix_Gain[i_0 + 6] *
                     rtb_Add[1] +
                     EVA_MIMOControlCodeGen_ConstP.Bdmatrix_Gain[i_0] * rtb_Add
                     [0]) + EVA_MIMOControlCodeGen_ConstP.Bdmatrix_Gain[i_0 + 12]
                    * rtb_Add[2]) +
                   EVA_MIMOControlCodeGen_ConstP.Bdmatrix_Gain[i_0 + 18] *
                   rtb_Add[3]) + rtb_Add_0) + tmp_1;

    /* End of Sum: '<S1>/Sum3' */
  }

  /* Update for UnitDelay: '<S1>/Unit Delay' */
  for (i_0 = 0; i_0 < 6; i_0++) {
    EVA_MIMOControlCodeGen_DW.UnitDelay_DSTATE[i_0] = tmp_0[i_0];
  }

  /* End of Update for UnitDelay: '<S1>/Unit Delay' */

  /* Inport: '<Root>/pitch_ref' incorporates:
   *  DiscreteIntegrator: '<Root>/Discrete-Time Integrator'
   *  Gain: '<Root>/ExtractRPZ'
   *  Inport: '<Root>/z_ref'
   *  Sum: '<Root>/Subtract'
   */
  tmp_4 = _mm_add_pd(_mm_mul_pd(_mm_sub_pd(_mm_set_pd
    (EVA_MIMOControlCodeGen_U.pitch_ref, EVA_MIMOControlCodeGen_U.z_ref),
    _mm_loadu_pd(&rtb_ExtractRPZ[0])), _mm_set1_pd(0.01)), _mm_loadu_pd
                     (&EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[0]));

  /* Update for DiscreteIntegrator: '<Root>/Discrete-Time Integrator' incorporates:
   *  Gain: '<Root>/ExtractRPZ'
   *  Inport: '<Root>/roll_ref'
   *  Sum: '<Root>/Subtract'
   */
  _mm_storeu_pd(&EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[0],
                tmp_4);
  EVA_MIMOControlCodeGen_DW.DiscreteTimeIntegrator_DSTATE[2] +=
    (EVA_MIMOControlCodeGen_U.roll_ref - rtb_ExtractRPZ[2]) * 0.01;
}

// Initialize or change model parameters
void set_parameters(const json& params) {
  for (int i=0; i<36; i++) EVA_MIMOControlCodeGen_ConstP.Admatrix_Gain[i] = params["Admatrix_Gain"].at(i);
  for (int i=0; i<12; i++) EVA_MIMOControlCodeGen_ConstP.InegratorGain_Gain[i] = params["InegratorGain_Gain"].at(i);
  for (int i=0; i<24; i++) EVA_MIMOControlCodeGen_ConstP.StateGain_Gain[i] = params["StateGain_Gain"].at(i);
  for (int i=0; i<24; i++) EVA_MIMOControlCodeGen_ConstP.Bdmatrix_Gain[i] = params["Bdmatrix_Gain"].at(i);
  for (int i=0; i<30; i++) EVA_MIMOControlCodeGen_ConstP.Cdmatrix_Gain[i] = params["Cdmatrix_Gain"].at(i);
  for (int i=0; i<30; i++) EVA_MIMOControlCodeGen_ConstP.Ldmatrix_Gain[i] = params["Ldmatrix_Gain"].at(i);
  for (int i=0; i<18; i++) EVA_MIMOControlCodeGen_ConstP.ExtractRPZ_Gain[i] = params["ExtractRPZ_Gain"].at(i);
}
/* Model terminate function */
void EVA_MIMOControlCodeGen::terminate()
{
  /* (no terminate code required) */
}

/* Constructor */
EVA_MIMOControlCodeGen::EVA_MIMOControlCodeGen() :
  EVA_MIMOControlCodeGen_U(),
  EVA_MIMOControlCodeGen_Y(),
  EVA_MIMOControlCodeGen_DW()
  //EVA_MIMOControlCodeGen_M()
{
  /* Currently there is no constructor body generated.*/
}

/* Destructor */
/* Currently there is no destructor body generated.*/
EVA_MIMOControlCodeGen::~EVA_MIMOControlCodeGen() = default;

