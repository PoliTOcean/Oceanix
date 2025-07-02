#include "bar02.hpp"

Bar02::Bar02(logLevel minimumLogLevel, bool test_mode) 
    : status(0), 
    temperature(0.0), 
    depth(0.0), 
    test_mode(test_mode),
    simulation_time(0.0f),
    logger(Logger(BAR02_LOG_NAME, minimumLogLevel)) {
    
    if (!test_mode) {
        status = ms5837_basic_init(MS5837_TYPE_02BA01);
        if (status != 0)
            logger.log(logERROR, "barometer init fail");
        else
            logger.log(logINFO, "init done");
    } else {
        logger.log(logINFO, "barometer in simulation mode");
        status = 0; // Always good status in test mode
    }
}

void Bar02::read_sensor() {
    if (test_mode) {
        // In test mode, update values with simulated data
        temperature = simulate_temperature();
        depth = simulate_depth();
        return;
    }
    
    float pressure_mbar;
    status = ms5837_basic_read(&temperature, &pressure_mbar);
    if (status != 0){
        logger.log(logERROR, "barometer read fail");
        return;
    }
    else
      depth = (pressure_mbar*100)/(9.80665*997.0f);  //997=density fresh water
}


/*
@return 0 if OK
*/
int Bar02::get_status() {
    return status;
}

// Function to return the external temperature in Celsius
float Bar02::get_temperature() {
    return temperature;
}

// Function to return the depth in meters
float Bar02::get_depth() {
    return depth;
}

void Bar02::update_parameters(const json& general_config){
    logger.setLogLevel(general_config["bar02_loglevel"]);
}

void Bar02::update_simulation_time(float dt) {
    if (test_mode)
        simulation_time += dt;
}

float Bar02::simulate_temperature() {
    // Slow variation around 25°C with 5°C amplitude
    return 25.0f + 5.0f * sin(0.05f * simulation_time);
}

float Bar02::simulate_depth() {
    // Medium-speed variation between 0.5m and 3.5m
    return 2.0f + 1.5f * sin(0.2f * simulation_time);
}