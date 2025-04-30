#include "PolePlacementControl.h"
#include <iostream>


// Constructor


ControlSystemZ::ControlSystemZ(double minForce, double maxForce, double minError, double weight, double buoyancy, double denFHeave2, double numFHeave1, double numFHeave2, double denCHeave2, double denCHeave3, double numCHeave2, double numCHeave3, double cZ_inf):
    
    minForce(minForce),
    maxForce(maxForce),  
    minError(minError),
    weight(weight),
    buoyancy(buoyancy),  
    denFHeave2(denFHeave2),
    numFHeave1(numFHeave1),
    numFHeave2(numFHeave2),
    denCHeave2(denCHeave2),
    denCHeave3(denCHeave3),
    numCHeave2(numCHeave2),
    numCHeave3(numCHeave3),
    cZ_inf(cZ_inf)

{

    this->oldDOF2Reference= 0.0;
    this->old2DOF2Reference= 0.0;
    this->oldReference= 0.0;
    this->old2Reference= 0.0;
    this->oldAntiWindUpSignal= 0.0;
    this->old2AntiWindUpSignal= 0.0;
    this->oldForce= 0.0;
    this->old2Force= 0.0;       
    
}

ControlSystemPITCH::ControlSystemPITCH(double minForce, double maxForce, double minError, double weight, double buoyancy, double denFPitch2, double denFPitch3, double numFPitch2, double numFPitch3, double denCPitch2, double denCPitch3, double numCPitch2, double numCPitch3, double cPITCH_inf):
    
    minForce(minForce),
    maxForce(maxForce),  
    minError(minError),
    weight(weight),
    buoyancy(buoyancy),  
    denFPitch2(denFPitch2),
    denFPitch3(denFPitch3),
    numFPitch2(numFPitch2),
    numFPitch3(numFPitch3),
    denCPitch2(denCPitch2),
    denCPitch3(denCPitch3),
    numCPitch2(numCPitch2),
    numCPitch3(numCPitch3),
    cPITCH_inf(cPITCH_inf)

   
{

    this->oldDOF2Reference= 0.0;
    this->old2DOF2Reference= 0.0;
    this->oldReference= 0.0;
    this->old2Reference= 0.0;
    this->oldAntiWindUpSignal= 0.0;
    this->old2AntiWindUpSignal= 0.0;
    this->oldForce= 0.0;
    this->old2Force= 0.0;  

}

ControlSystemROLL::ControlSystemROLL(double minForce, double maxForce, double minError, double weight, double buoyancy, double denCRoll2, double denCRoll3, double numCRoll2, double numCRoll3, double cROLL_inf):
    
    minForce(minForce),
    maxForce(maxForce),  
    minError(minError),
    weight(weight),
    buoyancy(buoyancy),  
    denCRoll2(denCRoll2),
    denCRoll3(denCRoll3),
    numCRoll2(numCRoll2),
    numCRoll3(numCRoll3),
    cROLL_inf(cROLL_inf)
   
{

    this->oldDOF2Reference= 0.0;
    this->old2DOF2Reference= 0.0;
    this->oldReference= 0.0;
    this->old2Reference= 0.0;
    this->oldAntiWindUpSignal= 0.0;
    this->old2AntiWindUpSignal= 0.0;
    this->oldForce= 0.0;
    this->old2Force= 0.0;       
    
}





