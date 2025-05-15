#ifndef BAR02_H
#define BAR02_H

#include <iostream>
#include "driver_ms5837_basic.h"
#include "logger.hpp"
#include <sstream> //for using std::ostringstream
#include "utils.hpp"

/**
 * @brief Bar02 class representing a pressure sensor.
 */
class Bar02 {
public:
    /**
     * @brief Constructor for Bar02 class.
     */
    Bar02(logLevel minimumLogLevel, bool test_mode = false);

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
     * @brief Gets the temperature in Celsius.
     * @return Temperature in Celsius.
     */
    float get_temperature();

    /**
     * @brief Gets the depth in meters.
     * @return Depth in meters.
     */
    float get_depth();

    void update_parameters(const json& general_config);
    
    /**
     * @brief Update simulation time (only used in test mode)
     * 
     * @param dt Time increment in seconds
     */
    void update_simulation_time(float dt);

private:
    int status;         ///< Status of the sensor.
    float temperature;  ///< Temperature in Celsius.
    float depth;        ///< Depth in meters.
    bool test_mode;     ///< Flag to indicate test mode
    float simulation_time; ///< Time counter for simulations (seconds)
    
    Logger logger;      ///< Logger
    
    // Simulation methods
    float simulate_temperature();
    float simulate_depth();
};

#endif // BAR02_H