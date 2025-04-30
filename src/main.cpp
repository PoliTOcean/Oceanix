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
#include <pigpio.h>
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

#define RST_PIN 17

using json = nlohmann::json;

void timer_motors_callback(uv_timer_t* handle);
void timer_com_callback(uv_timer_t* handle);
void timer_status_callback(uv_timer_t* handle);

struct Timer_data {
    Sensor* sensor;
    Motors* motors;
    Controller* controller;
    Nucleo* nucleo;
    MQTTClient* mqtt_client;
    Config* config;
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
    PITCH_REFERENCE_UPDATE,
    ROLL_REFERENCE_UPDATE,
    DEPTH_REFERENCE_UPDATE,
    PITCH_REFERENCE_OFFSET,
    ROLL_REFERENCE_OFFSET,
    DEPTH_REFERENCE_OFFSET,
    THRUST_MAX_OFFSET,
    REQUEST_CONFIG,
    WORK_MODE,
    NONE
} state_commands_map;
std::map <std::string, state_commands_map> state_mapper;

uint8_t rov_armed=0;
uint8_t nucleo_connected=0;
uint8_t motors_work_mode=0;
uint8_t status_callback=0;

uint8_t controller_state=CONTROL_OFF;
Logger *logger;

void state_commands(json msg, Timer_data* data);

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
    uv_timer_t timer_status;

    //Now it defaults to all, but the config should be read from a file in the future
    
    state_mapper["ARM_ROV"] = ARM_ROV;
    state_mapper["CHANGE_CONTROLLER_STATUS"] = CHANGE_CONTROLLER_STATUS;
    state_mapper["PITCH_REFERENCE_UPDATE"] = PITCH_REFERENCE_UPDATE;
    state_mapper["ROLL_REFERENCE_UPDATE"] = ROLL_REFERENCE_UPDATE;
    state_mapper["DEPTH_REFERENCE_UPDATE"] = DEPTH_REFERENCE_UPDATE;
    state_mapper["PITCH_REFERENCE_OFFSET"] = PITCH_REFERENCE_OFFSET;
    state_mapper["ROLL_REFERENCE_OFFSET"] = ROLL_REFERENCE_OFFSET;
    state_mapper["DEPTH_REFERENCE_OFFSET"] = DEPTH_REFERENCE_OFFSET;
    state_mapper["THRUST_MAX_OFFSET"] = THRUST_MAX_OFFSET;
    state_mapper["REQUEST_CONFIG"] = REQUEST_CONFIG;
    state_mapper["WORK_MODE"] = WORK_MODE;
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
    Nucleo nucleo = Nucleo(0, 115200, 0x01, 0x00, general_config["nucleo_debug"], test_mode); // true to mantain compatibility

    if(gpioInitialise()<0)
        logger->log(logERROR, "FAILED GPIO INIT");

    for(int i=0; i<5; i++){
        gpioSetMode(RST_PIN, PI_INPUT);
        gpioSetPullUpDown(RST_PIN, PI_PUD_UP);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        nucleo_connected = (nucleo.init(0x04) == COMM_STATUS::OK);
        if(nucleo_connected){
            logger->log(logINFO,"NUCLEO INIT SUCCESS");
            break; //Exits from the for cycle
        }
        gpioSetMode(RST_PIN, PI_OUTPUT);
        gpioWrite(RST_PIN, 0);

        logger->log(logERROR,"NUCLEO INIT FAILED");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    Sensor sensor = Sensor(general_config["Zspeed_alpha"], general_config["Zspeed_beta"], general_config["imu_loglevel"], general_config["bar02_loglevel"], test_mode); 

    std::string controllerType = general_config["controller_type"]; // Read from config
    Controller* controller = nullptr;
    if (controllerType == "MIMO") {
        controller = new MIMOController(sensor, config.get_config(ConfigType::CONTROLLER), general_config["controller_loglevel"]);
    } else if (controllerType == "PP") {
        controller = new PPController(sensor, config.get_config(ConfigType::CONTROLLER), general_config["controller_loglevel"]);
    } else {
        logger->log(logERROR,"INVALID CONTROLLER TYPE");
    }

    Motors motors = Motors(config.get_config(ConfigType::MOTORS), general_config["motors_loglevel"]);

    Timer_data* timer_data = new Timer_data();
    timer_data->sensor = &sensor;
    timer_data->motors = &motors;
    timer_data->controller = &controller;
    timer_data->nucleo = &nucleo;
    timer_data->mqtt_client = &mqtt_client;
    timer_data->config = &config;

    uv_loop_t* loop = uv_default_loop();

    uv_timer_init(loop, &timer_motors);
    timer_motors.data = timer_data;
    uv_timer_start(&timer_motors, timer_motors_callback, 200, general_config["motor_interval"]); // 10 ms iniziali, 10 ms di intervallo

    uv_timer_init(loop, &timer_com);
    timer_com.data = timer_data;
    uv_timer_start(&timer_com, timer_com_callback, 200, 2);

    // uv_timer_init(loop, &timer_status);
    // timer_status.data = timer_data;
    // uv_timer_start(&timer_status, timer_status_callback, general_config["debug_interval"], general_config["debug_interval"]);

    controller.activate(CONTROL_OFF);
    
    uv_run(loop, UV_RUN_DEFAULT);

    Logger::closeLogFiles(); //Not sure this should be placed here

    return 0;
}

