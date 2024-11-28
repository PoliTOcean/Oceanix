#include "controller.hpp"

Controller::Controller(Sensor sensor, json jsonConfig, bool verbose) 
    : sensor(sensor), 
    state(CONTROL_OFF),
    reference_roll(0),
    reference_pitch(0),
    c_verbose(verbose),
    control_z(ControlSystemZ(jsonConfig["minForceZ"], jsonConfig["maxForceZ"], jsonConfig["minErrorBar"], jsonConfig["weight"], jsonConfig["buoyancy"], 
                                    jsonConfig["denFHeave2"], jsonConfig["numFHeave1"], jsonConfig["numFHeave2"], jsonConfig["denCHeave2"], jsonConfig["denCHeave3"], 
                                    jsonConfig["numCHeave2"], jsonConfig["numCHeave3"], jsonConfig["cZ_inf"])),
    control_pitch(ControlSystemPITCH(jsonConfig["minForcePitch"], jsonConfig["maxForcePitch"], (float)jsonConfig["minErrorImu"]*DEGtoRAD, jsonConfig["weight"], jsonConfig["buoyancy"],
                                    jsonConfig["denFPitch2"], jsonConfig["denFPitch3"], jsonConfig["numFPitch2"], jsonConfig["numFPitch3"], jsonConfig["denCPitch2"], 
                                    jsonConfig["denCPitch3"], jsonConfig["numCPitch2"], jsonConfig["numCPitch3"], jsonConfig["cPITCH_inf"])),
    control_roll(ControlSystemROLL(jsonConfig["minForceRoll"], jsonConfig["maxForceRoll"], (float)jsonConfig["minErrorImu"]*DEGtoRAD, jsonConfig["weight"], jsonConfig["buoyancy"], 
                                    jsonConfig["denCRoll2"], jsonConfig["denCRoll3"], jsonConfig["numCRoll2"], jsonConfig["numCRoll3"], jsonConfig["cROLL_inf"])){}

void Controller::calculate(float* motor_thrust) {  //directly modify the motor_thrust array from motors class
    float total_power=0;
    force_z=0;
    force_roll=0;
    force_pitch=0;

    for(int i=4; i<8; i++)
        total_power += motor_thrust[i];
    
    controller_active_old = controller_active;

    if(total_power<0.01 && state!=CONTROL_OFF)
        controller_active=true;
    else{
        controller_active=false;
        return;
    }
    
    if(controller_active==true && controller_active_old==false){
        reference_z=sensor.get_depth();
        if(c_verbose)
            std::cout << "[CONTROLLER][INFO] control Z active at depth " << reference_z << std::endl;
    }

    if(state == CONTROL_OFF)
        return;
    if(state & CONTROL_Z && controller_active)
        force_z = control_z.calculateZ(reference_z*10, sensor.get_depth());   //figure out why *10
    if(state & CONTROL_ROLL && controller_active)
        force_roll = control_roll.calculateRoll(reference_roll*DEGtoRAD, sensor.get_roll()*DEGtoRAD);
    if(state & CONTROL_PITCH && controller_active)
        force_pitch = control_pitch.calculatePitch(reference_pitch*DEGtoRAD, sensor.get_pitch()*DEGtoRAD);
    
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
        std::cout << "[CONTROLLER][ERROR] Invalid reference type" << std::endl;
}

void Controller::disactivate(uint8_t ref_type) {
    if((ref_type & CONTROL_ALL) == ref_type)  // Check that ref_type has at maximum the first 3 bits set
        state &= ~ref_type;
    else
        std::cout << "[CONTROLLER][ERROR] Invalid reference type" << std::endl;
}

void Controller::change_reference(uint8_t ref_type, float ref) {
    switch(ref_type) {
        case CONTROL_Z:
            reference_z = ref;
            if(c_verbose)
                std::cout << "[CONTROLLER][INFO] reference_z changed to: " << reference_z << std::endl;
            break;
        case CONTROL_ROLL:
            reference_roll = ref;
            if(c_verbose)
                std::cout << "[CONTROLLER][INFO] reference_roll changed to: " << reference_roll << std::endl;
            break;
        case CONTROL_PITCH:
            reference_pitch = ref;
            if(c_verbose)
                std::cout << "[CONTROLLER][INFO] reference_pitch changed to: " << reference_pitch << std::endl;
            break;
        default:
            std::cout << "[CONTROLLER][ERROR] Invalid reference type" << std::endl;
            break;
    }
}

float Controller::get_reference(uint8_t ref_type) {
    if(ref_type == CONTROL_Z)
        return reference_z;
    else if(ref_type == CONTROL_ROLL)
        return reference_roll;
    else if(ref_type == CONTROL_PITCH)
        return reference_pitch;
    std::cout << "[CONTROLLER][ERROR] Invalid reference type" << std::endl;
    return 0;
}

void Controller::update_debug(json& debug){
    debug["controller_state"]["DEPTH"] = (state & CONTROL_Z) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    debug["controller_state"]["ROLL"] = (state & CONTROL_ROLL) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    debug["controller_state"]["PITCH"] = (state & CONTROL_PITCH) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    debug["force_z"] = floatToStringWithDecimals(force_z, 3);
    debug["force_roll"] = floatToStringWithDecimals(force_roll, 3);
    debug["force_pitch"] = floatToStringWithDecimals(force_pitch, 3);
    debug["reference_z"] = floatToStringWithDecimals(reference_z, 3);
    debug["reference_roll"] = floatToStringWithDecimals(reference_roll, 3);
    debug["reference_pitch"] = floatToStringWithDecimals(reference_pitch, 3);
    debug["depth"] = floatToStringWithDecimals(sensor.get_depth(), 3);
    debug["roll"] = floatToStringWithDecimals(sensor.get_roll(), 3);
    debug["pitch"] = floatToStringWithDecimals(sensor.get_pitch(), 3);
}