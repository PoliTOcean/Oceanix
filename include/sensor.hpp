#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <json.hpp>
#include "wt61.hpp"
#include "bar02.hpp"
#include "utils.hpp"

using json = nlohmann::json;

const int IMU_OK=0x01;
const int IMU_ERROR=0x02;
const int BAR_OK=0x04;
const int BAR_ERROR=0x08;

const float DEGtoRAD=0.01745329f;

class Sensor {
public:
    Wt61 imu;           ///< Imu object
    Bar02 barometer;    ///< Barometer object
    
    /**
     * @brief Construct a new Sensor object containing IMU and barometer
     * 
     */
    Sensor(float Zspeed_alpha, float Zspeed_beta, logLevel imuLogLevel, logLevel bar02LogLevel, bool test_mode = false);

    /**
     * @brief read all sensors, should be called at 100 Hz
     * 
     */
    void read_sensor();

    /**
     * @brief get sensor status
     * 
     * @return int status can be compared to the constants values
     */
    int sensor_status();

    /**
     * @brief Set the pressure baseline, to be called at setup with the ROV in water
     * 
     */
    void set_pressure_baseline();

    /**
     * @brief Get the internal temperature value
     * 
     * @return float temperature in C°
     */
    float get_internal_temperature();

    /**
     * @brief Get the external temperature value
     * 
     * @return float temperature in C°
     */
    float get_external_temperature();

    /**
     * @brief Get the depth
     * 
     * @return float depth in meters
     */
    float get_depth();

    /**
     * @brief Get the roll
     * 
     * @return float roll in DEG
     */
    float get_roll();

    /**
     * @brief Get the pitch object
     * 
     * @return float pitch in DEG
     */
    float get_pitch();

    /**
     * @brief Get the yaw object
     * 
     * @return float yaw in DEG
     */
    float get_yaw();

    /**
     * @brief Get the acc array (x, y, z)
     * 
     * @return float* array with the 3 axes accelleration in m/s^2
     */
    float* get_acc();

    /**
     * @brief Get the gyro array (x, y, z)
     * 
     * @return float* array with the 3 axes gyroscope in DEG/s^2
     */
    float* get_gyro();

    float lowPassFilter(float current_value, float prev_filtered_value);

    float get_Zspeed();

    /**
     * @brief Get the status in JSON
     * 
     * @return json containing the status
     */
    json get_status();

    void update_parameters(const json& general_config);

private:
    float prev_depth = 0;    // Previous depth (m)
    float prev_speed = 0;     // Initial speed (m/s)

    // Filtered values initialization
    float filtered_accel = 0;
    float filtered_depth = 0;
    float fused_speed = 0;

    float alpha;  // Complementary filter constant  1-> only accelerometer
    float beta;    // Low-pass filter coefficient (adjust as needed)
    float dt = 0.03;  //30 millis

    bool test_mode;

    float simulate_temperature();
    float simulate_depth();
    float simulate_angle();
    float simulate_acceleration();
    float simulate_gyro();

};
#endif // SENSOR_H
