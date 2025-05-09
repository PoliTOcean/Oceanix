#include "sensor.hpp"

std::mutex Sensor::write_mtx;

Sensor::Sensor(const json& general_config, bool test_mode) 
    : imu(Wt61(general_config["imu_loglevel"], general_config)), 
    barometer(Bar02(general_config["bar02_loglevel"])),
    alpha(general_config["Zspeed_alpha"]),
    beta(general_config["Zspeed_beta"]),
    test_mode(test_mode) {
    
    barometer_values = {
        .state = false,
        .depth = 0.00,
        .internal_temperature = 0.00,
        .external_temperature = 0.00,
    };

    imu_values = {
        .state = false,
        .roll = 0.00,
        .pitch = 0.00,
        .yaw = 0.00,
        .z_speed = 0.00,
        .acc = {},
        .gyro = {}
    };

    std::srand(0);
}

// ! [PUBLIC FUNCTIONS]

// Function to set the pressure baseline for depth calculations (you can expand this as needed)
void Sensor::set_pressure_baseline() {
    if (test_mode) {
        // Simulate setting pressure baseline in test mode
        return;
    }
    barometer.set_pressure_baseline();
    imu.set_yaw_zero();
}

json Sensor::get_status() {
    json status;

    std::unique_lock<std::mutex> lock(write_mtx);
    
    // IMU related values
    status["imu_state"] = imu_values.state ? "OK" : "OFF";
    status["roll"] = floatToStringWithDecimals(imu_values.roll, 3);
    status["pitch"] = floatToStringWithDecimals(imu_values.pitch, 3);
    status["yaw"] = floatToStringWithDecimals(imu_values.yaw, 3);
    status["Zspeed"] = floatToStringWithDecimals(imu_values.z_speed, 3);
    status["Zacc"] = floatToStringWithDecimals(imu_values.acc[2], 3);
    status["angular_x"] = floatToStringWithDecimals(imu_values.gyro[0], 3);
    status["angular_y"] = floatToStringWithDecimals(imu_values.gyro[1], 3);
    status["angular_z"] = floatToStringWithDecimals(imu_values.gyro[2], 3);

    // Barometer related values
    status["bar_state"] = barometer_values.state ? "OK" : "OFF";
    status["depth"] = floatToStringWithDecimals(barometer_values.depth, 3);
    status["internal_temperature"] = floatToStringWithDecimals(barometer_values.internal_temperature, 3);
    status["external_temperature"] = floatToStringWithDecimals(barometer_values.external_temperature, 3);

    lock.unlock();

    return status;
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

// Function to get internal temperature from the barometer
float Sensor::get_internal_temperature() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = barometer_values.internal_temperature;
    lock.unlock();
    return value;
}

// Function to get external temperature from the barometer
float Sensor::get_external_temperature() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = barometer_values.external_temperature;
    lock.unlock();
    return value;
}

// Function to get depth (from the barometer)
float Sensor::get_depth() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = barometer_values.depth;
    lock.unlock();
    return value;
}

// Function to get roll from the IMU sensor
float Sensor::get_roll() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.roll;
    lock.unlock();
    return value;
}

// Function to get pitch from the IMU sensor
float Sensor::get_pitch() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.pitch;
    lock.unlock();
    return value;
}

// Function to get yaw from the IMU sensor
float Sensor::get_yaw() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.yaw;
    lock.unlock();
    return value;
}

// Function to get acceleration from the IMU sensor
float* Sensor::get_acc() {
    float *values;
    std::unique_lock<std::mutex> lock(write_mtx);
    values = imu_values.acc;
    lock.unlock();
    return values;
}

// Function to get gyroscope data from the IMU sensor
float* Sensor::get_gyro() {
    float *values;
    std::unique_lock<std::mutex> lock(write_mtx);
    values = imu_values.gyro;
    lock.unlock();
    return values;
}

// Function to get z_speed data
float Sensor::get_Zspeed() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.z_speed;
    lock.unlock();
    return value;
}

float Sensor::lowPassFilter(float current_value, float prev_filtered_value) {
    return beta * current_value + (1 - beta) * prev_filtered_value;
}

void Sensor::update_parameters(const json& general_config) {
    barometer.update_parameters(general_config);
    imu.update_parameters(general_config);
}

// ! [PRIVATE FUNCTIONS]

