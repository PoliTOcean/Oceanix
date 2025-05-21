#include "PP_controller.hpp"
#include <cmath> // For std::abs

PPController::PPController(Sensor& sensor, json jsonConfig, logLevel minimumLoglevel, float dt) 
    : sensor(sensor), 
    state(CONTROL_OFF),
    reference_z(0.0),      // Initialize reference_z
    reference_roll(0.0),   // Initialize reference_roll
    reference_pitch(0.0),  // Initialize reference_pitch
    control_z(ControlSystemZ(jsonConfig["minForceZ"], jsonConfig["maxForceZ"], 0, jsonConfig["weight"], jsonConfig["buoyancy"], 
                                    jsonConfig["denFHeave2"], jsonConfig["numFHeave1"], jsonConfig["numFHeave2"], jsonConfig["denCHeave2"], jsonConfig["denCHeave3"], 
                                    jsonConfig["numCHeave2"], jsonConfig["numCHeave3"], jsonConfig["cZ_inf"])),
    pid_pitch_controller_(
        jsonConfig["Kp_pitch"], jsonConfig["Ki_pitch"], jsonConfig["Kd_pitch"],
        jsonConfig["output_min_pitch"], jsonConfig["output_max_pitch"],
        jsonConfig["integral_min_pitch"], jsonConfig["integral_max_pitch"]
    ),
    pid_roll_controller_(
        jsonConfig["Kp_roll"], jsonConfig["Ki_roll"], jsonConfig["Kd_roll"],
        jsonConfig["output_min_roll"], jsonConfig["output_max_roll"],
        jsonConfig["integral_min_roll"], jsonConfig["integral_max_roll"]
    ),
    logger(Logger(CONTROLLER_LOG_NAME, minimumLoglevel)),
    dt_(dt) 
{
    c_verbose = true;
}

void PPController::calculate(float* motor_thrust) {  //directly modify the motor_thrust array from motors class
    std::ostringstream message;
    
    float total_power=0;
    force_z=0;
    force_roll=0;
    force_pitch=0;

    for(int i=4; i<8; i++) 
        total_power += motor_thrust[i];
    
    controller_active_old = controller_active;

    if(std::abs(total_power)<0.01 && state!=CONTROL_OFF && sensor.get_depth()>0.05) 
        controller_active=true;
    else{
        controller_active=false;
        pid_roll_controller_.reset();
        pid_pitch_controller_.reset();
        return;
    }
    
    if(controller_active==true && controller_active_old==false){
        reference_z=sensor.get_depth();
        pid_roll_controller_.reset(); // Reset PID state
        pid_pitch_controller_.reset(); // Reset PID state

        if(c_verbose){ 
            message.str(""); 
            message  << "control Z active at depth " << reference_z;
            logger.log(logINFO, message.str()); 
        }
    }

    if(state == CONTROL_OFF)
        return;

    if(state & CONTROL_Z && controller_active)
        force_z = control_z.calculateZ(reference_z, sensor.get_depth());
    
    if(state & CONTROL_ROLL && controller_active) {
        force_roll = pid_roll_controller_.calculate(reference_roll * DEGtoRAD, sensor.get_roll() * DEGtoRAD, dt_);
    } else {
        pid_roll_controller_.reset(); 
        force_roll = 0;
    }

    if(state & CONTROL_PITCH && controller_active) {
        force_pitch = pid_pitch_controller_.calculate(reference_pitch * DEGtoRAD, sensor.get_pitch() * DEGtoRAD, dt_);
    } else {
        pid_pitch_controller_.reset(); 
        force_pitch = 0;
    }
    
    OutputValues thrust = compute_thrust(force_z, force_roll, force_pitch);

    motor_thrust[static_cast<int>(MotorID::UPFDX)] = thrust.T5;
    motor_thrust[static_cast<int>(MotorID::UPFSX)] = thrust.T6;
    motor_thrust[static_cast<int>(MotorID::UPRDX)] = thrust.T7;
    motor_thrust[static_cast<int>(MotorID::UPRSX)] = thrust.T8;
}

