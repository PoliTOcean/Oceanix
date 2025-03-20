#ifndef WT61_H
#define WT61_H

#include <iostream>
#include "WIT61P.h"
#include "logger.hpp"
#include "utils.hpp"

class Wt61{
public:
    /**
     * @brief Construct a new imu Wt61 object
     * 
     */
    Wt61(logLevel minimumLoglevel);

    /**
     * @brief call for updating the measure
     * 
     */
    void read_sensor();

    /**
     * @brief Get the status of the sensor
     * 
     * @return int the status if = 0 everything works
     */
    int get_status();

    /**
     * @brief Get the temperature value
     * 
     * @return float temperature in Celsius
     */
    float get_temperature();

    /**
     * @brief Get the roll value
     * 
     * @return float roll in DEG
     */
    float get_roll();

    /**
     * @brief Get the pitch value
     * 
     * @return float pitch in DEG
     */
    float get_pitch();

    /**
     * @brief Get the yaw value
     * 
     * @return float yaw in DEG
     */
    float get_yaw();

    /**
     * @brief Get the acc array
     * 
     * @return float* array for the accellerations (x, y, z) in m/s^2
     */
    float* get_acc();

    /**
     * @brief Get the gyro array
     * 
     * @return float* array for the gyroscope (x, y, z) in DEG/s^2
     */
    float* get_gyro();

    void update_parameters(const json& general_config);

private:
    int status;         ///< status of the sensor
    float temperature;  ///< internal temperature in Celsius
    float roll;         ///< roll angle in DEG
    float pitch;        ///< pitch angle in DEG
    float yaw;          ///< yaw angle in DEG
    float acc[3];       ///< accelleration (x, y, z) in m/s^2
    float gyro[3];      ///< gyroscope (x, y, z) in DEG/s^2

    Logger logger;
};

#endif // WT61_H