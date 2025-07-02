//
// File: EVA_MIMOControlCodeGen.h
//
// Code generated for Simulink model 'EVA_MIMOControlCodeGen'.
//
// Model version                  : 1.19
// Simulink Coder version         : 24.2 (R2024b) 21-Jun-2024
// C/C++ source code generated on : Wed Apr 30 13:19:57 2025
//
// Target selection: ert.tlc
// Embedded hardware selection: ARM Compatible->ARM 8
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef EVA_MIMOControlCodeGen_h_
#define EVA_MIMOControlCodeGen_h_
#include <cmath>
#include "rtwtypes.h"

// Class declaration for model EVA_MIMOControlCodeGen
class EVA_MIMOControlCodeGen final
{
  // public data and function members
 public:
  // Block signals and states (default storage) for system '<Root>'
  struct DW {
    real_T UnitDelay_DSTATE[6];        // '<S1>/Unit Delay'
    real_T DiscreteTimeIntegrator_DSTATE[3];// '<Root>/Discrete-Time Integrator' 
  };

  // Constant parameters (default storage)
  struct ConstP {
    // Expression: Ad
    //  Referenced by: '<S1>/Ad matrix'

    real_T Admatrix_Gain[36];

    // Expression: -Kd(:,7:end)
    //  Referenced by: '<Root>/InegratorGain'

    real_T IntegratorGain_Gain[12];

    // Expression: -Kd(:,1:6)
    //  Referenced by: '<Root>/StateGain'

    real_T StateGain_Gain[24];

    // Expression: Bd
    //  Referenced by: '<S1>/Bd matrix'

    real_T Bdmatrix_Gain[24];

    // Expression: Cd
    //  Referenced by: '<S1>/Cd matrix'

    real_T Cdmatrix_Gain[30];

    // Expression: Ld
    //  Referenced by: '<S1>/Ld matrix'

    real_T Ldmatrix_Gain[30];

    // Expression: [Ce_heave;Ce_pitch;Ce_roll]
    //  Referenced by: '<Root>/ExtractRPZ'

    real_T ExtractRPZ_Gain[18];
  };

  // External inputs (root inport signals with default storage)
  struct ExtU {
    real_T y_measurement[5];           // '<Root>/y_measurement'
    real_T z_ref;                      // '<Root>/z_ref'
    real_T pitch_ref;                  // '<Root>/pitch_ref'
    real_T roll_ref;                   // '<Root>/roll_ref'
  };

  // External outputs (root outports fed by signals with default storage)
  struct ExtY {
    real_T u[4];                       // '<Root>/u'
  };

  struct ObsStates {
    real_T z;
    real_T roll;
    real_T pitch;
  };

  // Copy Constructor
  EVA_MIMOControlCodeGen(EVA_MIMOControlCodeGen const&) = delete;

  // Assignment Operator
  EVA_MIMOControlCodeGen& operator= (EVA_MIMOControlCodeGen const&) & = delete;

  // Move Constructor
  EVA_MIMOControlCodeGen(EVA_MIMOControlCodeGen &&) = delete;

  // Move Assignment Operator
  EVA_MIMOControlCodeGen& operator= (EVA_MIMOControlCodeGen &&) = delete;

  // External inputs
  ExtU rtU;

  // External outputs
  ExtY rtY;

  ObsStates obs_states;

  void print_matrices();

  // model step function
  void step();

  void update_observer(const double motor_commands[4]);

  // Constructor
  EVA_MIMOControlCodeGen();

  // Destructor
  ~EVA_MIMOControlCodeGen();

  EVA_MIMOControlCodeGen::ConstP rtConstP;

  // private data and function members
  // Block states
  DW rtDW;
};

// Constant parameters (default storage)


//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'EVA_MIMOControlCodeGen'
//  '<S1>'   : 'EVA_MIMOControlCodeGen/Luenberger Observer'

#endif                                 // EVA_MIMOControlCodeGen_h_

//
// File trailer for generated code.
//
// [EOF]
//
