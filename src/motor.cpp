#include "motor.hpp"

Motor::Motor(MotorID id, float correction, uint16_t pwm_zero, float max_slew_rate, float thrust_max)
    : motor_id(id), correction_coeff(correction), pwm_zero(pwm_zero), slew_rate_max(max_slew_rate), thrust_old(0), thrust_max(thrust_max) {}

float Motor::limit_thrust(float thrust){
    if(fabsf(thrust)>thrust_max){
        int sign = fabsf(thrust)/thrust;
        thrust = thrust_max * sign;
        //logger.log(logWARNING, "motor thrust limit surpassed, limited to the max");
    }
    thrust = limit_slew_rate(thrust);
    return thrust;
}

uint16_t Motor::calculate_pwm(float thrust){
    thrust = thrust * correction_coeff;

    uint16_t pwm = (uint16_t)lookup_table(thrust);
    pwm += (pwm_zero - 1500);
    return pwm;
}

// TODO: Add method in motors
void Motor::change_parameters(float correction, uint16_t new_pwm_zero) {
    correction_coeff = correction;
    pwm_zero = new_pwm_zero;
}

void Motor::set_thrust_max(float new_thrust_max) {
    thrust_max = new_thrust_max;
}

float Motor::limit_slew_rate(float thrust){
    float delta = thrust - thrust_old;

    // Apply slew rate limiting only if the change exceeds the allowed rate
    if (delta > slew_rate_max) {
        thrust = thrust_old + slew_rate_max;
    } 
    else if (delta < -slew_rate_max) {
        thrust = thrust_old - slew_rate_max;
    }

    // Update thrust_old for the next call
    thrust_old = thrust;

    return thrust;
}
