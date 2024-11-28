#include "control_allocation.hpp"

double lookup_table(double T)  // Convert Thrust into PWM with an interpolation
{
    if(T>MAX_kgf)
    {
        T=MAX_kgf;
    }
    if(T<MIN_kgf)
    {
        T=MIN_kgf;
    }
    if (T < -EPS)
    {
        return T*T*THETA_1_1+T*THETA_1_2+THETA_1_3;
    } else if (T > EPS) 
    {
        return T*T*THETA_2_1+T*THETA_2_2+THETA_2_3;
    } else 
    {
        return T*THETA_C_1+THETA_C_2;
    }

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

    // Lookup table for the conversion into PWM signals
    // result.T5=lookup_table(result.T5);
    // result.T6=lookup_table(result.T6);
    // result.T7=lookup_table(result.T7);
    // result.T8=lookup_table(result.T8);

    return result;
}