// Function to read sensor data from both IMU and barometer
void Sensor::read_sensor() {
    if (test_mode) {
        return;
    } else {
        imu.read_sensor();      // Read data from the IMU sensor
        barometer.read_sensor(); // Read data from the barometer
    }
}

// Function to get internal temperature from the IMU sensor
float Sensor::get_internal_temperature_hardware() {
    if (test_mode) {
        return simulate_temperature(); // Simulate temperature in test mode
    }
    return imu.get_temperature();
}

// Function to get external temperature from the barometer
float Sensor::get_external_temperature_hardware() {
    if (test_mode) {
        return simulate_temperature(); // Simulate temperature in test mode
    }
    return barometer.get_temperature();
}

// Function to get depth (from the barometer)
float Sensor::get_depth_hardware() {
    if (test_mode) {
        return simulate_depth(); // Simulate depth in test mode
    }
    float depth = barometer.get_depth();
    float x = 0.134;
    float y = -0.105;
    float offset = 0;
    
    offset += sin(get_pitch_hardware()*DEGtoRAD) * x;
    offset -= sin(get_roll_hardware()*DEGtoRAD) * y;
    
    return depth + offset;
}

// Function to get roll from the IMU sensor
float Sensor::get_roll_hardware() {
    if (test_mode) {
        return simulate_angle(); // Simulate roll in test mode
    }
    return imu.get_roll();
}

// Function to get pitch from the IMU sensor
float Sensor::get_pitch_hardware() {
    if (test_mode) {
        return simulate_angle(); // Simulate pitch in test mode
    }
    return imu.get_pitch();
}

// Function to get yaw from the IMU sensor
float Sensor::get_yaw_hardware() {
    if (test_mode) {
        return simulate_angle(); // Simulate yaw in test mode
    }
    return imu.get_yaw();
}

float Sensor::get_Zspeed_hardware() {
    float accel_z = get_acc_hardware()[2] + 9.80;

    prev_depth=filtered_depth;
    prev_speed=fused_speed;

    // Step 1: Apply low-pass filter to acceleration and depth
    filtered_accel = lowPassFilter(accel_z, filtered_accel);
    filtered_depth = lowPassFilter(get_depth_hardware(), filtered_depth);

    // Step 2: Speed estimate from depth differentiation
    float speed_from_depth = (filtered_depth - prev_depth) / dt;

    // Step 3: Speed estimate from acceleration integration
    float speed_from_accel = prev_speed + filtered_accel * dt;

    // Step 4: Fuse the two estimates using a complementary filter
    fused_speed = alpha * speed_from_accel + (1 - alpha) * speed_from_depth;

    return fused_speed;
}

// Function to get acceleration from the IMU sensor
float* Sensor::get_acc_hardware() {
    if (test_mode) {
        static float acc[3] = {simulate_acceleration(), simulate_acceleration(), simulate_acceleration()};
        return acc;
    }
    return imu.get_acc();
}

// Function to get gyroscope data from the IMU sensor
float* Sensor::get_gyro_hardware() {
    if (test_mode) {
        static float gyro[3] = {simulate_gyro(), simulate_gyro(), simulate_gyro()};
        return gyro;
    }
    return imu.get_gyro();
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

void Sensor::write_sensor() {
    Imu tmp_imu_values;
    Barometer tmp_barometer_values;

    // First, get values from sensors

    // IMU related values
    tmp_imu_values= {
        .state = imu.get_status() == 0,
        .roll = get_roll_hardware(),
        .pitch = get_pitch_hardware(),
        .yaw = get_yaw_hardware(),
        .z_speed = get_Zspeed_hardware(),
        .acc = get_acc_hardware(),
        .gyro = get_gyro_hardware()
    };
 
    // Barometer related values
    tmp_barometer_values = {
        .state = barometer.get_status() == 0,
        .depth = get_depth_hardware(),
        .internal_temperature = get_internal_temperature_hardware(),
        .external_temperature = get_external_temperature_hardware(),
    };
    
    // Copy values in the shared struct, minimize mutex time overhead;

    std::unique_lock<std::mutex> lock(write_mtx);

    barometer_values = tmp_barometer_values;
    imu_values = tmp_imu_values;

    lock.unlock();
}

void Sensor::update_thread(Sensor *sensor, uint64_t timeout) {
    while (1) {
        
        sensor->read_sensor();

        sensor->write_sensor();

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}