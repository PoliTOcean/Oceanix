#include "bar02.hpp"

Bar02::Bar02(logLevel minimumLogLevel) : status(0), temperature(0.0), depth(0.0), logger(Logger(BAR02_LOG_NAME, minimumLogLevel)) {
    status = ms5837_basic_init(MS5837_TYPE_02BA01);
    if (status != 0)
        logger.log(logERROR, "barometer init fail");
    else
        logger.log(logINFO, "init done");
}

void Bar02::read_sensor() {
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