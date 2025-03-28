#include "motor.hpp"

Motor::Motor(MotorID id, float correction, uint16_t pwm_zero, int max_slew_rate)
    : motor_id(id), correction_coeff(correction), pwm_zero(pwm_zero), slew_rate_max(max_slew_rate), pwm_old(pwm_zero) {}

uint16_t Motor::calculate_pwm(float x){
    x = x * correction_coeff;

    pwm = (uint16_t)lookup_table(x);
    pwm += (pwm_zero - 1500);

    limit_slew_rate();
    pwm_old = pwm;
    return pwm;
}

// TODO: Add method in motors
void Motor::change_parameters(float correction, uint16_t new_pwm_zero) {
    correction_coeff = correction;
    pwm_zero = new_pwm_zero;
}

void Motor::limit_slew_rate(){
    if(pwm>pwm_zero){
        if((pwm-pwm_old)>slew_rate_max)
            pwm = pwm_old + slew_rate_max;
    } else {
        if((pwm_old-pwm)>slew_rate_max)
            pwm = pwm_old - slew_rate_max;
    }
}