void timer_motors_callback(uv_timer_t* handle) {
    float* motor_thrust;
    uint16_t* motor_pwm;

    Timer_data* data = static_cast<Timer_data*>(handle->data);

    if(rov_armed){
        motor_thrust = data->motors->calculate_thrust(json_axes);
        data->sensor->read_sensor();
        data->controller->calculate(motor_thrust);
    }
    else
        motor_thrust = data->motors->calculate_thrust(json_axes_off);
    
    motor_pwm = data->motors->calculate_pwm();

    data->nucleo->send_pwm(motor_pwm);


    json rov_status_json;
    rov_status_json.update(data->motors->get_status());
    rov_status_json.update(data->controller->get_status());    
    rov_status_json.update(data->sensor->get_status());
    rov_status_json["AXES"] = json_axes;
    rov_status_json["rov_armed"] = (rov_armed) ? "OK" : "OFF";

    if(!rov_status_json.empty())
        logger->log(logSTATUS, rov_status_json.dump());

    status_callback++;
    if(status_callback==5){
        status_callback=0;
        rov_status_json["rov_armed"] = (rov_armed) ? "OK" : "OFF";
        rov_status_json["nucleo_connected"] = (nucleo_connected) ? "OK" : "OFF";

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
        
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::ARM))
            data->nucleo->send_arm(msg.second.begin().key());
        
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::CONFIG)){
            logMessage << "config message: " << msg.second.dump();
            logger->log(logINFO, logMessage.str());
            
            data->config->change_config(msg.second);
            data->config->write_base_config();
            general_config = data->config->get_config(ConfigType::GENERAL);
            motors_config = data->config->get_config(ConfigType::MOTORS);

            data->controller->update_parameters(general_config, data->config->get_config(ConfigType::CONTROLLER));
            data->motors->update_parameters(general_config, data->config->get_config(ConfigType::MOTORS));
            data->mqtt_client->update_parameters(general_config);
            data->sensor->update_parameters(general_config);
        
            motors_work_mode = 0; //Since the motors values get restored to the new defaults, to avoid inconsistencies with future code.

            logger->setLogLevel(general_config["main_loglevel"]);
        }
    }

    data->nucleo->update_buffer();
    data->nucleo->get_heartbeat();
}

// void timer_status_callback(uv_timer_t* handle){
//     Timer_data* data = static_cast<Timer_data*>(handle->data);
//     json rov_status_json;

//     rov_status_json.update(data->motors->get_status());
//     rov_status_json.update(data->controller->get_status());    
//     rov_status_json.update(data->sensor->get_status());

//     rov_status_json["rov_armed"] = (rov_armed) ? "OK" : "OFF";
//     rov_status_json["nucleo_connected"] = (nucleo_connected) ? "OK" : "OFF";
    

//     if(!rov_status_json.empty())
//         //logger->log(logSTATUS, rov_status_json.dump());
//         data->mqtt_client->send_msg(rov_status_json.dump(), Topic::STATUS);
        
//     if(!data->nucleo->is_connected()){
//         logger->log(logINFO,"NUCLEO disconnected");
//         nucleo_connected = 0;
//         if(data->nucleo->init(0x04) == COMM_STATUS::OK){ //We dont track if the init was succesful, we simply check the current connection and initialize the nucleo again.
//             nucleo_connected = 1;
//             logger->log(logINFO,"NUCLEO connected");
//         }
//     }
// }

void state_commands(json msg, Timer_data* data){
    state_commands_map cmd = NONE;
    float current_ref=0;
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
                if(controller_state)
                    data->controller->activate(general_config["controller_profile"]);
                else
                    data->controller->deactivate(CONTROL_ALL);
                break;
            case PITCH_REFERENCE_UPDATE:
                data->controller->change_reference(CONTROL_PITCH, msg["PITCH_REFERENCE_UPDATE"]);
                break;
            case ROLL_REFERENCE_UPDATE:
                data->controller->change_reference(CONTROL_ROLL, msg["ROLL_REFERENCE_UPDATE"]);
                break;
            case DEPTH_REFERENCE_UPDATE:
                data->controller->change_reference(CONTROL_Z, msg["DEPTH_REFERENCE_UPDATE"]);
                break;
            case PITCH_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_PITCH);
                data->controller->change_reference(CONTROL_PITCH, current_ref + (float)msg["PITCH_REFERENCE_OFFSET"]);
                break;
            case ROLL_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_ROLL);
                data->controller->change_reference(CONTROL_ROLL, current_ref + (float)msg["ROLL_REFERENCE_OFFSET"]);
                break;
            case DEPTH_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_Z);
                data->controller->change_reference(CONTROL_Z, current_ref + (float)msg["DEPTH_REFERENCE_OFFSET"]);
                break;
            case THRUST_MAX_OFFSET:
                data->motors->offset_thrust_max(msg["THRUST_MAX_OFFSET"]);
                break;
            case REQUEST_CONFIG:
                conf = data->config->get_config(ConfigType::ALL);
                data->mqtt_client->send_msg(conf.dump(), Topic::CONFIG);
                break;
            case WORK_MODE:
                motors_work_mode =! motors_work_mode;

                if(motors_work_mode){
                    data->motors->set_thrust_max(motors_config["thrust_max_xy_work"], motors_config["thrust_max_z_work"]);
                }
                else{
                    data->motors->set_thrust_max(motors_config["thrust_max_xy"], motors_config["thrust_max_z"]);
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
