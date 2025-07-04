//
// File: EVA_MIMOControlCodeGen.cpp
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
#include "EVA_MIMOControlCodeGen.h"
#include "rtwtypes.h"
#include <iostream>

// Model step function
void EVA_MIMOControlCodeGen::step()
{
  real_T rtb_Ldmatrix[6];
  real_T tmp[5];
  real_T rtb_Add[4];
  real_T rtb_ExtractRPZ[3];
  real_T rtb_Add_0;
  real_T tmp_0;
  real_T tmp_1;
  real_T tmp_2;
  int32_T i;
  int32_T i_0;

  // Saturate: '<Root>/Saturation1' incorporates:
  //   DiscreteIntegrator: '<Root>/Discrete-Time Integrator' (moved to the end of step)
  tmp_0 = rtDW.DiscreteTimeIntegrator_DSTATE[0];
  tmp_1 = rtDW.DiscreteTimeIntegrator_DSTATE[1];
  tmp_2 = rtDW.DiscreteTimeIntegrator_DSTATE[2];

  // End of Saturate: '<Root>/Saturation1'
  for (i = 0; i < 4; i++) {
    // Gain: '<Root>/StateGain' incorporates:
    //   UnitDelay: '<S1>/Unit Delay'

    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += rtConstP.StateGain_Gain[(i_0 << 2) + i] *
        rtDW.UnitDelay_DSTATE[i_0];
    }

    // Sum: '<Root>/Add' incorporates:
    //   Gain: '<Root>/InegratorGain'
    //   Gain: '<Root>/StateGain'

    rtb_Add[i] = ((rtConstP.IntegratorGain_Gain[i + 4] * tmp_1 +
                   rtConstP.IntegratorGain_Gain[i] * tmp_0) +
                  rtConstP.IntegratorGain_Gain[i + 8] * tmp_2) + rtb_Add_0;
  }

  // Gain: '<S1>/Dd matrix' incorporates:
  //   Sum: '<Root>/Add'

  rtb_Add_0 = rtb_Add[0];
  tmp_0 = rtb_Add[1];
  tmp_1 = rtb_Add[2];
  tmp_2 = rtb_Add[3];
  for (i = 0; i < 5; i++) {
    rtb_Ldmatrix[i] = ((0.0 * rtb_Add_0 + 0.0 * tmp_0) + 0.0 * tmp_1) + 0.0 *
      tmp_2;
  }

  // End of Gain: '<S1>/Dd matrix'

  // Outport: '<Root>/u' incorporates:
  //   Sum: '<Root>/Add'

  rtY.u[0] = rtb_Add[0];
  rtY.u[1] = rtb_Add[1];
  rtY.u[2] = rtb_Add[2];
  rtY.u[3] = rtb_Add[3];

  // Gain: '<Root>/ExtractRPZ' incorporates:
  //   UnitDelay: '<S1>/Unit Delay'

  for (i = 0; i < 3; i++) {
    // Gain: '<Root>/ExtractRPZ'
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += rtConstP.ExtractRPZ_Gain[3 * i_0 + i] *
        rtDW.UnitDelay_DSTATE[i_0];
    }

    // Gain: '<Root>/ExtractRPZ' incorporates:
    //   UnitDelay: '<S1>/Unit Delay'

    rtb_ExtractRPZ[i] = rtb_Add_0;
  }
  obs_states.z = rtb_ExtractRPZ[0];
  obs_states.pitch = rtb_ExtractRPZ[1];
  obs_states.roll = rtb_ExtractRPZ[2];

  // End of Gain: '<Root>/ExtractRPZ'

  // Sum: '<S1>/Sum2' incorporates:
  //   Gain: '<S1>/Cd matrix'
  //   Inport: '<Root>/y_measurement'
  //   Sum: '<S1>/Sum'
  //   UnitDelay: '<S1>/Unit Delay'

  for (i = 0; i < 5; i++) {
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += rtConstP.Cdmatrix_Gain[5 * i_0 + i] *
        rtDW.UnitDelay_DSTATE[i_0];
    }

    tmp[i] = rtU.y_measurement[i] - (rtb_Ldmatrix[i] + rtb_Add_0);
  }

  // End of Sum: '<S1>/Sum2'
  for (i = 0; i < 6; i++) {
    // Gain: '<S1>/Ld matrix'
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 5; i_0++) {
      rtb_Add_0 += rtConstP.Ldmatrix_Gain[6 * i_0 + i] * tmp[i_0];
    }

    // Sum: '<S1>/Sum3' incorporates:
    //   Gain: '<S1>/Ad matrix'
    //   Gain: '<S1>/Bd matrix'
    //   Gain: '<S1>/Ld matrix'
    //   Sum: '<Root>/Add'
    //   Sum: '<S1>/Sum1'
    //   UnitDelay: '<S1>/Unit Delay'

    tmp_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      tmp_0 += rtConstP.Admatrix_Gain[6 * i_0 + i] * rtDW.UnitDelay_DSTATE[i_0];
    }

    rtb_Ldmatrix[i] = ((((rtConstP.Bdmatrix_Gain[i + 6] * rtb_Add[1] +
                          rtConstP.Bdmatrix_Gain[i] * rtb_Add[0]) +
                         rtConstP.Bdmatrix_Gain[i + 12] * rtb_Add[2]) +
                        rtConstP.Bdmatrix_Gain[i + 18] * rtb_Add[3]) + rtb_Add_0)
      + tmp_0;

    // End of Sum: '<S1>/Sum3'
  }

  // Update for UnitDelay: '<S1>/Unit Delay'
  for (i = 0; i < 6; i++) {
    rtDW.UnitDelay_DSTATE[i] = rtb_Ldmatrix[i];
  }

  // End of Update for UnitDelay: '<S1>/Unit Delay'

  // Update for DiscreteIntegrator: '<Root>/Discrete-Time Integrator' incorporates:
  //   Gain: '<Root>/ExtractRPZ'
  //   Inport: '<Root>/pitch_ref'
  //   Inport: '<Root>/roll_ref'
  //   Inport: '<Root>/z_ref'
  //   Sum: '<Root>/Subtract'

  const double MAX_ERROR = 1000.0; // Maximum allowed error in one step
  
  double z_error = rtU.z_ref - rtb_ExtractRPZ[0];
  z_error = (z_error > MAX_ERROR) ? MAX_ERROR : (z_error < -MAX_ERROR ? -MAX_ERROR : z_error);
  rtDW.DiscreteTimeIntegrator_DSTATE[0] += z_error * 0.01;

  double pitch_error = rtU.pitch_ref - rtb_ExtractRPZ[1];
  pitch_error = (pitch_error > MAX_ERROR) ? MAX_ERROR : (pitch_error < -MAX_ERROR ? -MAX_ERROR : pitch_error);
  rtDW.DiscreteTimeIntegrator_DSTATE[1] += pitch_error * 0.01;

  double roll_error = rtU.roll_ref - rtb_ExtractRPZ[2];
  roll_error = (roll_error > MAX_ERROR) ? MAX_ERROR : (roll_error < -MAX_ERROR ? -MAX_ERROR : roll_error);
  rtDW.DiscreteTimeIntegrator_DSTATE[2] += roll_error * 0.01;

  // Saturate integrator output
  if (rtDW.DiscreteTimeIntegrator_DSTATE[0] > 100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[0] = 100.0;
  else if (rtDW.DiscreteTimeIntegrator_DSTATE[0] < -100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[0] = -100.0;

  if (rtDW.DiscreteTimeIntegrator_DSTATE[1] > 100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[1] = 100.0;
  else if (rtDW.DiscreteTimeIntegrator_DSTATE[1] < -100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[1] = -100.0;

  if (rtDW.DiscreteTimeIntegrator_DSTATE[2] > 100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[2] = 100.0;
  else if (rtDW.DiscreteTimeIntegrator_DSTATE[2] < -100.0)
    rtDW.DiscreteTimeIntegrator_DSTATE[2] = -100.0;
}

void EVA_MIMOControlCodeGen::update_observer(const double motor_commands[4])
{
  real_T rtb_Ldmatrix[6];
  real_T tmp[5];
  real_T rtb_Add_0;
  real_T tmp_0;
  int32_T i;
  int32_T i_0;

  // Observer update - can be called even when controller is not active
  
  // Gain: '<S1>/Dd matrix' incorporates the motor commands
  for (i = 0; i < 5; i++) {
    rtb_Ldmatrix[i] = ((0.0 * motor_commands[0] + 0.0 * motor_commands[1]) + 
                       0.0 * motor_commands[2]) + 0.0 * motor_commands[3];
  }

  // Observer update process - this is extracted from the step() function
  // Sum: '<S1>/Sum2'
  for (i = 0; i < 5; i++) {
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += rtConstP.Cdmatrix_Gain[5 * i_0 + i] * rtDW.UnitDelay_DSTATE[i_0];
    }

    tmp[i] = rtU.y_measurement[i] - (rtb_Ldmatrix[i] + rtb_Add_0);
  }

  for (i = 0; i < 6; i++) {
    // Gain: '<S1>/Ld matrix'
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 5; i_0++) {
      rtb_Add_0 += rtConstP.Ldmatrix_Gain[6 * i_0 + i] * tmp[i_0];
    }

    // State update
    tmp_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      tmp_0 += rtConstP.Admatrix_Gain[6 * i_0 + i] * rtDW.UnitDelay_DSTATE[i_0];
    }

    rtb_Ldmatrix[i] = ((((rtConstP.Bdmatrix_Gain[i + 6] * motor_commands[1] +
                          rtConstP.Bdmatrix_Gain[i] * motor_commands[0]) +
                         rtConstP.Bdmatrix_Gain[i + 12] * motor_commands[2]) +
                        rtConstP.Bdmatrix_Gain[i + 18] * motor_commands[3]) + rtb_Add_0) + tmp_0;
  }

  // Update observer states
  for (i = 0; i < 6; i++) {
    rtDW.UnitDelay_DSTATE[i] = rtb_Ldmatrix[i];
  }

  // Update observed outputs for use elsewhere in the system
  for (i = 0; i < 3; i++) {
    rtb_Add_0 = 0.0;
    for (i_0 = 0; i_0 < 6; i_0++) {
      rtb_Add_0 += rtConstP.ExtractRPZ_Gain[3 * i_0 + i] * rtDW.UnitDelay_DSTATE[i_0];
    }
    
    // Store the extracted RPZ values
    if (i == 0) obs_states.z = rtb_Add_0;
    else if (i == 1) obs_states.pitch = rtb_Add_0;
    else obs_states.roll = rtb_Add_0;
  }
}


// Model initialize function
void EVA_MIMOControlCodeGen::print_matrices()
{
  // Print all stored matrices for debugging
  std::cout << "=== EVA_MIMOControlCodeGen Matrices ===" << std::endl;
  
  // Print Admatrix_Gain (6x6 matrix)
  std::cout << "Admatrix_Gain (6x6):" << std::endl;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 6; j++) {
      std::cout << rtConstP.Admatrix_Gain[j*6 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print Bdmatrix_Gain (6x4 matrix)
  std::cout << "\nBdmatrix_Gain (6x4):" << std::endl;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << rtConstP.Bdmatrix_Gain[j*6 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print Cdmatrix_Gain (5x6 matrix)
  std::cout << "\nCdmatrix_Gain (5x6):" << std::endl;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 6; j++) {
      std::cout << rtConstP.Cdmatrix_Gain[j*5 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print Ldmatrix_Gain (6x5 matrix)
  std::cout << "\nLdmatrix_Gain (6x5):" << std::endl;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 5; j++) {
      std::cout << rtConstP.Ldmatrix_Gain[j*6 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print StateGain_Gain (4x6 matrix)
  std::cout << "\nStateGain_Gain (4x6):" << std::endl;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      std::cout << rtConstP.StateGain_Gain[j*6 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print IntegratorGain_Gain (4x3 matrix)
  std::cout << "\nIntegratorGain_Gain (4x3):" << std::endl;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << rtConstP.IntegratorGain_Gain[j*4 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  // Print ExtractRPZ_Gain (3x6 matrix)
  std::cout << "\nExtractRPZ_Gain (3x6):" << std::endl;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 6; j++) {
      std::cout << rtConstP.ExtractRPZ_Gain[j*3 + i] << "\t";
    }
    std::cout << std::endl;
  }
  
  std::cout << "========================================" << std::endl;

  // (no initialization code required)
}

// Constructor
EVA_MIMOControlCodeGen::EVA_MIMOControlCodeGen():
  rtU(),
  rtY(),
  rtDW()
{
  // Initialize integrator states to zero
  rtDW.DiscreteTimeIntegrator_DSTATE[0] = 0.0;
  rtDW.DiscreteTimeIntegrator_DSTATE[1] = 0.0;
  rtDW.DiscreteTimeIntegrator_DSTATE[2] = 0.0;
  
  // Initialize all state variables
  for (int i = 0; i < 6; i++) {
    rtDW.UnitDelay_DSTATE[i] = 0.0;
  }
}

// Destructor
// Currently there is no destructor body generated.
EVA_MIMOControlCodeGen::~EVA_MIMOControlCodeGen() = default;

//
// File trailer for generated code.
//
// [EOF]
//
