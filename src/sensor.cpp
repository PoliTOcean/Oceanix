#include "sensor.hpp"

Sensor::Sensor(float Zspeed_alpha, float Zspeed_beta) 
    : imu(Wt61()), 
    barometer(Bar02()),
    alpha(Zspeed_alpha),
    beta(Zspeed_beta) {}

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
    float depth = barometer.get_depth();
    float x = 0.134;
    float y = -0.105;
    float offset = 0;
    
    offset += sin(get_pitch()*DEGtoRAD) * x;
    offset -= sin(get_roll()*DEGtoRAD) * y;
    
    return depth + offset;
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

float Sensor::lowPassFilter(float current_value, float prev_filtered_value) {
    return beta * current_value + (1 - beta) * prev_filtered_value;
}

float Sensor::get_Zspeed() {
    float accel_z = get_acc()[2] + 9.80;

    prev_depth=filtered_depth;
    prev_speed=fused_speed;

    // Step 1: Apply low-pass filter to acceleration and depth
    filtered_accel = lowPassFilter(accel_z, filtered_accel);
    filtered_depth = lowPassFilter(get_depth(), filtered_depth);

    // Step 2: Speed estimate from depth differentiation
    float speed_from_depth = (filtered_depth - prev_depth) / dt;

    // Step 3: Speed estimate from acceleration integration
    float speed_from_accel = prev_speed + filtered_accel * dt;

    // Step 4: Fuse the two estimates using a complementary filter
    fused_speed = alpha * speed_from_accel + (1 - alpha) * speed_from_depth;

    return fused_speed;
}

void Sensor::update_debug(json& debug){
    read_sensor();
    debug["imu_state"] = (imu.get_status() == 0) ? "OK" : "OFF";
    debug["bar_state"] = (barometer.get_status() == 0) ? "OK" : "OFF";
    debug["depth"] = floatToStringWithDecimals(get_depth(), 3);
    debug["roll"] = floatToStringWithDecimals(get_roll(), 3);
    debug["pitch"] = floatToStringWithDecimals(get_pitch(), 3);
    debug["yaw"] = floatToStringWithDecimals(get_yaw(), 3);
    debug["internal_temperature"] = floatToStringWithDecimals(get_internal_temperature(), 3);
    debug["external_temperature"] = floatToStringWithDecimals(get_external_temperature(), 3);
    debug["Zspeed"] = floatToStringWithDecimals(get_Zspeed(), 3);
    debug["Zacc"] = floatToStringWithDecimals(get_acc()[2], 3);
}