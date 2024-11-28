#ifndef _CONTROL_ALLOCATION_H
#define _CONTROL_ALLOCATION_H

// TCM (Thrust Configuration Matrix) parameters of interest
// Indices represent the row and column indexes of the TCM
const double a51 = 0.2500;
const double a61 = 0.2500;
const double a71 = 0.2500;
const double a81 = 0.2500;
const double a52 = 0.9568;
const double a72 = 0.9568;
const double a73 = 0.9568;
const double a83 = 0.9568;
const double a53 = -0.9568;
const double a62 = -0.9568;
const double a63 = -0.9568;
const double a82 = -0.9568;

// Lookup table interpolation coefficients
const double THETA_1_1 = 0.0158 * 1000;
const double THETA_1_2 = 0.1633 * 1000;
const double THETA_1_3 = 1.4527 * 1000;
const double THETA_2_1 = -0.0105 * 1000;
const double THETA_2_2 = 0.1304 * 1000;
const double THETA_2_3 = 1.5477 * 1000;
const double THETA_C_1 = 4.0000 * 1000;
const double THETA_C_2 = 1.5000 * 1000;

const double EPS = 0.01;
const double MAX_kgf = 3.5; // Max forward force of a single thrust with 12V of power supply
const double MIN_kgf = -2.6; // Max reverse force of a single thrust with 12V of power supply

struct OutputValues {
    double T5, T6, T7, T8; // Vertical thrusters (that are the ones that affect z roll and pitch)
};

OutputValues compute_thrust(double Fz, double Fr, double Fp); // Convert Force along z and moments around roll and pitch into thrust forces
double lookup_table(double T);

#endif