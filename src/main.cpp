/*
*********************************************************************************************************
*                               Main of Oceanix
*
* Filename: main.cpp
* Version: 0.1
* Developers: Fassio Simone, Mele Federico, Amendolia Giovanni
* Company: Team PoliTOcean @ Politecnico di Torino
*
* Description:
********************************************************************************************************* 
*/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <uv.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <csignal>
#include <json.hpp>
#include "sensor.hpp"
#include "MIMO_controller.hpp"
#include "PP_controller.hpp"
#include "motors.hpp"
#include "mqtt_client.hpp"
#include "nucleo.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "system_monitor.hpp"


using json = nlohmann::json;

void timer_motors_callback(uv_timer_t* handle);
void timer_com_callback(uv_timer_t* handle);

struct Timer_data {
    Sensor* sensor;
    Motors* motors;
    Controller* controller;
    Nucleo* nucleo;
    MQTTClient* mqtt_client;
    Config* config;
    SystemMonitor* system_monitor;
};

const std::string config_path = "../config/config.json";

json general_config, motors_config;
json json_axes_off = {
    {"X", 0},
    {"Y", 0},
    {"Z", 0},
    {"PITCH", 0},
    {"ROLL", 0},
    {"YAW", 0}
};
json json_axes = {
    {"X", 0},
    {"Y", 0},
    {"Z", 0},
    {"PITCH", 0},
    {"ROLL", 0},
    {"YAW", 0}
};

typedef enum {
    ARM_ROV,
    CHANGE_CONTROLLER_STATUS,
    CHANGE_CONTROLLER_PROFILE,
    CHANGE_PITCH_STATUS,
    CHANGE_ROLL_STATUS,
    CHANGE_DEPTH_STATUS,
    PITCH_REFERENCE_UPDATE,
    ROLL_REFERENCE_UPDATE,
    DEPTH_REFERENCE_UPDATE,
    PITCH_REFERENCE_OFFSET,
    ROLL_REFERENCE_OFFSET,
    DEPTH_REFERENCE_OFFSET,
    THRUST_MAX_OFFSET,
    REQUEST_CONFIG,
    WORK_MODE,
    VERTICAL_MODE_TOGGLE,
    NONE
} state_commands_map;
std::map <std::string, state_commands_map> state_mapper;

uint8_t rov_armed=0;
uint8_t motors_work_mode=0;
uint8_t motor_status_counter=0;
uint8_t motor_status_counter_max=0;
uint8_t vertical_mode=0;

uint8_t controller_state=CONTROL_OFF;
Logger *logger;

void state_commands(json msg, Timer_data* data);

// Signal handler function
void signal_handler(int signal) {
    logger->log(logWARNING, "Received signal to terminate: " + std::to_string(signal));
    uv_stop(uv_default_loop());
}

