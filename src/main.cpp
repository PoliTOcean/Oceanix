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
#include <json.hpp>
#include "sensor.hpp"
#include "controller.hpp"
#include "motors.hpp"
#include "mqtt_client.hpp"
#include "nucleo.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include <iostream>

using json = nlohmann::json;

void timer_motors_callback(uv_timer_t* handle);
void timer_com_callback(uv_timer_t* handle);
void timer_debug_callback(uv_timer_t* handle);

struct Timer_data {
    Sensor* sensor;
    Motors* motors;
    Controller* controller;
    Nucleo* nucleo;
    MQTTClient* mqtt_client;
};

const std::string config_path = "../config/config.json";

json general_config;
json json_axes_off = {
    {"X", 0},
    {"Y", 0},
    {"Z", 0},
    {"YAW", 0}
};
json json_axes = {
    {"X", 0},
    {"Y", 0},
    {"Z", 0},
    {"YAW", 0}
};

typedef enum {
    ARM_ROV,
    CHANGE_CONTROLLER_STATUS,
    PITCH_REFERENCE_UPDATE,
    ROLL_REFERENCE_UPDATE,
    PITCH_REFERENCE_OFFSET,
    ROLL_REFERENCE_OFFSET,
    THRUST_MAX_OFFSET,
    NONE
} state_commands_map;
std::map <std::string, state_commands_map> state_mapper;

uint8_t rov_armed=0;
uint8_t controller_state=CONTROL_OFF;
Logger logger("MAIN   ", LOG_ALL);

void state_commands(json msg, Timer_data* data);

int main(){
    int sensor_status;
    uv_timer_t timer_motors;
    uv_timer_t timer_com;
    uv_timer_t timer_debug;

    //Now it defaults to all, but the config should be read from a file in the future
    Logger::configLogType(LOG_TYPE_COUT | LOG_TYPE_FILE | LOG_TYPE_MQTT);

    state_mapper["ARM_ROV"] = ARM_ROV;
    state_mapper["CHANGE_CONTROLLER_STATUS"] = CHANGE_CONTROLLER_STATUS;
    state_mapper["PITCH_REFERENCE_UPDATE"] = PITCH_REFERENCE_UPDATE;
    state_mapper["ROLL_REFERENCE_UPDATE"] = ROLL_REFERENCE_UPDATE;
    state_mapper["PITCH_REFERENCE_OFFSET"] = PITCH_REFERENCE_OFFSET;
    state_mapper["ROLL_REFERENCE_OFFSET"] = ROLL_REFERENCE_OFFSET;
    state_mapper["THRUST_MAX_OFFSET"] = THRUST_MAX_OFFSET;
    state_mapper["NONE"] = NONE;

    Config config = Config(config_path, LOG_ALL);
    config.load_base_config();
	general_config = config.get_config(ConfigType::GENERAL);

    MQTTClient mqtt_client = MQTTClient(general_config["mqtt_server_addr"], general_config["mqtt_client_id"], 0, general_config["verbose_mqtt"]);
	while(!mqtt_client.mqtt_connect())
        sleep(1);

    Nucleo nucleo = Nucleo(0, 115200, 0x01, 0x00, general_config["verbose_nucleo"]);
    while (nucleo.init(0x00) != COMM_STATUS::OK) {
        nucleo.connect();
        logger.log(logERROR,"INIT FAILED");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    logger.log(logINFO,"INIT SUCCESS");

    Sensor sensor = Sensor();

    Controller controller = Controller(sensor, config.get_config(ConfigType::CONTROLLER), general_config["verbose_controller"]);

    Motors motors = Motors(config.get_config(ConfigType::MOTORS), LOG_ALL);

    Timer_data* timer_data = new Timer_data();
    timer_data->sensor = &sensor;
    timer_data->motors = &motors;
    timer_data->controller = &controller;
    timer_data->nucleo = &nucleo;
    timer_data->mqtt_client = &mqtt_client;

    uv_loop_t* loop = uv_default_loop();

    uv_timer_init(loop, &timer_motors);
    timer_motors.data = timer_data;
    uv_timer_start(&timer_motors, timer_motors_callback, 200, 10); // 10 ms iniziali, 10 ms di intervallo

    uv_timer_init(loop, &timer_com);
    timer_com.data = timer_data;
    uv_timer_start(&timer_com, timer_com_callback, 200, 2);

    uv_timer_init(loop, &timer_debug);
    timer_debug.data = timer_data;
    uv_timer_start(&timer_debug, timer_debug_callback, general_config["debug_interval"], general_config["debug_interval"]);

    controller.activate(CONTROL_OFF);
    
    uv_run(loop, UV_RUN_DEFAULT);

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
            data->nucleo->send_arm(msg.second["command"]);
        else if(data->mqtt_client->is_msg_type(msg.first, Topic::CONFIG)){
            logMessage << "config message: " << msg.second.dump();
            logger.log(logINFO, logMessage.str());
        }
    }
    data->nucleo->update_buffer();
    data->nucleo->get_heartbeat();
}

void timer_debug_callback(uv_timer_t* handle){
    Timer_data* data = static_cast<Timer_data*>(handle->data);
    json json_debug;

    if(general_config["debug_motor"])
        data->motors->update_debug(json_debug);
    if(general_config["debug_controller"])
        data->controller->update_debug(json_debug);
    if(general_config["debug_general"]){
        json_debug["rov_armed"] = (rov_armed) ? "OK" : "OFF";
        data->sensor->update_debug(json_debug);
    }

    if(!json_debug.empty())
        data->mqtt_client->send_debug(json_debug);
        
    if(!data->nucleo->is_connected())
        logger.log(logINFO,"NUCLEO disconnected");
}

void state_commands(json msg, Timer_data* data){
    state_commands_map cmd = NONE;
    float current_ref=0;
    std::ostringstream logMessage;
    try{
        cmd = state_mapper[msg.begin().key()];
        switch(cmd){
            case ARM_ROV:
                rov_armed = !rov_armed;
                if(rov_armed){
                    logger.log(logINFO, "ROV ARMED");
                    data->sensor->set_pressure_baseline();
                }
                else
                    logger.log(logINFO, "ROV DISARMED");
                break;
            case CHANGE_CONTROLLER_STATUS:
                controller_state = !controller_state;
                if(controller_state){
                    //data->controller->activate(CONTROL_Z);
                    data->controller->activate(general_config["controller_profile"]);
                }
                else
                    data->controller->disactivate(CONTROL_ALL);
                break;
            case PITCH_REFERENCE_UPDATE:
                data->controller->change_reference(CONTROL_PITCH, msg["PITCH_REFERENCE_UPDATE"]);
                break;
            case ROLL_REFERENCE_UPDATE:
                data->controller->change_reference(CONTROL_ROLL, msg["ROLL_REFERENCE_UPDATE"]);
                break;
            case PITCH_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_PITCH);
                data->controller->change_reference(CONTROL_PITCH, current_ref + (float)msg["PITCH_REFERENCE_OFFSET"]);
                break;
            case ROLL_REFERENCE_OFFSET:
                current_ref = data->controller->get_reference(CONTROL_ROLL);
                data->controller->change_reference(CONTROL_ROLL, current_ref + (float)msg["ROLL_REFERENCE_OFFSET"]);
                break;
            case THRUST_MAX_OFFSET:
                data->motors->offset_thrust_max(msg["THRUST_MAX_OFFSET"]);
                break;
            case NONE:
                break;
        }
    } catch (const json::exception& e) {
        logMessage << "JSON error parsing state_commands: " << e.what();
        logger.log(logERROR, logMessage.str());
    }
}