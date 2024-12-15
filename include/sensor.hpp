#ifndef SENSOR_H
#define SENSOR_H

#include <iostream>
#include <json.hpp>
#include "wt61.hpp"
#include "bar02.hpp"
#include "utils.hpp"

using json = nlohmann::json;

const int IMU_OK=0x01;
const int IMU_ERROR=0x02;
const int BAR_OK=0x04;
const int BAR_ERROR=0x08;

class Sensor {
public:
    Wt61 imu;           ///< Imu object
    Bar02 barometer;    ///< Barometer object
    
    /**
     * @brief Construct a new Sensor object containing IMU and barometer
     * 
     */
    Sensor(logLevel imuLogLevel, logLevel bar02LogLevel);

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

    /**
     * @brief update the debug json with sensors status, depth, roll, pitch, yaw
     * 
     * @param debug json to be modified
     */
    void update_debug(json& debug);
};

#endif // SENSOR_H
