#ifndef BAR02_H
#define BAR02_H

#include <iostream>
#include "driver_ms5837_basic.h"
#include "logger.hpp"
#include <sstream> //for using std::ostringstream

/**
 * @brief Bar02 class representing a pressure sensor.
 */
class Bar02 {
public:
    /**
     * @brief Constructor for Bar02 class.
     */
    Bar02(logLevel minimumLogLevel);

    /**
     * @brief Reads data from the sensor.
     */
    void read_sensor();

    /**
     * @brief Gets the status of the sensor.
     * @return Status of the sensor.
     */
    int get_status();

    /**
     * @brief Sets the pressure baseline.
     */
    void set_pressure_baseline();

    /**
     * @brief Gets the temperature in Celsius.
     * @return Temperature in Celsius.
     */
    float get_temperature();

    /**
     * @brief Gets the depth in meters.
     * @return Depth in meters.
     */
    float get_depth();

private:
    int status;         ///< Status of the sensor.
    float pressure_baseline; ///< Pressure baseline.
    float temperature;  ///< Temperature in Celsius.
    float depth;        ///< Depth in meters.
    Logger logger;      ///< Logger    
};

#endif // BAR02_H