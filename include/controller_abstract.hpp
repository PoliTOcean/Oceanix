#ifndef CONTROLLER_INTERFACE_H
#define CONTROLLER_INTERFACE_H

#include <json.hpp>

using json = nlohmann::json;

const uint8_t CONTROL_OFF =     0x00;    //000
const uint8_t CONTROL_Z =       0x01;    //001
const uint8_t CONTROL_ROLL =    0x02;    //010
const uint8_t CONTROL_PITCH =   0x04;    //100
const uint8_t CONTROL_ALL =     0x07;    //111

class Controller {
public:
    virtual void set_reference(uint8_t ref_type, float ref) = 0;
    virtual float get_reference(uint8_t ref_type) = 0;
    virtual void set_parameters(const json& general_config, const json& specific_config) = 0;
    virtual void calculate(float* motor_thrust) = 0;
    virtual json get_status() = 0;
    virtual void activate(uint8_t ref_type) = 0;
    virtual void deactivate(uint8_t ref_type) = 0;
    virtual ~Controller() = default; //virtual destructor
};

#endif // CONTROLLER_INTERFACE_H