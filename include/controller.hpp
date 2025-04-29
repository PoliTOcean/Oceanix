#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <json.hpp>
#include "sensor.hpp"
#include "motors.hpp"
#include "logger.hpp"
#include "EVA_MIMOControllerCodeGen.h"
//#include "control_allocation.hpp"
//#include "FullStateFeedbackControl.hpp"

using json = nlohmann::json;

const uint8_t CONTROL_OFF =     0x00;    //000
const uint8_t CONTROL_Z =       0x01;    //001
const uint8_t CONTROL_ROLL =    0x02;    //010
const uint8_t CONTROL_PITCH =   0x04;    //100
const uint8_t CONTROL_ALL =     0x07;    //111


class Controller {
    public:
        void virtual set_reference(uint8_t ref_type, float ref) = 0;
        float virtual get_reference(uint8_t ref_type) = 0;
        void virtual set_parameters(const json& general_config, const json& specific_config) = 0;
        void virtual calculate(float* motor_thrust) = 0;
        json virtual get_status() = 0; 
        void virtual activate(uint8_t ref_type) = 0;
        void virtual deactivate(uint_t ref_type) = 0;
}

/**
 * @class Controller
 * @brief Controls the depth, pitch and roll. When active calculates the thrust for the 4 UP motors
 */
class MIMOController: public Controller {
private:
    uint8_t state;                      /// controller's current state
    bool controller_active;         /// controller internal state  
    bool controller_active_old;     /// last controller internal state
    float reference_z;              /// reference depth
    float reference_roll;           /// reference CONTROL_Z | CONTROL_ROLL | CONTROL_PITCH
    float reference_pitch;          /// reference pitch
    bool c_verbose;                   /// verbose mode
    Logger logger;
    Sensor& sensor;                  /// sensor class
    EVA_MIMOControlCodeGen mimo_controller;

public:
    /**
     * @brief Constructor for the Controller class.
     * @param sensor pointer to the sensor class, must include depth, pitch and roll sensors
     * @param jsonConfig Pointer to the json object with the configuration.
     * @param verbose if true [INFO] are printed
     */
    Controller(Sensor& sensor, json jsonConfig, logLevel minimumLoglevel);

    /**
    * @brief the variable state is controlled from external methods, changes following inputs from the gui
    * @brief automatically this function activate and disactivate the controller when using the z axis in respect to the current state
    */
    void calculate(float* motor_thrust);

    /**
     * @brief control the state of the controller activating the corresponding section
     * 
     * @param ref_type CONTROL_Z | CONTROL_ROLL | CONTROL_PITCH or CONTROL_ALL
     */
    void activate(uint8_t ref_type);

    /**
     * @brief totally or partially disactivate the controller (corresponding to parameter)
     * 
     * @param ref_type CONTROL_Z | CONTROL_ROLL | CONTROL_PITCH or CONTROLL_ALL
     */
    void disactivate(uint8_t ref_type);

    /**
     * @brief change the reference for one axis
     * 
     * @param ref_type CONTROL_Z or CONTROL_ROLL or CONTROL_PITCH
     * @param ref reference value
     */
    void change_reference(uint8_t ref_type, float ref);

    /**
     * @brief Get the reference object
     * 
     * @param ref_type CONTROL_Z or CONTROL_ROLL or CONTROL_PITCH
     * @return float the reference value
     */
    float get_reference(uint8_t ref_type);

    /**
     * @brief Update internal parameters from config
     */
    void update_parameters(const json& general_config, const json& specific_config);


    /**
     * @brief Get the status in JSON
     * 
     * @return json containing the status
     */
    json get_status();
};

#endif // CONTROLLER_H
