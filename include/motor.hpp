#ifndef MOTOR_H
#define MOTOR_H

#include <iostream>
#include "control_allocation.hpp"
#include "motorID.hpp"
#include <stdint.h>

class Motor {
private:
    MotorID motor_id;
    uint16_t pwm;                ///< current pwm value
    uint16_t pwm_old;
    uint16_t pwm_zero;           ///< pwm when motor is still
    float correction_coeff; ///< correction of the thrust for pwm calculation (-1 for reverse)
    int slew_rate_max;      ///< max pwm change between 2 calculations

public:
    Motor(MotorID id, float correction, uint16_t pwm_zero, int max_slew_rate);

    /**
     * @brief Given the thrust calcutate the corresponding pwm of the motor
     * 
     * @param x the thrust of the motor in kgf
     * @return int pwm value
     */
    uint16_t calculate_pwm(float x);

    /**
     * @brief Change correction coefficient and pwm_zero of the motor. 
     * @param correction The new correction coefficient.
     * @param new_pwm_zero The new pwm_zero value.
     */
    void change_parameters(float correction, uint16_t new_pwm_zero);

private:
    /**
     * @brief limit the change rate in pwm iteration to iteration
     */
    void limit_slew_rate();
};

#endif // MOTOR_H
