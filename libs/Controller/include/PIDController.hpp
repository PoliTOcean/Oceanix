#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

#include <algorithm> // For std::min and std::max

class PIDController {
public:
    PIDController();

    PIDController(double kp, double ki, double kd,
                  double output_min, double output_max,
                  double integral_min, double integral_max);

    void set_parameters(double kp, double ki, double kd,
                        double output_min, double output_max,
                        double integral_min, double integral_max);

    double calculate(double setpoint, double current_value, double dt);
    double calculate_with_measured_derivative(double setpoint, double current_value, 
                                             double measured_derivative, double dt);
    void reset();

    double get_integral_term() const;

private:
    double kp_;
    double ki_;
    double kd_;

    double output_min_;
    double output_max_;

    double integral_min_;
    double integral_max_;

    double integral_term_;
    double prev_error_;
    bool first_calculation_;
};

#endif // PID_CONTROLLER_HPP