// Computes the output of the Z controller
double ControlSystemZ::calculateZ(double reference, double measurement)
{
    double error = reference - measurement;
    //Check whether the error is too small
    if(error<(this->minError) && error> -(this->minError))
    {
        return 0.0;
    }
    // 2DOF DIOPHONTAINE CONTROLLER WITH ANTI WIND-UP
    // From now on all the coefficients come from the antiZ-transform of controller transfer functions
    
    //2DOF control
    double DOF2Reference = denFHeave2 * oldDOF2Reference + numFHeave1 *reference + numFHeave2 *oldReference;

    //Error signal
    double DOF2error = DOF2Reference - measurement;
    
    //1DOF controller with anti wind-up scheme
    double antiWindUpSignal = denCHeave2 *oldAntiWindUpSignal + denCHeave3*old2AntiWindUpSignal + numCHeave2 *oldForce + numCHeave3 *old2Force;
    double infSignal = DOF2error - antiWindUpSignal;
    double unSatForce = infSignal * cZ_inf; 
    

    if(unSatForce > maxForce)
    {
        unSatForce = maxForce;
    }
    else if(unSatForce < minForce)
    {
        unSatForce = minForce;
    }

    double Force = unSatForce; 
    
    // Controller memory
    old2DOF2Reference = oldDOF2Reference;
    oldDOF2Reference = DOF2Reference;
    old2Reference = oldReference;
    oldReference =  reference;
    old2AntiWindUpSignal =  oldAntiWindUpSignal;
    oldAntiWindUpSignal = antiWindUpSignal;
    old2Force = oldForce;
    oldForce = unSatForce; 
    

    return Force;
}





// Computes the output of the PITCH controller
double ControlSystemPITCH::calculatePitch(double reference, double measurement)
{
    double error = reference - measurement;
    //Check whether the error is too small
    if(error<(this->minError) && error> -(this->minError))
    {
        return 0.0;
    }
    // 2DOF DIOPHONTAINE CONTROLLER WITH ANTI WIND-UP
    // From now on all the coefficients come from the antiZ-transform of controller transfer functions
    
    //2DOF control
    double DOF2Reference = denFPitch2 * oldDOF2Reference + denFPitch3 * old2DOF2Reference + numFPitch2 *oldReference + numFPitch3 *old2Reference;
    //Error signal
    double DOF2error = DOF2Reference - measurement;

    //1DOF controller with anti wind-up scheme
    double antiWindUpSignal = denCPitch2 *oldAntiWindUpSignal + denCPitch3 *old2AntiWindUpSignal + numCPitch2 *oldForce + numCPitch3 *old2Force;
    double infSignal = DOF2error - antiWindUpSignal;
    double unSatForce = infSignal * cPITCH_inf;

    if(unSatForce > maxForce)
    {
        unSatForce = maxForce;
    }
    else if(unSatForce < minForce)
    {
        unSatForce = minForce;
    }

    double Force = unSatForce; 
    
    // Controller memory
    old2DOF2Reference = oldDOF2Reference;
    oldDOF2Reference = DOF2Reference;
    old2Reference = oldReference;
    oldReference =  reference;
    old2AntiWindUpSignal =  oldAntiWindUpSignal;
    oldAntiWindUpSignal = antiWindUpSignal;
    old2Force = oldForce;
    oldForce = unSatForce; 
    

    return Force;
}





// Computes the output of the ROLL controller
double ControlSystemROLL::calculateRoll(double reference, double measurement)
{
    double error = reference - measurement;
    
    //Check whether the error is too small
    if(error<(this->minError) && error> -(this->minError))
    {
        return 0.0;
    }

    //1DOF controller with anti wind-up scheme
    double antiWindUpSignal = denCRoll2 *oldAntiWindUpSignal + denCRoll3 *old2AntiWindUpSignal + numCRoll2 *oldForce + numCRoll3 *old2Force;
    double infSignal = error - antiWindUpSignal;
    double unSatForce = infSignal * cROLL_inf;

    if(unSatForce > maxForce)
    {
        unSatForce = maxForce;
    }
    else if(unSatForce < minForce)
    {
        unSatForce = minForce;
    }

    double Force = unSatForce - weight + buoyancy; 
    
    // Controller memory
    old2AntiWindUpSignal =  oldAntiWindUpSignal;
    oldAntiWindUpSignal = antiWindUpSignal;
    old2Force = oldForce;
    oldForce = unSatForce; 
    

    return Force;
}