int main(int argc, char* argv[]){
    bool test_mode = false;
    // Check if 'test' argument is passed
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "test") {
            test_mode = true;
            std::cout << "\n\n####### TEST MODE #######\n\n" << std::endl;
            break;
        }
    }

    uv_timer_t timer_motors;
    uv_timer_t timer_com;
    
    state_mapper["ARM_ROV"] = ARM_ROV;
    state_mapper["CHANGE_CONTROLLER_STATUS"] = CHANGE_CONTROLLER_STATUS;
    state_mapper["CHANGE_CONTROLLER_PROFILE"] = CHANGE_CONTROLLER_PROFILE;
    state_mapper["CHANGE_PITCH_STATUS"] = CHANGE_PITCH_STATUS;
    state_mapper["CHANGE_ROLL_STATUS"] = CHANGE_ROLL_STATUS;
    state_mapper["CHANGE_DEPTH_STATUS"] = CHANGE_DEPTH_STATUS;
    state_mapper["PITCH_REFERENCE_UPDATE"] = PITCH_REFERENCE_UPDATE;
    state_mapper["ROLL_REFERENCE_UPDATE"] = ROLL_REFERENCE_UPDATE;
    state_mapper["DEPTH_REFERENCE_UPDATE"] = DEPTH_REFERENCE_UPDATE;
    state_mapper["PITCH_REFERENCE_OFFSET"] = PITCH_REFERENCE_OFFSET;
    state_mapper["ROLL_REFERENCE_OFFSET"] = ROLL_REFERENCE_OFFSET;
    state_mapper["DEPTH_REFERENCE_OFFSET"] = DEPTH_REFERENCE_OFFSET;
    state_mapper["THRUST_MAX_OFFSET"] = THRUST_MAX_OFFSET;
    state_mapper["REQUEST_CONFIG"] = REQUEST_CONFIG;
    state_mapper["WORK_MODE"] = WORK_MODE;
    state_mapper["VERTICAL_MODE_TOGGLE"] = VERTICAL_MODE_TOGGLE;
    state_mapper["NONE"] = NONE;

    Logger::configLogTypeCout(true);

    Config config = Config(config_path, LOG_ALL);
    config.load_base_config();
	general_config = config.get_config(ConfigType::GENERAL);
    motors_config = config.get_config(ConfigType::MOTORS);

    Logger::configLogTypeCout(general_config["logTypeCOUT"]);
    Logger::configLogTypeFile(general_config["logTypeFILE"]);
    Logger::configLogTypeMQTT(general_config["logTypeMQTT"]);
    Logger::setLogFileDir(general_config["logFileDir"]);
    
    MQTTClient mqtt_client = MQTTClient(general_config["mqtt_server_addr"], general_config["mqtt_client_id"], 0, general_config["mqtt_loglevel"]);

    while(!mqtt_client.mqtt_connect())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    Logger::setMQTTClient(&mqtt_client);

    logger = new Logger(MAIN_LOG_NAME, general_config["main_loglevel"]);
    Nucleo nucleo = Nucleo(0, 115200, 0x01, 0x00, general_config["nucleo_loglevel"], 2, 0x04, test_mode); // true to mantain compatibility
    nucleo.init(5);


    Sensor sensor = Sensor(general_config, test_mode); 

    std::string controllerType = general_config["controller_type"]; // Read from config
    Controller* controller = nullptr;
    if (controllerType == "MIMO") {
        controller = new MIMOController(sensor, config.get_config(ConfigType::CONTROLLER)[controllerType], general_config["controller_loglevel"]);
        logger->log(logINFO,"Loaded MIMO controller");
    } else if (controllerType == "PP") {
        controller = new PPController(sensor, config.get_config(ConfigType::CONTROLLER)[controllerType], general_config["controller_loglevel"], general_config["motor_interval"]);
        logger->log(logINFO,"Loaded Pole Placement controller");
    } else {
        logger->log(logERROR,"INVALID CONTROLLER TYPE");
    }
    controller->activate(CONTROL_OFF);

    Motors motors = Motors(config.get_config(ConfigType::MOTORS), general_config["motors_loglevel"]);

    SystemMonitor system_monitor = SystemMonitor();

    Timer_data* timer_data = new Timer_data();
    timer_data->sensor = &sensor;
    timer_data->motors = &motors;
    timer_data->controller = controller;
    timer_data->nucleo = &nucleo;
    timer_data->mqtt_client = &mqtt_client;
    timer_data->config = &config;
    timer_data->system_monitor = &system_monitor;

    // Set up signal handlers for graceful termination
    std::signal(SIGINT, signal_handler);   // Ctrl+C
    std::signal(SIGTERM, signal_handler);  // kill or systemctl stop

    // Start sensor thread and save the handle
    Sensor::sensor_thread = std::thread(Sensor::update_thread, &sensor, 1);
    

    uv_loop_t* loop = uv_default_loop();

    uv_timer_init(loop, &timer_motors);
    timer_motors.data = timer_data;
    uv_timer_start(&timer_motors, timer_motors_callback, 200, general_config["motor_interval"]); // 200 ms iniziali, 10 ms di intervallo

    uv_timer_init(loop, &timer_com);
    timer_com.data = timer_data;
    uv_timer_start(&timer_com, timer_com_callback, 200, 2);

    motor_status_counter_max = (int)general_config["debug_interval"] / (int)general_config["motor_interval"];
    
    logger->log(logINFO,"Starting main loop...");
    
    // Run the event loop once
    uv_run(loop, UV_RUN_DEFAULT);
    
    // Clean shutdown
    logger->log(logINFO, "Shutting down gracefully...");
    
    // Stop timers
    uv_timer_stop(&timer_motors);
    uv_timer_stop(&timer_com);
    
    // Stop the sensor thread explicitly
    sensor.stop_thread_and_wait();
    
    // Clean up UV resources
    uv_loop_close(loop);
    
    // Clean up other resources
    delete controller;  // If you used new to create it
    delete timer_data;
    delete logger;
    
    Logger::closeLogFiles();

    return 0;
}

