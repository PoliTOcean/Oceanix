#ifndef _POLEPLACEMENTCONTROL_H
#define _POLEPLACEMENTCONTROL_H

class ControlSystemZ
{
    public:

        //Constructor
        ControlSystemZ(double minForce, double maxForce, double minError, double weight, double buoyancy, double denFHeave2, double numFHeave1, double numFHeave2, double denCHeave2, double denCHeave3, double numCHeave2, double numCHeave3, double cZ_inf);

        
        //Returns the output of the controller given a reference signal and the actual measured state of the system
        double calculateZ(double reference, double measurement);

    public:

        const double minForce;  //for control input saturation purposes
        const double maxForce;  //for control input saturation purposes
        const double minError;  //It prevents the controller from activating due to small measurement errors
        const double weight;
        const double buoyancy;

        // Variable for control laws's coefficients
        double denFHeave2;
        double numFHeave1;
        double numFHeave2;
        double denCHeave2;
        double denCHeave3;
        double numCHeave2;
        double numCHeave3;
        double cZ_inf;

        //Controller memory
        double oldDOF2Reference;
        double old2DOF2Reference;
        double oldReference;
        double old2Reference;	
        double oldAntiWindUpSignal;
        double old2AntiWindUpSignal;
        double oldForce;
        double old2Force;
};

class ControlSystemPITCH
{
    public:

        //Constructor
        ControlSystemPITCH(double minForce, double maxForce, double minError, double weight, double buoyancy, double denFPitch2, double denFPitch3, double numFPitch2, double numFPitch3, double denCPitch2, double denCPitch3, double numCPitch2, double numCPitch3, double cPITCH_inf);

        
        //Returns the output of the controller given a reference signal and the actual measured state of the system
        double calculatePitch(double reference, double measurement);

    public:

        const double minForce;  //for control input saturation purposes
        const double maxForce;  //for control input saturation purposes
        const double minError;  //It prevents the controller from activating due to small measurement errors
        const double weight;
        const double buoyancy;
        
         // Variable for control laws's coefficients
        double denFPitch2;
        double denFPitch3;
        double numFPitch2;
        double numFPitch3;
        double denCPitch2;
        double denCPitch3;
        double numCPitch2;
        double numCPitch3;
        double cPITCH_inf;

        //Controller memory
        double oldDOF2Reference;
        double old2DOF2Reference;
        double oldReference;
        double old2Reference;	
        double oldAntiWindUpSignal;
        double old2AntiWindUpSignal;
        double oldForce;
        double old2Force;
       
};

class ControlSystemROLL
{
    public:

        //Constructor
        ControlSystemROLL(double minForce, double maxForce, double minError, double weight, double buoyancy, double denCRoll2, double denCRoll3, double numCRoll2, double numCRoll3, double cROLL_inf);

        
        //Returns the output of the controller given a reference signal and the actual measured state of the system
        double calculateRoll(double reference, double measurement);

    public:

        const double minForce;  //for control input saturation purposes
        const double maxForce;  //for control input saturation purposes
        const double minError;  //It prevents the controller from activating due to small measurement errors
        const double weight;
        const double buoyancy;
        
        // Variable for control laws's coefficients
        double denCRoll2;
        double denCRoll3;
        double numCRoll2;
        double numCRoll3;
        double cROLL_inf;

        //Controller memory
        double oldDOF2Reference;
        double old2DOF2Reference;
        double oldReference;
        double old2Reference;	
        double oldAntiWindUpSignal;
        double old2AntiWindUpSignal;
        double oldForce;
        double old2Force;
        
};

#endif