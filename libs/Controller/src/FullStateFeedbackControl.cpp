#include <iostream>
#include "FullStateFeedbackControl.hpp"
#include <vector>

// Constructor
ControlSystem::ControlSystem(double minForce, double maxForce, double minErrrorInt, double maxErrorInt, std::vector<double> Kx, double Ki):


    minForce(minForce),
    maxForce(maxForce),
    maxErrorIntegral(maxErrorInt), //140
    minErrorIntegral(minErrrorInt), //-120
    Kx(Kx),
    Ki(Ki)
{
    this->ErrorIntegral= 0.0;
}

// Computes the output of the controller
double ControlSystem::calculateU(double reference, double y_measurement, std::vector<double> x_measurement)
{
    double error = reference - y_measurement; // error

    ErrorIntegral += error; // integrate error

    double ErrorIntegral_Ki = Ki * ErrorIntegral;

    // Saturate error
    if(ErrorIntegral_Ki > maxErrorIntegral)
    {
        ErrorIntegral_Ki = maxErrorIntegral;
    }
    else if(ErrorIntegral_Ki < minErrorIntegral)
    {
        ErrorIntegral_Ki = minErrorIntegral;
    }

    // Calculate force
    double unSatForce = -Kx[0] * x_measurement[0] -Kx[1] * x_measurement[1] - ErrorIntegral_Ki;

    // Saturate force
    if(unSatForce > maxForce)
    {
        unSatForce = maxForce;
    }
    else if(unSatForce < minForce)
    {
        unSatForce = minForce;
    }

    double Force = unSatForce; 
    
    return Force;
}

void ControlSystem::update_paramters(double minForce, double maxForce, double minErrrorInt, double maxErrorInt, std::vector<double> Kx, double Ki){
    this->minForce = minForce;
    this->maxForce = maxForce;
    this->maxErrorIntegral = maxErrorInt;
    this->minErrorIntegral = minErrrorInt;
    this->Kx = Kx;
    this->Ki = Ki;
}