void timer_motors_callback(uv_timer_t* handle) {
    float* motor_thrust;
    uint16_t* motors_pwm;
    std::ostringstream logMessage;

    Timer_data* data = static_cast<Timer_data*>(handle->data);

    if(rov_armed){
        if (vertical_mode) {
            float current_roll_rad = data->sensor->get_roll() * DEGtoRAD;
            float current_pitch_rad = data->sensor->get_pitch() * DEGtoRAD;
            current_pitch_rad = data->controller->get_reference(CONTROL_PITCH) * DEGtoRAD;
            motor_thrust = data->motors->calculate_thrust_vertical(json_axes, current_roll_rad, current_pitch_rad);
        } else {
            motor_thrust = data->motors->calculate_thrust(json_axes);
        }
        if (vertical_mode>0)
            data->controller->calculate_vertical_mode(motor_thrust, json_axes);
        else
            data->controller->calculate(motor_thrust);
    }
    else
        motor_thrust = data->motors->calculate_thrust(json_axes_off);
    
    motors_pwm = data->motors->calculate_pwm();
    data->nucleo->send_pwm(motors_pwm);


    json rov_status_json;
    rov_status_json.update(data->motors->get_status());
    rov_status_json.update(data->controller->get_status());    
    rov_status_json.update(data->sensor->get_status());
    rov_status_json["AXES"] = json_axes;
    rov_status_json["rov_armed"] = (rov_armed) ? "OK" : "OFF";

    if (Logger::transformed_status_file_keys.empty()) {
        for (const auto& s : Logger::status_file_keys) {
            std::string modified = s;
            std::replace(modified.begin(), modified.end(), '.', '/');
            modified = "/" + modified;
            Logger::transformed_status_file_keys.push_back(modified);
        }
    }


    if(!rov_status_json.empty()){
        json flat_json = rov_status_json.flatten();

        logMessage << flat_json[Logger::transformed_status_file_keys[1]];
        //Starting from 1 because it's the timestamp key and it is generated inside the log method 
        for(int i=2; i<Logger::transformed_status_file_keys.size(); i++){
            logMessage << "," << flat_json[Logger::transformed_status_file_keys[i]];
        }
        logger->log(logSTATUS, logMessage.str());
    }

    motor_status_counter++;
    if(motor_status_counter==motor_status_counter_max){
        motor_status_counter=0;

        data->system_monitor->read_info();
        rov_status_json.update(data->system_monitor->get_status());
        rov_status_json["rov_armed"] = (rov_armed) ? "OK" : "OFF";
        rov_status_json["nucleo_connected"] = (data->nucleo->is_connected()) ? "OK" : "OFF";
        rov_status_json["work_mode"] = (motors_work_mode) ? "OK" : "OFF";
        rov_status_json["vertical_mode"] = vertical_mode;
        rov_status_json["torque_mode"] = (data->nucleo->is_torque_on()) ? "OK" : "OFF";

        if(!rov_status_json.empty())
            data->mqtt_client->send_msg(rov_status_json.dump(), Topic::STATUS);
    }
}

void timer_com_callback(uv_timer_t* handle){
    std::ostringstream logMessage;

    Timer_data* data = static_cast<Timer_data*>(handle->data);
    std::pair <Topic, json> msg;

    if(data->mqtt_client->receive_msg(&msg)){
        if(data->mqtt_client->is_msg_type(msg.first, Topic::AXES))
            json_axes = msg.second;
        
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::COMMANDS))
            state_commands(msg.second, data);
        
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::ARM)){
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            data->nucleo->send_arm(msg.second.begin().key());
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::CONFIG)){
            data->config->change_config(msg.second);
            data->config->write_base_config();
            general_config = data->config->get_config(ConfigType::GENERAL);
            motors_config = data->config->get_config(ConfigType::MOTORS);

            data->controller->set_parameters(general_config, data->config->get_config(ConfigType::CONTROLLER)[general_config["controller_type"]]);
            data->motors->update_parameters(general_config, data->config->get_config(ConfigType::MOTORS));
            data->mqtt_client->update_parameters(general_config);
            data->sensor->update_parameters(general_config);
        
            motors_work_mode = 0; //Since the motors values get restored to the new defaults, to avoid inconsistencies with future code.

            logger->setLogLevel(general_config["main_loglevel"]);
        }
    }

    data->nucleo->update_buffer();
}

