#include "sensor.hpp"

Sensor::Sensor() 
    : imu(Wt61(LOG_ALL)), 
    barometer(Bar02(LOG_ALL)) {}

// Function to read sensor data from both IMU and barometer
void Sensor::read_sensor() {
    imu.read_sensor();      // Read data from the IMU sensor
    barometer.read_sensor(); // Read data from the barometer
}

// Function to check the status of both sensors and return a combined status
int Sensor::sensor_status() {
    int status = 0;
    
    // Check IMU status
    if (imu.get_status() == 0) {
        status |= IMU_OK;  // IMU is OK
    } else {
        status |= IMU_ERROR;  // IMU error
    }

    // Check Barometer status
    if (barometer.get_status() == 0) {
        status |= BAR_OK;  // Barometer is OK
    } else {
        status |= BAR_ERROR;  // Barometer error
    }

    return status;
}

// Function to set the pressure baseline for depth calculations (you can expand this as needed)
void Sensor::set_pressure_baseline() {
    barometer.set_pressure_baseline();
}

// Function to get internal temperature from the IMU sensor
float Sensor::get_internal_temperature() {
    return imu.get_temperature();
}

// Function to get external temperature from the barometer
float Sensor::get_external_temperature() {
    return barometer.get_temperature();
}

// Function to get depth (from the barometer)
float Sensor::get_depth() {
    return barometer.get_depth();
}

// Function to get roll from the IMU sensor
float Sensor::get_roll() {
    return imu.get_roll();
}

// Function to get pitch from the IMU sensor
float Sensor::get_pitch() {
    return imu.get_pitch();
}

// Function to get yaw from the IMU sensor
float Sensor::get_yaw() {
    return imu.get_yaw();
}

// Function to get acceleration from the IMU sensor
float* Sensor::get_acc() {
    return imu.get_acc();
}

// Function to get gyroscope data from the IMU sensor
float* Sensor::get_gyro() {
    return imu.get_gyro();
}

void Sensor::update_debug(json& debug){
    read_sensor();
    debug["imu_state"] = (imu.get_status() == 0) ? "OK" : "OFF";
    debug["bar_state"] = (barometer.get_status() == 0) ? "OK" : "OFF";
    debug["depth"] = floatToStringWithDecimals(get_depth(), 3);
    debug["roll"] = floatToStringWithDecimals(get_roll(), 3);
    debug["pitch"] = floatToStringWithDecimals(get_pitch(), 3);
    debug["yaw"] = floatToStringWithDecimals(get_pitch(), 3);
}