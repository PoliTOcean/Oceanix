#include "PIDController.hpp"
#include <cmath> // For std::isinf, std::isnan

PIDController::PIDController()
    : kp_(0.0), ki_(0.0), kd_(0.0),
      output_min_(-1.0), output_max_(1.0),
      integral_min_(-0.5), integral_max_(0.5),
      integral_term_(0.0), prev_error_(0.0), first_calculation_(true) {}

PIDController::PIDController(double kp, double ki, double kd,
                             double output_min, double output_max,
                             double integral_min, double integral_max)
    : kp_(kp), ki_(ki), kd_(kd),
      output_min_(output_min), output_max_(output_max),
      integral_min_(integral_min), integral_max_(integral_max),
      integral_term_(0.0), prev_error_(0.0), first_calculation_(true) {}

void PIDController::set_parameters(double kp, double ki, double kd,
                                   double output_min, double output_max,
                                   double integral_min, double integral_max) {
    kp_ = kp;
    ki_ = ki;
    kd_ = kd;
    output_min_ = output_min;
    output_max_ = output_max;
    integral_min_ = integral_min;
    integral_max_ = integral_max;
    reset(); // Reset state when parameters change
}

double PIDController::calculate(double setpoint, double current_value, double dt) {
    if (dt <= 0.0 || std::isinf(dt) || std::isnan(dt)) {
        // Invalid dt, return 0 or handle error appropriately
        return 0.0;
    }

    double error = setpoint - current_value;

    // Proportional term
    double p_term = kp_ * error;

    // Integral term (with anti-windup)
    integral_term_ += ki_ * error * dt;
    integral_term_ = std::max(integral_min_, std::min(integral_term_, integral_max_));

    // Derivative term
    double d_term = 0.0;
    if (!first_calculation_) {
        d_term = kd_ * (error - prev_error_) / dt;
    } else {
        first_calculation_ = false;
    }

    // Total output
    double output = p_term + integral_term_ + d_term;

    // Store error for next iteration
    prev_error_ = error;

    // Clamp output to limits
    return std::max(output_min_, std::min(output, output_max_));
}

void PIDController::reset() {
    integral_term_ = 0.0;
    prev_error_ = 0.0;
    first_calculation_ = true;
}

double PIDController::get_integral_term() const {
    return integral_term_;
}
