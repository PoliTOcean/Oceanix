#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <stdlib.h>
#include <iostream>
#include "json.hpp"
#include "mqtt/async_client.h"
#include "utils.hpp"
#include "logger.hpp"

using msg_pt = std::shared_ptr<const mqtt::message>;
using json = nlohmann::json;

enum class Topic { AXES, COMMANDS, ARM, CONFIG, STATUS, LOG};

class MQTTClient{
    public:
        MQTTClient(std::string server_address, std::string client_id, int QOS, logLevel minimumLoglevel);

        bool is_connected();
        bool mqtt_connect();
        void mqtt_disconnect();
        bool mqtt_reconnect();
        bool send_msg(std::string msg, Topic topic);
        bool receive_msg(std::pair <Topic, json>* msgp);
        bool is_msg_type(Topic topic2, Topic topic1);

        void update_parameters(const json& general_config);

    private:
        std::string m_server_address;
        std::string m_client_id;
        std::any m_conn_opts;
        mqtt::async_client cli;
        int m_QOS;

        msg_pt m_msg;

        Logger logger;
};

#endif // MQTTCLIENT_H
