#ifndef _FULLSTATEFEEDBACKCONTROL_H
#define _FULLSTATEFEEDBACKCONTROL_H
#include <vector>

class ControlSystem
{
    public:

        //Constructor
        ControlSystem(double minForce, double maxForce, double minErrrorInt, double maxErrorInt, std::vector<double> Kx, double Ki);

        
        //Returns the output of the controller given a reference signal and the actual measured state of the system
        double calculateU(double reference, double y_measurement, std::vector<double> x_measurement);

    public:

        const double minForce;  //for control input saturation purposes
        const double maxForce;  //for control input saturation purposes
        const double maxErrorIntegral; //for integrator anti-windup
        const double minErrorIntegral; //for integrator anti-windup

        // Variable for control laws's coefficients
        std::vector<double> Kx;
        double Ki;

        //Controller memory
        double ErrorIntegral;
};

#endif