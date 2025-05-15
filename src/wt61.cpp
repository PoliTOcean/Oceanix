#include "wt61.hpp"

Wt61::Wt61(logLevel minimumLoglevel, const json& general_config, bool test_mode) 
    : status(0), 
    temperature(0.0), 
    roll(0.0), pitch(0.0), yaw(0.0), 
    acc({0,0,0}), gyro({0,0,0}), 
    test_mode(test_mode),
    simulation_time(0.0f),
    logger(Logger(IMU_LOG_NAME, minimumLoglevel)) {
    
    if (!test_mode) {
        char const *dev = "/dev/i2c-1";
        status = WT61P_begin(const_cast<char*>(dev), 0x50);
        if (status != 0)
            logger.log(logERROR, "imu init fail");
        else
            logger.log(logINFO, "imu init success");
    } else {
        logger.log(logINFO, "imu in simulation mode");
        status = 0; // Always good status in test mode
    }
}

void Wt61::read_sensor() {
    if (test_mode) {
        // In test mode, update values with simulated data
        temperature = simulate_temperature();
        roll = simulate_angle(0); // 0 = roll
        pitch = simulate_angle(1); // 1 = pitch
        yaw = simulate_angle(2); // 2 = yaw
        
        acc[0] = simulate_acceleration(0); // x axis
        acc[1] = simulate_acceleration(1); // y axis
        acc[2] = simulate_acceleration(2); // z axis
        
        gyro[0] = simulate_gyro(0); // x axis
        gyro[1] = simulate_gyro(1); // y axis
        gyro[2] = simulate_gyro(2); // z axis
        
        return;
    }
    
    status = WT61P_read_all();
    if (status != 0)
        logger.log(logERROR, "imu read fail");

    double roll_new = WT61P_get_roll();
	double pitch_new = WT61P_get_pitch();
    double yaw_new = WT61P_get_yaw();

    //if no connection read are zeros, so exclude them and keep the previous
    if (roll_new != 0)
        pitch = roll_new;  //swap pitch roll
    if (yaw_new != 0)
        yaw = yaw_new*-1;
    if (pitch_new != 0)
        roll = pitch_new;

    acc[0] = WT61P_get_AY()*9.81; //swap x, y
    acc[1] = WT61P_get_AX()*9.81;
    acc[2] = WT61P_get_AZ()*-9.81; //invert z

    gyro[0] = WT61P_get_GY();
    gyro[1] = WT61P_get_GX();
    gyro[2] = WT61P_get_GZ()*-1;

    temperature = WT61P_get_temp();
}

int Wt61::get_status() {
    return status;
}

float Wt61::get_temperature() {
    return temperature;
}

float Wt61::get_roll() {
    return roll;
}

float Wt61::get_pitch() {
    return pitch;
}

float Wt61::get_yaw() {
    return yaw;
}

float* Wt61::get_acc() {
    return acc;
}

float* Wt61::get_gyro() {
    return gyro;
}


void Wt61::update_parameters(const json& general_config){
    logger.setLogLevel(general_config["imu_loglevel"]);
}

void Wt61::update_simulation_time(float dt) {
    if (test_mode)
        simulation_time += dt;
}

// Simulation methods
float Wt61::simulate_temperature() {
    // Slow variation around 25°C with 5°C amplitude
    return 25.0f + 5.0f * sin(0.05f * simulation_time);
}

float Wt61::simulate_angle(int angle_type) {
    // Each angle uses different frequency and phase for more realistic motion
    switch(angle_type) {
        case 0: // Roll - slower oscillation
            return 5.0f * sin(0.3f * simulation_time);
        case 1: // Pitch - medium oscillation with phase shift
            return 8.0f * sin(0.4f * simulation_time + 1.0f);
        case 2: // Yaw - faster oscillation with different phase
            return 100.0f * sin(0.01f * simulation_time + 2.0f);
        default:
            return 0.0f;
    }
}

float Wt61::simulate_acceleration(int axis) {
    // Each acceleration axis gets a different pattern
    switch(axis) {
        case 0: // X acceleration
            return 0.8f * sin(1.1f * simulation_time);
        case 1: // Y acceleration
            return 0.5f * sin(0.9f * simulation_time + 1.5f);
        case 2: // Z acceleration
            return 0.3f * sin(0.7f * simulation_time + 2.5f);
        default:
            return 0.0f;
    }
}

float Wt61::simulate_gyro(int axis) {
    // Each gyro axis gets a different pattern
    switch(axis) {
        case 0: // X angular velocity
            return 5.0f * sin(1.5f * simulation_time);
        case 1: // Y angular velocity
            return 4.0f * sin(1.2f * simulation_time + 1.0f);
        case 2: // Z angular velocity
            return 3.0f * sin(1.0f * simulation_time + 2.0f);
        default:
            return 0.0f;
    }
}