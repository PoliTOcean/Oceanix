#include "control_allocation.hpp"

// Interpolation Algorithm
float linearInterpolation(float x, float x0, float x1, float y0, float y1) {
    if (x1==x0)
        return y0;
    return y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
}

// Function of Interpolation
uint16_t lookup_table (float Force_12V_Kgf)
{
    // i (x_interpolate) e interpolati (y_interpolated) 
    float x_interpolate = 0;
    float y_interpolated = 0;

    // Saturation of Kgf's values
        // Force_12V_Kgf>Sat_max
    if (Force_12V_Kgf > Array_Force_12V_Kgf[MAX_ROWS-1]){
        x_interpolate=Array_Force_12V_Kgf[MAX_ROWS-1];
        y_interpolated=Array_PWM_Relative_12V_Kgf[MAX_ROWS-1];
    } 
        //Force_12V_Kgf<Sat_min
    else if(Force_12V_Kgf < Array_Force_12V_Kgf[0]){    
        x_interpolate=Array_Force_12V_Kgf[0];
        y_interpolated=Array_PWM_Relative_12V_Kgf[0];
    }
        // Normal Case
    else{
        x_interpolate=Force_12V_Kgf;
        // Find the interaval of values and apply interpolation algorithm
        for (int i = 0; i < MAX_ROWS-1; i++) {
            if (x_interpolate >= Array_Force_12V_Kgf[i] && x_interpolate <= Array_Force_12V_Kgf[i + 1]) {
                y_interpolated = linearInterpolation(x_interpolate, Array_Force_12V_Kgf[i], Array_Force_12V_Kgf[i + 1], Array_PWM_Relative_12V_Kgf[i], Array_PWM_Relative_12V_Kgf[i + 1]);
                break;
            }
        }
    }
    return y_interpolated;
}


OutputValues compute_thrust(double Fz, double Fr, double Fp)  // Compute the PWM signal from Forces that comes from the Controller 
{
    OutputValues result;

    // Computation of thrust forces
    result.T5 = a51 * Fz + a52 * Fr + a53 * Fp;
    result.T6 = a61 * Fz + a62 * Fr + a63 * Fp;
    result.T7 = a71 * Fz + a72 * Fr + a73 * Fp;
    result.T8 = a81 * Fz + a82 * Fr + a83 * Fp;

    // Conversion into Kgf
    result.T5=result.T5/9.8;
    result.T6=result.T6/9.8;
    result.T7=result.T7/9.8;
    result.T8=result.T8/9.8;

    return result;
}
