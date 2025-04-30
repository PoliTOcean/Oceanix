#ifndef PP_CONTROLLER_H
#define PP_CONTROLLER_H

#include <iostream>
#include <json.hpp>
#include "controller_interface.hpp"
#include "sensor.hpp"
#include "motors.hpp"
#include "logger.hpp"
#include "control_allocation.hpp"

using json = nlohmann::json;

/**
 * @class Controller
 * @brief Controls the depth, pitch and roll. When active calculates the thrust for the 4 UP motors
 */
class PPController: public Controller {
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
    PPController(Sensor& sensor, json jsonConfig, logLevel minimumLoglevel);

    /**
    * @brief the variable state is controlled from external methods, changes following inputs from the gui
    * @brief automatically this function activate and deactivate the controller when using the z axis in respect to the current state
    */
    void calculate(float* motor_thrust) override;

    /**
     * @brief control the state of the controller activating the corresponding section
     *
     * @param ref_type CONTROL_Z | CONTROL_ROLL | CONTROL_PITCH or CONTROL_ALL
     */
    void activate(uint8_t ref_type) override;

    /**
     * @brief totally or partially deactivate the controller (corresponding to parameter)
     *
     * @param ref_type CONTROL_Z | CONTROL_ROLL | CONTROL_PITCH or CONTROLL_ALL
     */
    void deactivate(uint8_t ref_type) override;

    /**
     * @brief change the reference for one axis
     *
     * @param ref_type CONTROL_Z or CONTROL_ROLL or CONTROL_PITCH
     * @param ref reference value
     */
    void change_reference(uint8_t ref_type, float ref) override;

    /**
     * @brief Get the reference object
     *
     * @param ref_type CONTROL_Z or CONTROL_ROLL or CONTROL_PITCH
     * @return float the reference value
     */
    float get_reference(uint8_t ref_type) override;

    /**
     * @brief Update internal parameters from config
     */
    void update_parameters(const json& general_config, const json& specific_config) override;


    /**
     * @brief Get the status in JSON
     *
     * @return json containing the status
     */
    json get_status() override;
};

#endif // PP_CONTROLLER_H