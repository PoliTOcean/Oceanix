#include "motors.hpp"

Motors::Motors(json config, logLevel minimumLoglevel)
    : thrust_max_xy(config["thrust_max_xy"]), thrust_max_z(config["thrust_max_z"]),
    motors{         //same order as motorID
        Motor(MotorID::FDX, config["FDX_coeff"], config["FDX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::FSX, config["FSX_coeff"], config["FSX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::RDX, config["RDX_coeff"], config["RDX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::RSX, config["RSX_coeff"], config["RSX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::UPFDX, config["UPFDX_coeff"], config["UPFDX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::UPFSX, config["UPFSX_coeff"], config["UPFSX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::UPRDX, config["UPRDX_coeff"], config["UPRDX_pwm_zero"], config["pwm_slew_rate_max"]),
        Motor(MotorID::UPRSX, config["UPRSX_coeff"], config["UPRSX_pwm_zero"], config["pwm_slew_rate_max"])
    },
    logger(Logger(MOTORS_LOG_NAME, minimumLoglevel)) {}

float* Motors::calculate_thrust(json axes){
    float z = (float)axes["Z"];
    float y = (float)axes["Y"];
    float x = (float)axes["X"];

    float pitch = (float)axes["PITCH"];
    float roll = (float)axes["ROLL"];
    float yaw = (float)axes["YAW"];

    x = normalize_quadratic(x, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);
    y = normalize_quadratic(y, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);
    
    pitch = normalize_quadratic(pitch, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);
    roll = normalize_quadratic(roll, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);

    yaw = normalize_quadratic(yaw, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);
    z = normalize(z, MIN_INPUT_READING, MAX_INPUT_READING, -1, 1);

    float joystick_input[6] = {x, y, z, pitch, roll, yaw};

    // Perform matrix multiplication
    for (int i = 0; i < 8; ++i) {
        motor_thrust[i] = 0;  // Initialize the result for the current row
        for (int j = 0; j < 6; ++j) {
            motor_thrust[i] += fixed_mixing_matrix[i][j] * joystick_input[j];
        }
    }

    normalize_vector(motor_thrust, motor_thrust, 8);

    for(int i=0; i<8; i++){
        //from [-1, 1] to thrust
        if(i<4)
            motor_thrust[i] *= thrust_max_xy;
        else
            motor_thrust[i] *= thrust_max_z;
    }
    return motor_thrust;
}

uint16_t* Motors::calculate_pwm(){
    for(int i=0; i<8; i++){
        if(i<4)
            motor_thrust[i] = limit_thrust(motor_thrust[i], thrust_max_xy);
        else
            motor_thrust[i] = limit_thrust(motor_thrust[i], thrust_max_z);
    }

    for(int i=0; i<8; i++)
        pwm[i] = motors[i].calculate_pwm(motor_thrust[i]);
    return pwm;
}

void Motors::normalize_vector(const float *input_array, float *output_array, int size){
    float max_abs_value = 0.0f;
    for (uint8_t i = 0; i < size; i++)
    {
        float abs_value = std::abs(input_array[i]);
        if (abs_value > max_abs_value)
        {
            max_abs_value = abs_value;
        }
    }

    if (max_abs_value > 1)
    {
        for (uint8_t i = 0; i < size; i++)
        {
            output_array[i] = input_array[i] / max_abs_value;
        }
    }
}

float Motors::normalize(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float Motors::normalize_quadratic(float x, float in_min, float in_max, float out_min, float out_max){
    int sign = (x > 0) ? 1 : -1;
    
    float max = std::max(std::abs(in_min), std::abs(in_max));

    return (x*x)/(max*max)*sign;
}

float Motors::limit_thrust(float thrust, float thrust_max){
    if(fabsf(thrust)>thrust_max){
        int sign = fabsf(thrust)/thrust;
        thrust = thrust_max * sign;
        logger.log(logWARNING, "motor thrust limit surpassed, limited to the max");
    }
    return thrust;
}

void Motors::update_debug(json& debug){
    for (int i = 0; i < static_cast<int>(MotorID::NUM_MOTORS); ++i) {
        MotorID motor_id = static_cast<MotorID>(i);
        std::string motor_name = motorID_to_string(motor_id);
        
        debug["motor_thrust"][motor_name] = floatToStringWithDecimals(motor_thrust[i], 2);
        debug["pwm"][motor_name] = floatToStringWithDecimals(pwm[i], 2);
        debug["motor_thrust_max_xy"] = floatToStringWithDecimals(thrust_max_xy, 2);
        debug["motor_thrust_max_z"] = floatToStringWithDecimals(thrust_max_z, 2);
    }
}

void Motors::update_parameters(json config) {
    std::string motorNames[] = {"FDX", "FSX", "RDX", "RSX", "UPFDX", "UPFSX", "UPRDX", "UPRSX"}
    for (int i=0; i<8; i++)
        motors[i].change_parameters((float) config[motorNames[i]+"_coeff"], (uint16_t) config[motorNames[i]+"_pwm_zero"])

    thrust_max_xy = config["thrust_max_xy"]; 
    thrust_max_z = config["thrust_max_z"]; 
}

void Motors::offset_thrust_max(float offset){
    thrust_max_xy += offset;
    thrust_max_z += offset;
    std::ostringstream logMessage;

    logMessage << "new motor thrust max for xy: " << thrust_max_xy << std::endl;
    logger.log(logINFO, logMessage.str());

    logMessage << "new motor thrust max for z: " << thrust_max_z << std::endl;
    logger.log(logINFO, logMessage.str());

}
