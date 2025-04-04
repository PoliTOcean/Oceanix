#ifndef MOTOR_H
#define MOTOR_H

#include <iostream>
#include "control_allocation.hpp"
#include "motorID.hpp"
#include <stdint.h>
#include <cmath>

class Motor {
private:
    MotorID motor_id;
    uint16_t pwm_zero;           ///< pwm when motor is still
    float thrust_old;
    float correction_coeff; ///< correction of the thrust for pwm calculation (-1 for reverse)
    float slew_rate_max;      ///< max thrust change between 2 calculations
    float thrust_max;       ///< max thrust of the motor

public:
    Motor(MotorID id, float correction, uint16_t pwm_zero, float max_slew_rate, float thrust_max);
    
    /**
     * @brief Given the thrust limit to the max value, limit also slewrate
     * 
     * @param thrust the thrust of the motor in kgf
     * @return thrust limited thrust value
     */
    float limit_thrust(float thrust);

    /**
     * @brief Given the thrust calcutate the corresponding pwm of the motor
     * 
     * @param thrust the thrust of the motor in kgf
     * @return int pwm value
     */
    uint16_t calculate_pwm(float thrust);

    /**
     * @brief Change correction coefficient and pwm_zero of the motor. 
     * @param correction The new correction coefficient.
     * @param new_pwm_zero The new pwm_zero value.
     */
    void change_parameters(float correction, uint16_t new_pwm_zero);
    
    /**
     * @brief Change the thrust max of the motor
     * 
     * @param new_thrust_max The new thrust max value.
     */
    void set_thrust_max(float new_thrust_max);

private:
    /**
     * @brief limit the change rate in thrust iteration to iteration
     * @param thrust the thrust to limit
     * @return the limited thrust
     */
    float limit_slew_rate(float thrust);
};

#endif // MOTOR_H
