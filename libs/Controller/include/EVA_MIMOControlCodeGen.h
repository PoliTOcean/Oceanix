/*
 * EVA_MIMOControlCodeGen.h
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

#ifndef EVA_MIMOControlCodeGen_h_
#define EVA_MIMOControlCodeGen_h_
#include <cmath>
#include <json.hpp>
#include "rtwtypes.h"
//#include "rtw_continuous.h"
//#include "rtw_solver.h"
//include "EVA_MIMOControlCodeGen_types.h"

using json = nlohmann::json;

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

//typedef struct tag_RTM_EVA_MIMOControlCodeGe_T RT_MODEL_EVA_MIMOControlCodeG_T;

/* Block states (default storage) for system '<Root>' */
struct DW_EVA_MIMOControlCodeGen_T {
  real_T UnitDelay_DSTATE[6];          /* '<S1>/Unit Delay' */
  real_T DiscreteTimeIntegrator_DSTATE[3];/* '<Root>/Discrete-Time Integrator' */
};

/* Constant parameters (default storage) */
struct ConstP_EVA_MIMOControlCodeGen_T {
  /* Expression: Ad
   * Referenced by: '<S1>/Ad matrix'
   */
  real_T Admatrix_Gain[36];

  /* Expression: -Kd(:,7:end)
   * Referenced by: '<Root>/InegratorGain'
   */
  real_T InegratorGain_Gain[12];

  /* Expression: -Kd(:,1:6)
   * Referenced by: '<Root>/StateGain'
   */
  real_T StateGain_Gain[24];

  /* Expression: Bd
   * Referenced by: '<S1>/Bd matrix'
   */
  real_T Bdmatrix_Gain[24];

  /* Expression: Cd
   * Referenced by: '<S1>/Cd matrix'
   */
  real_T Cdmatrix_Gain[30];

  /* Expression: Ld
   * Referenced by: '<S1>/Ld matrix'
   */
  real_T Ldmatrix_Gain[30];

  /* Expression: [Ce_heave;Ce_pitch;Ce_roll]
   * Referenced by: '<Root>/ExtractRPZ'
   */
  real_T ExtractRPZ_Gain[18];
};

/* External inputs (root inport signals with default storage) */
struct ExtU_EVA_MIMOControlCodeGen_T {
  real_T y_measurement[5];             /* '<Root>/y_measurement' */
  real_T z_ref;                        /* '<Root>/z_ref' */
  real_T pitch_ref;                    /* '<Root>/pitch_ref' */
  real_T roll_ref;                     /* '<Root>/roll_ref' */
};

/* External outputs (root outports fed by signals with default storage) */
struct ExtY_EVA_MIMOControlCodeGen_T {
  real_T u[4];                         /* '<Root>/u' */
};



/* Constant parameters (default storage) */
//extern const ConstP_EVA_MIMOControlCodeGen_T EVA_MIMOControlCodeGen_ConstP;
ConstP_EVA_MIMOControlCodeGen_T EVA_MIMOControlCodeGen_ConstP;

/* Class declaration for model EVA_MIMOControlCodeGen */
class EVA_MIMOControlCodeGen final
{
  /* public data and function members */
 public:
  /* Copy Constructor */
  EVA_MIMOControlCodeGen(EVA_MIMOControlCodeGen const&) = delete;

  /* Assignment Operator */
  EVA_MIMOControlCodeGen& operator= (EVA_MIMOControlCodeGen const&) & = delete;

  /* Move Constructor */
  EVA_MIMOControlCodeGen(EVA_MIMOControlCodeGen &&) = delete;

  /* Move Assignment Operator */
  EVA_MIMOControlCodeGen& operator= (EVA_MIMOControlCodeGen &&) = delete;

  /* External inputs */
  ExtU_EVA_MIMOControlCodeGen_T EVA_MIMOControlCodeGen_U;

  /* External outputs */
  ExtY_EVA_MIMOControlCodeGen_T EVA_MIMOControlCodeGen_Y;

  /* Initial conditions function */
  static void set_parameters(const json& params);

  /* model step function */
  void step();

  /* model terminate function */
  static void terminate();

  /* Constructor */
  EVA_MIMOControlCodeGen();

  /* Destructor */
  ~EVA_MIMOControlCodeGen();

  /* private data and function members */
 private:
  /* Block states */
  DW_EVA_MIMOControlCodeGen_T EVA_MIMOControlCodeGen_DW;
};

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'EVA_MIMOControlCodeGen'
 * '<S1>'   : 'EVA_MIMOControlCodeGen/Luenberger Observer'
 */
#endif                                 /* EVA_MIMOControlCodeGen_h_ */
