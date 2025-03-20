#ifndef MOTORS_H
#define MOTORS_H

#include <iostream>
#include <json.hpp>
#include "motor.hpp"
#include "motorID.hpp"
#include "utils.hpp"
#include "logger.hpp"

const int MIN_INPUT_READING = -32678;    // Minimum input reading value from the joystick
const int MAX_INPUT_READING = 32678;     // Maximum input reading value from the joystick
const float PI = 3.141592653589793f;

using json = nlohmann::json;

class Motors
{
private:
    std::array<Motor, static_cast<int>(MotorID::NUM_MOTORS)> motors;    ///< motor array
    float motor_thrust[static_cast<int>(MotorID::NUM_MOTORS)];          ///< motor thrust (same order as motor array)
    uint16_t pwm[static_cast<int>(MotorID::NUM_MOTORS)];                ///< pwm of the motors
    float thrust_max_xy;                                                ///< thrust max limit for the motors controlling x and y
    float thrust_max_z;                                                 ///< thrust max limit for the UP motors

    //must have same order as motorID
    const double fixed_mixing_matrix[8][6] = {
        {sin(PI / 4), -cos(PI / 4), 0, 0, 0, -0.5},//FDX
        {sin(PI / 4), cos(PI / 4), 0, 0, 0, 0.5}, //FSX
        {-cos(PI / 4), -sin(PI / 4), 0, 0, 0, 0.5},//RDX
        {-cos(PI / 4), sin(PI / 4), 0, 0, 0, -0.5},//RSX
        {0, 0, 1, 1, 1, 0},//UPFDX
        {0, 0, 1, 1, -1, 0},//UPFSX    
        {0, 0, 1, -1, 1, 0},//UPRDX     
        {0, 0, 1, -1, -1, 0}//UPRSX       
    };                                                                  ///< matrix for thrust calculation

public:
    /**
     * @brief Construct a new Motors object
     * 
     * @param config the json containig all the configuration parameters
     */
    Motors(json config, logLevel minimumLoglevel);

    /**
    * @brief calculate thrust of all the motors
    * @param json from the 4 movement axes
    * @return pointer to motor_thrust array
    */
    float* calculate_thrust(json axes);

    /**
     * @brief generate pwm value from internal motor_thrust
     * 
     * @return int* array with 8 pwm of the motors
     */
    uint16_t* calculate_pwm();

    /**
     * @brief Get the debug object
     * 
     * @param debug document for adding info
     */
    void update_debug(json& debug);

    /**
     * @brief Increase/decrease the max thrust of all motors
     * 
     * @param offset is the value added to current thrust max
     */
    void offset_thrust_max(float offset);

    /**
    * @brief Change parameters of all motors
    * @param config, json file with new parameters.
    */
    void update_parameters(const json& general_config, const json& specific_config);

private:
    /**
    * @brief map the input interval to the output interval
    * @param x value
    * @param in_min minimum value of the input interval
    * @param in_max maximum value of the input interval
    * @param out_min minimum value of the output interval
    * @param out_max maximum value of the output interval
    * @return mapped value in the output interval
    */
    float normalize(float x, float in_min, float in_max, float out_min, float out_max);

    /**
    * @brief map the input interval (zero centred) to -1, 1
    * @param x value
    * @param in_min minimum value of input interval
    * @param in_max maximum value of output interval
    * @param out_min -1
    * @param out_max 1
    * @return mapped value
    */
    float normalize_quadratic(float x, float in_min, float in_max, float out_min, float out_max);
    void normalize_vector(const float *input_array, float *output_array, int size);

    /**
     * @brief check if the calculated thrust is greater than the limit, if needed limit it
     * 
     * @param thrust current thrust
     * @param thrust_max thrust limit
     * @return float the value of the thrust
     */
    float limit_thrust(float thrust, float thrust_max);
    Logger logger;
};

#endif
