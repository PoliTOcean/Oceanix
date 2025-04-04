#include "sensor.hpp"

Sensor::Sensor(float Zspeed_alpha, float Zspeed_beta, logLevel imuLogLevel, logLevel bar02LogLevel, bool test_mode) 
    : imu(Wt61(imuLogLevel)), 
    barometer(Bar02(bar02LogLevel)),
    alpha(Zspeed_alpha),
    beta(Zspeed_beta),
    test_mode(test_mode) {

    std::srand(0);
}

// Function to read sensor data from both IMU and barometer
void Sensor::read_sensor() {
    if (test_mode) {
        return;
    } else {
        imu.read_sensor();      // Read data from the IMU sensor
        barometer.read_sensor(); // Read data from the barometer
    }
}

// Function to check the status of both sensors and return a combined status
int Sensor::sensor_status() {
    if (test_mode) {
        return IMU_OK | BAR_OK; // Simulate both sensors being OK in test mode
    }

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
    if (test_mode) {
        // Simulate setting pressure baseline in test mode
        return;
    }
    barometer.set_pressure_baseline();
}

// Function to get internal temperature from the IMU sensor
float Sensor::get_internal_temperature() {
    if (test_mode) {
        return simulate_temperature(); // Simulate temperature in test mode
    }
    return imu.get_temperature();
}

// Function to get external temperature from the barometer
float Sensor::get_external_temperature() {
    if (test_mode) {
        return simulate_temperature(); // Simulate temperature in test mode
    }
    return barometer.get_temperature();
}

// Function to get depth (from the barometer)
float Sensor::get_depth() {
    if (test_mode) {
        return simulate_depth(); // Simulate depth in test mode
    }
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
    if (test_mode) {
        return simulate_angle(); // Simulate roll in test mode
    }
    return imu.get_roll();
}

// Function to get pitch from the IMU sensor
float Sensor::get_pitch() {
    if (test_mode) {
        return simulate_angle(); // Simulate pitch in test mode
    }
    return imu.get_pitch();
}

// Function to get yaw from the IMU sensor
float Sensor::get_yaw() {
    if (test_mode) {
        return simulate_angle(); // Simulate yaw in test mode
    }
    return imu.get_yaw();
}

// Function to get acceleration from the IMU sensor
float* Sensor::get_acc() {
    if (test_mode) {
        static float acc[3] = {simulate_acceleration(), simulate_acceleration(), simulate_acceleration()};
        return acc;
    }
    return imu.get_acc();
}

// Function to get gyroscope data from the IMU sensor
float* Sensor::get_gyro() {
    if (test_mode) {
        static float gyro[3] = {simulate_gyro(), simulate_gyro(), simulate_gyro()};
        return gyro;
    }
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

json Sensor::get_status() {
    json status;
    read_sensor();
    status["imu_state"] = (imu.get_status() == 0) ? "OK" : "OFF";
    status["bar_state"] = (barometer.get_status() == 0) ? "OK" : "OFF";
    status["depth"] = floatToStringWithDecimals(get_depth(), 3);
    status["roll"] = floatToStringWithDecimals(get_roll(), 3);
    status["pitch"] = floatToStringWithDecimals(get_pitch(), 3);
    status["yaw"] = floatToStringWithDecimals(get_yaw(), 3);
    status["internal_temperature"] = floatToStringWithDecimals(get_internal_temperature(), 3);
    status["external_temperature"] = floatToStringWithDecimals(get_external_temperature(), 3);
    status["Zspeed"] = floatToStringWithDecimals(get_Zspeed(), 3);
    status["Zacc"] = floatToStringWithDecimals(get_acc()[2], 3);
    return status;
}

void Sensor::update_parameters(const json& general_config) {
    barometer.update_parameters(general_config);
    imu.update_parameters(general_config);
}

// Functions to generate simulated sensor data
float Sensor::simulate_temperature() {
    return static_cast<float>(std::rand()) / RAND_MAX * 30; // Simulate temperature in range [0-30]
}

float Sensor::simulate_depth() {
    return static_cast<float>(std::rand()) / RAND_MAX * 5; // Simulate depth in range [0-5]
}

float Sensor::simulate_angle() {
    return static_cast<float>(std::rand()) / RAND_MAX * 20; // Simulate angle in range [0-20]
}

float Sensor::simulate_acceleration() {
    return static_cast<float>((std::rand()) / RAND_MAX * 4) - 2; // Simulate acceleration in range [-2, 2]
}

float Sensor::simulate_gyro() {
    return static_cast<float>((std::rand()) / RAND_MAX * 20) - 10; // Simulate gyro data in range [-10, 10]
}