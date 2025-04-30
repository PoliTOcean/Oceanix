#include "MIMO_controller.hpp"

MIMOController::MIMOController(Sensor& sensor, json jsonConfig, logLevel minimumLoglevel) 
    : sensor(sensor), 
    state(CONTROL_OFF),
    reference_roll(0),
    reference_pitch(0),
    mimo_controller(EVA_MIMOControlCodeGen()),
    logger(Logger(CONTROLLER_LOG_NAME, minimumLoglevel)) 
    {
        set_parameters({ { "controller_loglevel", minimumLoglevel } }, jsonConfig);
}

void MIMOController::calculate(float* motor_thrust) {  //directly modify the motor_thrust array from motors class
    std::ostringstream message;
    
    float total_power=0;
    float* gyro;

    for(int i=4; i<8; i++)
        total_power += motor_thrust[i];
    
    controller_active_old = controller_active;

    if(abs(total_power) < 0.01 && state != CONTROL_OFF && sensor.get_depth() > 0.05)
        controller_active = true;
    else{
        controller_active = false;
        return;
    }
    
    if(controller_active == true && controller_active_old == false){
        reference_z = sensor.get_depth();
        if (c_verbose){
            message  << "control Z active at depth " << reference_z;
            logger.log(logINFO, "Invalid reference type");
        }
    }

    if (state == CONTROL_OFF)
        return;
    else {
        gyro = sensor.get_gyro();
        mimo_controller.rtU.y_measurement[0] = (double) sensor.get_depth();
        mimo_controller.rtU.y_measurement[1] = (double) gyro[0];
        mimo_controller.rtU.y_measurement[2] = (double) sensor.get_roll()*DEGtoRAD;
        mimo_controller.rtU.y_measurement[3] = (double) gyro[1];
        mimo_controller.rtU.y_measurement[4] = (double) sensor.get_pitch()*DEGtoRAD;

        mimo_controller.rtU.z_ref = (double) reference_z;
        mimo_controller.rtU.pitch_ref = reference_pitch; 
        mimo_controller.rtU.roll_ref = reference_roll;
        mimo_controller.step();    
    }
    
    motor_thrust[static_cast<int>(MotorID::UPFDX)] = (float) mimo_controller.rtY.u[0]; 
    motor_thrust[static_cast<int>(MotorID::UPFSX)] = (float) mimo_controller.rtY.u[1];
    motor_thrust[static_cast<int>(MotorID::UPRDX)] = (float) mimo_controller.rtY.u[2]; 
    motor_thrust[static_cast<int>(MotorID::UPRSX)] = (float) mimo_controller.rtY.u[3];
}

void MIMOController::activate(uint8_t ref_type) {
    // Check that ref_type has at maximum the first 3 bits set
    if ((ref_type & CONTROL_ALL) == ref_type)  
        state |= ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void MIMOController::deactivate(uint8_t ref_type) {
    // Check that ref_type has at maximum the first 3 bits set
    if ((ref_type & CONTROL_ALL) == ref_type)  
        state &= ~ref_type;
    else
        logger.log(logERROR, "Invalid reference type");
}

void MIMOController::set_reference(uint8_t ref_type, float ref) {
    
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
    
void MIMOController::set_parameters(const json& general_config, const json& params) {
    int i;
    for (i=0; i<36; i++) mimo_controller.rtConstP.Admatrix_Gain[i] = params["Admatrix_Gain"].at(i);
    for (i=0; i<12; i++) mimo_controller.rtConstP.IntegratorGain_Gain[i] = params["IntegratorGain_Gain"].at(i);
    for (i=0; i<24; i++) mimo_controller.rtConstP.StateGain_Gain[i] = params["StateGain_Gain"].at(i);
    for (i=0; i<24; i++) mimo_controller.rtConstP.Bdmatrix_Gain[i] = params["Bdmatrix_Gain"].at(i);
    for (i=0; i<30; i++) mimo_controller.rtConstP.Cdmatrix_Gain[i] = params["Cdmatrix_Gain"].at(i);
    for (i=0; i<30; i++) mimo_controller.rtConstP.Ldmatrix_Gain[i] = params["Ldmatrix_Gain"].at(i);

    double ExtractRPZ_Gain[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    for (i = 0; i<18; i++)
        mimo_controller.rtConstP.ExtractRPZ_Gain[i] = ExtractRPZ_Gain[i];
    
    logger.setLogLevel(general_config["controller_loglevel"]);
}

float MIMOController::get_reference(uint8_t ref_type) {
    if(ref_type == CONTROL_Z)
        return reference_z;
    else if(ref_type == CONTROL_ROLL)
        return reference_roll;
    else if(ref_type == CONTROL_PITCH)
        return reference_pitch;
    logger.log(logERROR, "Invalid reference type");
    return 0;
}

json MIMOController::get_status(){
    json status;
    status["controller_state"]["DEPTH"] = (state & CONTROL_Z) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    status["controller_state"]["ROLL"] = (state & CONTROL_ROLL) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    status["controller_state"]["PITCH"] = (state & CONTROL_PITCH) ? (controller_active ? "ACTIVE" : "READY") : "OFF";
    /*
    status["force_z"] = floatToStringWithDecimals(force_z, 3);
    status["force_roll"] = floatToStringWithDecimals(force_roll, 3);
    status["force_pitch"] = floatToStringWithDecimals(force_pitch, 3);
    */

    status["reference_z"] = floatToStringWithDecimals(reference_z, 3);
    status["reference_roll"] = floatToStringWithDecimals(reference_roll, 3);
    status["reference_pitch"] = floatToStringWithDecimals(reference_pitch, 3);

    status["error_integral"]["Z"] = floatToStringWithDecimals(mimo_controller.rtDW.DiscreteTimeIntegrator_DSTATE[0], 3);
    status["error_integral"]["ROLL"] = floatToStringWithDecimals(mimo_controller.rtDW.DiscreteTimeIntegrator_DSTATE[1], 3);
    status["error_integral"]["PITCH"] = floatToStringWithDecimals(mimo_controller.rtDW.DiscreteTimeIntegrator_DSTATE[2], 3);

    return status;
}
