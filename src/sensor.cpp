#include "sensor.hpp"

// Initialize static members
std::mutex Sensor::write_mtx;
std::atomic<bool> Sensor::thread_running{true};
std::thread Sensor::sensor_thread;

Sensor::Sensor(const json& general_config, bool test_mode) 
    : imu(Wt61(general_config["imu_loglevel"], general_config, test_mode)), 
    barometer(Bar02(general_config["bar02_loglevel"], test_mode)),
    alpha(general_config["Zspeed_alpha"]),
    beta(general_config["Zspeed_beta"]),
    roll_offset(general_config["imu_roll_offset"]), 
    pitch_offset(general_config["imu_pitch_offset"]),
    yaw_offset(0),
    depth_offset(0) {
    
    barometer_values = {
        .state = false,
        .depth = 0.00,
        .external_temperature = 0.00,
    };

    imu_values = {
        .state = false,
        .roll = 0.00,
        .pitch = 0.00,
        .yaw = 0.00,
        .z_speed = 0.00,
        .acc = {},
        .gyro = {},
        .internal_temperature = 0.00
    };

    std::srand(0);
}

// ! [PUBLIC FUNCTIONS]

// Function to set the pressure baseline for depth calculations
void Sensor::set_pressure_baseline() {
    yaw_offset = yaw_offset - get_yaw(); //set the yaw to zero when arming rov
    depth_offset = depth_offset - get_depth();  //set the depth to zero when arming rov

    //logMessage << "Depth baseline set to: " << depth_offset << " m";  //TODO: add logger
    //logger.log(logINFO, logMessage.str()); 
}

json Sensor::get_status() {
    json status;
    // IMU related values
    status["imu_state"] = (sensor_status() & IMU_OK) ? "OK" : "OFF";
    status["roll"] = floatToStringWithDecimals(get_roll(), 3);
    status["pitch"] = floatToStringWithDecimals(get_pitch(), 3);
    status["yaw"] = floatToStringWithDecimals(get_yaw(), 3);
    status["Zspeed"] = floatToStringWithDecimals(get_Zspeed(), 3);
    status["Zacc"] = floatToStringWithDecimals(get_acc()[2], 3);
    status["angular_x"] = floatToStringWithDecimals(get_gyro()[0], 3);
    status["angular_y"] = floatToStringWithDecimals(get_gyro()[1], 3);
    status["angular_z"] = floatToStringWithDecimals(get_gyro()[2], 3);

    // Barometer related values
    status["bar_state"] = (sensor_status() & BAR_OK) ? "OK" : "OFF";
    status["depth"] = floatToStringWithDecimals(get_depth(), 3);
    status["internal_temperature"] = floatToStringWithDecimals(get_internal_temperature(), 3);
    status["external_temperature"] = floatToStringWithDecimals(get_external_temperature(), 3);
    
    return status;
}

// Function to check the status of all sensors and return a combined status
int Sensor::sensor_status() {
    int status = 0;
    std::unique_lock<std::mutex> lock(write_mtx);
    
    // Check IMU status
    if (imu_values.state == 0) {
        status |= IMU_OK;  // IMU is OK
    } else {
        status |= IMU_ERROR;  // IMU error
    }

    // Check Barometer status
    if (barometer_values.state == 0) {
        status |= BAR_OK;  // Barometer is OK
    } else {
        status |= BAR_ERROR;  // Barometer error
    }
    lock.unlock();

    return status;
}

// Function to get internal temperature from the barometer
float Sensor::get_internal_temperature() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.internal_temperature;
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
    return value+depth_offset;
}

// Function to get roll from the IMU sensor
float Sensor::get_roll() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.roll;
    lock.unlock();
    return value+roll_offset;
}

// Function to get pitch from the IMU sensor
float Sensor::get_pitch() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.pitch;
    lock.unlock();
    return value+pitch_offset;
}

// Function to get yaw from the IMU sensor
float Sensor::get_yaw() {
    float value;
    std::unique_lock<std::mutex> lock(write_mtx);
    value = imu_values.yaw;
    lock.unlock();
    return value+yaw_offset;
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
    roll_offset = general_config["imu_roll_offset"];
    pitch_offset = general_config["imu_pitch_offset"];
}

// ! [PRIVATE FUNCTIONS]

// Function to read sensor data from all sensors
void Sensor::read_sensor() {
    imu.read_sensor();      // Read data from the IMU sensor
    barometer.read_sensor(); // Read data from the barometer
}

float Sensor::get_Zspeed_hardware() {
    float accel_z = imu.get_acc()[2] + 9.80;

    prev_depth = filtered_depth;
    prev_speed = fused_speed;

    // Step 1: Apply low-pass filter to acceleration and depth
    filtered_accel = lowPassFilter(accel_z, filtered_accel);
    filtered_depth = lowPassFilter(barometer.get_depth(), filtered_depth);

    // Step 2: Speed estimate from depth differentiation
    float speed_from_depth = (filtered_depth - prev_depth) / dt;

    // Step 3: Speed estimate from acceleration integration
    float speed_from_accel = prev_speed + filtered_accel * dt;

    // Step 4: Fuse the two estimates using a complementary filter
    fused_speed = alpha * speed_from_accel + (1 - alpha) * speed_from_depth;

    return fused_speed;
}

// Simplify write_sensor function by calling sensor methods directly
void Sensor::write_sensor() {
    Imu tmp_imu_values;
    Barometer tmp_barometer_values;

    // Get Z speed (needs calculations from both sensors)
    float z_speed = get_Zspeed_hardware();

    // Now set all values directly from sensors
    tmp_imu_values = {
        .state = imu.get_status() == 0,
        .roll = imu.get_roll(),
        .pitch = imu.get_pitch(),
        .yaw = imu.get_yaw(),
        .z_speed = z_speed,
        .acc = imu.get_acc(),
        .gyro = imu.get_gyro(),
        .internal_temperature = imu.get_temperature()
    };
    
    // Calculate correction for depth based on orientation
    float x = 0.134;
    float y = -0.105;
    float depth_offset_correction = 0;
    
    depth_offset_correction += sin(imu.get_pitch()*DEGtoRAD) * x;
    depth_offset_correction -= sin(imu.get_roll()*DEGtoRAD) * y;
    
    // Barometer related values
    tmp_barometer_values = {
        .state = barometer.get_status() == 0,
        .depth = barometer.get_depth() + depth_offset_correction,
        .external_temperature = barometer.get_temperature(),
    };
    
    // Copy values to the shared structs with mutex protection
    std::unique_lock<std::mutex> lock(write_mtx);
    barometer_values = tmp_barometer_values;
    imu_values = tmp_imu_values;
    lock.unlock();
}

void Sensor::update_thread(Sensor *sensor, uint64_t timeout) {
    while (thread_running) {  // Check termination flag
        //update simulation time in the sensor classes
        float dt = 0.001f; // 1ms update
        sensor->imu.update_simulation_time(dt);
        sensor->barometer.update_simulation_time(dt);
        
        sensor->read_sensor();
        sensor->write_sensor();

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
}

void Sensor::stop_thread_and_wait() {
    thread_running = false;  // Signal thread to stop
    
    if (sensor_thread.joinable()) {
        sensor_thread.join();  // Wait for thread to finish
    }
}