void state_commands(json msg, Timer_data* data){
    state_commands_map cmd = NONE;
    float current_ref=0;
    uint8_t profile;
    std::ostringstream logMessage;
    json conf;
    try{
        auto it = state_mapper.find(msg.begin().key());
        if (it != state_mapper.end()) {
            cmd = it->second;
        } else {
            // Default to NONE for unknown commands
            cmd = NONE;
            logMessage << "Unknown command received: " << msg.begin().key();
            logger->log(logWARNING, logMessage.str());
        }
        switch(cmd){
            case ARM_ROV:
                rov_armed = !rov_armed;
                if(rov_armed){
                    logger->log(logINFO, "ROV ARMED");
                    data->sensor->set_pressure_baseline();
                }
                else
                    logger->log(logINFO, "ROV DISARMED");
                break;
            case CHANGE_CONTROLLER_STATUS:
                controller_state = !controller_state;
                if(controller_state){
                    data->controller->activate(general_config["controller_profile"]);
                    logger->log(logINFO, "Controller ACTIVATED with profile: " + std::to_string((int)general_config["controller_profile"]));
                }
                else{
                    data->controller->deactivate(CONTROL_ALL);
                    logger->log(logINFO, "Controller DEACTIVATED");
                }
                break;
            case CHANGE_CONTROLLER_PROFILE:
                profile = (uint8_t)msg["CHANGE_CONTROLLER_PROFILE"];
                general_config["controller_profile"] = profile;
                logMessage.str(""); logMessage.clear();
                logMessage << "Controller profile changed to: " << std::to_string(profile);
                logger->log(logINFO, logMessage.str());
                if(controller_state){
                    // Deactivate axes that are in CONTROL_ALL but not in the new profile
                    data->controller->deactivate(CONTROL_ALL & (~profile));
                    // Activate all axes specified in the new profile
                    data->controller->activate(profile);
                }
                break;
            case CHANGE_PITCH_STATUS:
                {
                    uint8_t current_profile_val = general_config["controller_profile"];
                    bool pitch_is_active = (current_profile_val & CONTROL_PITCH);

                    if (pitch_is_active) {
                        general_config["controller_profile"] = current_profile_val & ~CONTROL_PITCH;
                        if (controller_state) {
                            data->controller->deactivate(CONTROL_PITCH);
                        }
                        logger->log(logINFO, "Pitch control DEACTIVATED");
                    } else {
                        general_config["controller_profile"] = current_profile_val | CONTROL_PITCH;
                        if (controller_state) {
                            data->controller->activate(CONTROL_PITCH);
                        }
                        logger->log(logINFO, "Pitch control ACTIVATED");
                    }
                }
                break;
            case CHANGE_ROLL_STATUS:
                {
                    uint8_t current_profile_val = general_config["controller_profile"];
                    bool roll_is_active = (current_profile_val & CONTROL_ROLL);

                    if (roll_is_active) {
                        general_config["controller_profile"] = current_profile_val & ~CONTROL_ROLL;
                        if (controller_state) {
                            data->controller->deactivate(CONTROL_ROLL);
                        }
                        logger->log(logINFO, "Roll control DEACTIVATED");
                    } else {
                        general_config["controller_profile"] = current_profile_val | CONTROL_ROLL;
                        if (controller_state) {
                            data->controller->activate(CONTROL_ROLL);
                        }
                        logger->log(logINFO, "Roll control ACTIVATED");
                    }
                }
                break;
            case CHANGE_DEPTH_STATUS:
                {
                    uint8_t current_profile_val = general_config["controller_profile"];
                    bool depth_is_active = (current_profile_val & CONTROL_Z);

                    if (depth_is_active) {
                        general_config["controller_profile"] = current_profile_val & ~CONTROL_Z;
                        if (controller_state) {
                            data->controller->deactivate(CONTROL_Z);
                        }
                        logger->log(logINFO, "Depth control DEACTIVATED");
                    } else {
                        general_config["controller_profile"] = current_profile_val | CONTROL_Z;
                        if (controller_state) {
                            data->controller->activate(CONTROL_Z);
                        }
                        logger->log(logINFO, "Depth control ACTIVATED");
                    }
                }
                break;
            case PITCH_REFERENCE_UPDATE:
                data->controller->set_reference(CONTROL_PITCH, msg["PITCH_REFERENCE_UPDATE"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Pitch reference updated to: " << msg["PITCH_REFERENCE_UPDATE"].dump();
                logger->log(logINFO, logMessage.str());
                break;
            case ROLL_REFERENCE_UPDATE:
                data->controller->set_reference(CONTROL_ROLL, msg["ROLL_REFERENCE_UPDATE"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Roll reference updated to: " << msg["ROLL_REFERENCE_UPDATE"].dump();
                logger->log(logINFO, logMessage.str());
                break;
            case DEPTH_REFERENCE_UPDATE:
                data->controller->set_reference(CONTROL_Z, msg["DEPTH_REFERENCE_UPDATE"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Depth reference updated to: " << msg["DEPTH_REFERENCE_UPDATE"].dump();
                logger->log(logINFO, logMessage.str());
                break;
            case PITCH_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_PITCH);
                data->controller->set_reference(CONTROL_PITCH, current_ref + (float)msg["PITCH_REFERENCE_OFFSET"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Pitch reference offset by: " << msg["PITCH_REFERENCE_OFFSET"].dump() << ". New reference: " << data->controller->get_reference(CONTROL_PITCH);
                logger->log(logINFO, logMessage.str());
                break;
            case ROLL_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_ROLL);
                data->controller->set_reference(CONTROL_ROLL, current_ref + (float)msg["ROLL_REFERENCE_OFFSET"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Roll reference offset by: " << msg["ROLL_REFERENCE_OFFSET"].dump() << ". New reference: " << data->controller->get_reference(CONTROL_ROLL);
                logger->log(logINFO, logMessage.str());
                break;
            case DEPTH_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_Z);
                data->controller->set_reference(CONTROL_Z, current_ref + (float)msg["DEPTH_REFERENCE_OFFSET"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Depth reference offset by: " << msg["DEPTH_REFERENCE_OFFSET"].dump() << ". New reference: " << data->controller->get_reference(CONTROL_Z);
                logger->log(logINFO, logMessage.str());
                break;
            case THRUST_MAX_OFFSET:
                data->motors->offset_thrust_max(msg["THRUST_MAX_OFFSET"]);
                logMessage.str(""); logMessage.clear();
                logMessage << "Max thrust offset by: " << msg["THRUST_MAX_OFFSET"].dump();
                logger->log(logINFO, logMessage.str());
                break;
            case REQUEST_CONFIG:
                conf = data->config->get_config(ConfigType::ALL);
                data->mqtt_client->send_msg(conf.dump(), Topic::CONFIG);
                logger->log(logINFO, "Configuration requested and sent.");
                break;
            case WORK_MODE:
                motors_work_mode =! motors_work_mode;

                if(motors_work_mode){
                    data->motors->set_thrust_max(motors_config["thrust_max_xy_work"], motors_config["thrust_max_z_work"]);
                    logger->log(logINFO, "Work mode ACTIVATED. Max thrust set to work values.");
                }
                else{
                    data->motors->set_thrust_max(motors_config["thrust_max_xy"], motors_config["thrust_max_z"]);
                    logger->log(logINFO, "Work mode DEACTIVATED. Max thrust set to default values.");
                }
                break;
            case VERTICAL_MODE_TOGGLE:
                vertical_mode++;
                vertical_mode %= 3;
                if(vertical_mode==1){
                    logger->log(logINFO, "Vertical mode UP ACTIVATED.");
                    data->controller->set_reference(CONTROL_PITCH, 90.0);
                }
                else if(vertical_mode==2){
                    logger->log(logINFO, "Vertical mode DOWN ACTIVATED.");
                    data->controller->set_reference(CONTROL_PITCH, -90.0);
                }
                else{
                    logger->log(logINFO, "Vertical mode DEACTIVATED.");
                    data->controller->set_reference(CONTROL_PITCH, 0.0);
                }
                break;
            case NONE:
                break;
        }
    } catch (const json::exception& e) {
        logMessage << "JSON error parsing state_commands: " << e.what();
        logger->log(logERROR, logMessage.str());
    }
}