void PPController::activate(uint8_t ref_type) {
    if((ref_type & CONTROL_ALL) == ref_type)  // Check that ref_type has at maximum the first 3 bits set
        state |= ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void PPController::deactivate(uint8_t ref_type) {
    if((ref_type & CONTROL_ALL) == ref_type)  // Check that ref_type has at maximum the first 3 bits set
        state &= ~ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void PPController::set_reference(uint8_t ref_type, float ref) {
    
    std::ostringstream message;

    switch(ref_type) {
        case CONTROL_Z:
            reference_z = ref;
            if(c_verbose){
                message.str(""); 
                message << "reference_z changed to: " << reference_z; 
                logger.log(logINFO, message.str());
            }
            break;

        case CONTROL_ROLL:
            reference_roll = ref; 
            if(c_verbose){
                message.str("");
                message << "reference_roll changed to: " << reference_roll;
                logger.log(logINFO, message.str());
            }
            break;

        case CONTROL_PITCH:
            reference_pitch = ref; 
            if(c_verbose){
                message.str("");
                message << "reference_pitch changed to: " << reference_pitch;
                logger.log(logINFO, message.str());
            }
            break;

        default:
            logger.log(logERROR, "Invalid reference type in set_reference");
            break;
    }
}
    
void PPController::set_parameters(const json& general_config, const json& specific_config){

    if (general_config.contains("motor_interval")) {
        dt_ = general_config["motor_interval"].get<float>() / 1000.0;
    } else {
        dt_ = 0.01; 
        logger.log(logWARNING, "motor_interval not found in general_config, using default dt for PID.");
    }

    // Update ControlSystemZ parameters
    control_z.set_parameters(
        specific_config["minForceZ"], 
        specific_config["maxForceZ"],
        specific_config["weight"], 
        specific_config["buoyancy"],
        specific_config["denFHeave2"],
        specific_config["numFHeave1"],
        specific_config["numFHeave2"],
        specific_config["denCHeave2"],
        specific_config["denCHeave3"],
        specific_config["numCHeave2"],
        specific_config["numCHeave3"],
        specific_config["cZ_inf"]
    );

    // Update PID Roll Controller parameters
    pid_roll_controller_.set_parameters(
        specific_config["Kp_roll"], 
        specific_config["Ki_roll"], 
        specific_config["Kd_roll"],
        specific_config["output_min_roll"], 
        specific_config["output_max_roll"],
        specific_config["integral_min_roll"], 
        specific_config["integral_max_roll"]
    );

    // Update PID Pitch Controller parameters
    pid_pitch_controller_.set_parameters(
        specific_config["Kp_pitch"], 
        specific_config["Ki_pitch"], 
        specific_config["Kd_pitch"],
        specific_config["output_min_pitch"], 
        specific_config["output_max_pitch"],
        specific_config["integral_min_pitch"], 
        specific_config["integral_max_pitch"]
    );

    if (general_config.contains("controller_loglevel")) {
        logger.setLogLevel(general_config["controller_loglevel"]);
    }
}

float PPController::get_reference(uint8_t ref_type) {
    if(ref_type == CONTROL_Z)
        return reference_z;
    else if(ref_type == CONTROL_ROLL)
        return reference_roll; 
    else if(ref_type == CONTROL_PITCH)
        return reference_pitch; 
    logger.log(logERROR, "Invalid reference type in get_reference");
    return 0;
}

json PPController::get_status(){
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
    status["error_integral"]["ROLL"] = floatToStringWithDecimals(pid_roll_controller_.get_integral_term(), 3);
    status["error_integral"]["PITCH"] = floatToStringWithDecimals(pid_pitch_controller_.get_integral_term(), 3);
    return status;
}