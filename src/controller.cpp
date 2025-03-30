#include "controller.hpp"

Controller::Controller(Sensor& sensor, json jsonConfig, logLevel minimumLoglevel) 
    : sensor(sensor), 
    state(CONTROL_OFF),
    reference_roll(0),
    reference_pitch(0),
    control_z(ControlSystem(jsonConfig["minForceZ"], jsonConfig["maxForceZ"], jsonConfig["minErrorIntZ"], jsonConfig["maxErrorIntZ"], std::vector<double>{jsonConfig["Kx0_Z"], jsonConfig["Kx1_Z"]}, jsonConfig["Ki_Z"])),
    control_pitch(ControlSystem(jsonConfig["minForcePitch"], jsonConfig["maxForcePitch"], jsonConfig["minErrorIntPitch"], jsonConfig["maxErrorIntPitch"], std::vector<double>{jsonConfig["Kx0_Pitch"], jsonConfig["Kx1_Pitch"]}, jsonConfig["Ki_Pitch"])),
    control_roll(ControlSystem(jsonConfig["minForceRoll"], jsonConfig["maxForceRoll"], jsonConfig["minErrorIntRoll"], jsonConfig["maxErrorIntRoll"], std::vector<double>{jsonConfig["Kx0_Roll"], jsonConfig["Kx1_Roll"]}, jsonConfig["Ki_Roll"])),
    logger(Logger(CONTROLLER_LOG_NAME, minimumLoglevel)){}

void Controller::calculate(float* motor_thrust) {  //directly modify the motor_thrust array from motors class
    std::ostringstream message;
    
    float total_power=0;
    force_z=0;
    force_roll=0;
    force_pitch=0;
    float* gyro;
    gyro = sensor.get_gyro();

    for(int i=4; i<8; i++)
        total_power += motor_thrust[i];
    
    controller_active_old = controller_active;

    if(abs(total_power)<0.01 && state!=CONTROL_OFF && sensor.get_depth()>0.05)
        controller_active=true;
    else{
        controller_active=false;
        return;
    }
    
    if(controller_active==true && controller_active_old==false){
        reference_z=sensor.get_depth();
        if(c_verbose){
            message  << "control Z active at depth " << reference_z;
            logger.log(logINFO, "Invalid reference type");
        }
    }

    if(state == CONTROL_OFF)
        return;
    if(state & CONTROL_Z && controller_active)
        force_z = control_z.calculateU(reference_z, sensor.get_depth(), std::vector<double>{sensor.get_depth(), sensor.get_Zspeed()});
    if(state & CONTROL_ROLL && controller_active)
        force_roll = control_roll.calculateU(0, sensor.get_roll()*DEGtoRAD, std::vector<double>{sensor.get_roll()*DEGtoRAD, gyro[0]*DEGtoRAD});
    if(state & CONTROL_PITCH && controller_active)
        force_pitch = control_pitch.calculateU(0, sensor.get_pitch()*DEGtoRAD, std::vector<double>{sensor.get_pitch()*DEGtoRAD, gyro[1]*DEGtoRAD});
    
    OutputValues thrust = compute_thrust(force_z, force_roll, force_pitch);

    motor_thrust[static_cast<int>(MotorID::UPFDX)] = thrust.T5;
    motor_thrust[static_cast<int>(MotorID::UPRSX)] = thrust.T8;
    motor_thrust[static_cast<int>(MotorID::UPRDX)] = thrust.T7;
    motor_thrust[static_cast<int>(MotorID::UPFSX)] = thrust.T6;
}

void Controller::activate(uint8_t ref_type) {
    if((ref_type & CONTROL_ALL) == ref_type)  // Check that ref_type has at maximum the first 3 bits set
        state |= ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void Controller::disactivate(uint8_t ref_type) {
    if((ref_type & CONTROL_ALL) == ref_type)  // Check that ref_type has at maximum the first 3 bits set
        state &= ~ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void Controller::change_reference(uint8_t ref_type, float ref) {
    
    std::ostringstream message;
    logLevel message_level;

    switch(ref_type) {
        case CONTROL_Z:
            reference_z = ref;
            if(c_verbose){
                message << "reference_z changed to: " << reference_z << std::endl;
                logger.log(logINFO, message.str());
            }
            break;

        case CONTROL_ROLL:
            reference_roll = ref;

            if(c_verbose){
                message << "reference_roll changed to: " << reference_roll << std::endl;
                logger.log(logINFO, message.str());
            }
            break;

        case CONTROL_PITCH:
            reference_pitch = ref;
            if(c_verbose){
                message << "reference_pitch changed to: " << reference_pitch << std::endl;
                logger.log(logINFO, message.str());
            }
            break;

        default:
            logger.log(logERROR, "Invalid reference type");
            break;
    }
}
    
void Controller::update_parameters(const json& general_config, const json& specific_config){

    control_z.update_paramters(specific_config["minForceZ"], specific_config["maxForceZ"], specific_config["minErrorIntZ"], specific_config["maxErrorIntZ"], std::vector<double>{specific_config["Kx0_Z"], specific_config["Kx1_Z"]}, specific_config["Ki_Z"]);
    control_pitch.update_paramters(specific_config["minForcePitch"], specific_config["maxForcePitch"], specific_config["minErrorIntPitch"], specific_config["maxErrorIntPitch"], std::vector<double>{specific_config["Kx0_Pitch"], specific_config["Kx1_Pitch"]}, specific_config["Ki_Pitch"]);
    control_roll.update_paramters(specific_config["minForceRoll"], specific_config["maxForceRoll"], specific_config["minErrorIntRoll"], specific_config["maxErrorIntRoll"], std::vector<double>{specific_config["Kx0_Roll"], specific_config["Kx1_Roll"]}, specific_config["Ki_Roll"]);

    logger.setLogLevel(general_config["controller_loglevel"]);

}

float Controller::get_reference(uint8_t ref_type) {
    if(ref_type == CONTROL_Z)
        return reference_z;
    else if(ref_type == CONTROL_ROLL)
        return reference_roll;
    else if(ref_type == CONTROL_PITCH)
        return reference_pitch;
    logger.log(logERROR, "Invalid reference type");
    return 0;
}

json Controller::get_status(){
    json status;
    status["controller_state"]["DEPTH"] = (state & CONTROL_Z) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    status["controller_state"]["ROLL"] = (state & CONTROL_ROLL) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    status["controller_state"]["PITCH"] = (state & CONTROL_PITCH) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    status["force_z"] = floatToStringWithDecimals(force_z, 3);
    status["force_roll"] = floatToStringWithDecimals(force_roll, 3);
    status["force_pitch"] = floatToStringWithDecimals(force_pitch, 3);
    status["reference_z"] = floatToStringWithDecimals(reference_z, 3);
    status["reference_roll"] = floatToStringWithDecimals(reference_roll, 3);
    status["reference_pitch"] = floatToStringWithDecimals(reference_pitch, 3);
    return status;
}