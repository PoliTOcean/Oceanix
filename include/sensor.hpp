#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <thread>
#include <mutex>
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

    /**
     * @brief Static function: indipendent thread updating sensors' value at a given period
     * 
     * @param sensor The sensor object to be updated. This object holds the current value and any necessary state.
     * @param timeout The time in milliseconds between updates. The function will sleep for this duration between updates
     * 
     * @return Nothing
     */
    static void update_thread(Sensor *sensor, uint64_t timeout);

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

    /**
     * @brief read all sensors, should be called at 100 Hz
     * 
     */
    void read_sensor();
    
    static std::mutex write_mtx;

    typedef struct imu_values {
        bool state;
        float roll;
        float pitch;
        float yaw;
        float z_speed;
        float* acc;
        float* gyro;
    } Imu;

    typedef struct bar_values {
        bool state;
        float depth;
        float internal_temperature;
        float external_temperature;
    } Barometer;



    Imu imu_values;
    Barometer barometer_values;
 
    /**
     * @brief Write sensors values in the structs [CALL IT AFTER A SENSOR UPDATE] 
     *  
     * @warning Thread Safety: Not Safe
     * @return Nothing
     */
    void write_sensor();


    /**
     * @brief get sensor status
     * 
     * @return int status can be compared to the constants values
     */
    int sensor_status_hadware();

    /**
     * @brief Get the internal temperature value
     * 
     * @return float temperature in C°
     */
    float get_internal_temperature_hardware();

    /**
     * @brief Get the external temperature value
     * 
     * @return float temperature in C°
     */
    float get_external_temperature_hardware();

    /**
     * @brief Get the depth
     * 
     * @return float depth in meters
     */
    float get_depth_hardware();

    /**
     * @brief Get the roll
     * 
     * @return float roll in DEG
     */
    float get_roll_hardware();

    /**
     * @brief Get the pitch object
     * 
     * @return float pitch in DEG
     */
    float get_pitch_hardware();

    /**
     * @brief Get the yaw object
     * 
     * @return float yaw in DEG
     */
    float get_yaw_hardware();

    /**
     * @brief Get the acc array (x, y, z)
     * 
     * @return float* array with the 3 axes accelleration in m/s^2
     */
    float* get_acc_hardware();

    /**
     * @brief Get the gyro array (x, y, z)
     * 
     * @return float* array with the 3 axes gyroscope in DEG/s^2
     */
    float* get_gyro_hardware();


    float get_Zspeed_hardware();
};
#endif // SENSOR_H
