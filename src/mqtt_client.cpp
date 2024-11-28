#include "mqtt_client.hpp"

std::map <Topic, std::string> topic_map {
    {Topic::AXES, "axes/"},
    {Topic::COMMANDS, "state_commands/"},
    {Topic::ARM, "arm_commands/"},
    {Topic::CONFIG, "config/"},
    {Topic::DEBUG, "debug/"},
};

std::map <std::string, Topic> inverse_topic_map {
    {"axes/", Topic::AXES},
    {"state_commands/", Topic::COMMANDS},
    {"arm_commands/", Topic::ARM},
    {"config/", Topic::CONFIG},
    {"debug/", Topic::DEBUG},
};

MQTTClient::MQTTClient(std::string server_address, std::string client_id, int QOS, bool verbose):
    cli(server_address, client_id) {
    m_server_address = server_address;
    m_client_id = client_id;
    m_QOS = QOS;
    m_verbose = verbose;

    m_conn_opts = mqtt::connect_options_builder()
        .clean_session(true)
        .finalize();
}

bool MQTTClient::is_connected() { return cli.is_connected(); }

bool MQTTClient::mqtt_connect() {
    if (cli.is_connected()) return true;
        
    try {
        cli.start_consuming();

        std::cout << "[MQTT - INFO] Connecting to the MQTT server..." << std::endl;
        //auto tok = cli.connect(m_conn_opts);
        auto tok = cli.connect();

        // Getting the connect response will block waiting for the connection to complete.
        auto rsp = tok->get_connect_response();

        // If there is no session present, then we need to subscribe, but if
        // there is a session, then the server remembers us and our subscriptions.
        if (!rsp.is_session_present()) {
            cli.subscribe(topic_map[Topic::AXES], m_QOS)->wait();
            cli.subscribe(topic_map[Topic::COMMANDS], m_QOS)->wait();
            cli.subscribe(topic_map[Topic::ARM], m_QOS)->wait();
            cli.subscribe(topic_map[Topic::CONFIG], m_QOS)->wait();
            //cli.subscribe(topic_map[Topic::DEBUG], m_QOS)->wait();
        }
        std::cout << "[MQTT - INFO] Connection established" << std::endl;
    }
    
    catch (const mqtt::exception& exc) {
        std::cerr << "[MQTT - ERROR] Error connecting and subscribing to MQTT Broker" << exc << std::endl;
        return false;
    }
	return true;    
}

void MQTTClient::mqtt_disconnect() {
    if (cli.is_connected()) {
		std::cout << "[MQTT - INFO] Shutting down and disconnecting from the MQTT server" << std::endl;
        cli.unsubscribe(topic_map[Topic::AXES])->wait();
        cli.unsubscribe(topic_map[Topic::COMMANDS])->wait();
        cli.unsubscribe(topic_map[Topic::ARM])->wait();
        cli.unsubscribe(topic_map[Topic::CONFIG])->wait();
        cli.unsubscribe(topic_map[Topic::DEBUG])->wait();
        
        cli.stop_consuming();
		cli.disconnect()->wait();
	}
    else
	    std::cout << "[MQTT - WARNING] Client is already disconnected" << std::endl;
}

bool MQTTClient::mqtt_reconnect() {
    mqtt_disconnect();
    std::cout << "[MQTT - INFO] Reconnecting MQTT Client..." << std::endl; 

    return mqtt_connect();
}                                                                                            

bool MQTTClient::send_msg(std::string msg, Topic topic) {
    if (cli.is_connected()) {
        cli.publish(topic_map[topic], msg);
        if(m_verbose)
            std::cout << "MQTT send msg: " << msg << "  to topic: " << topic_map[topic] << std::endl;
        return true;
    }
    else {
        std::cout << "[MQTT - WARNING] Client is not conneted" << std::endl;
        return false;
    }
}

bool MQTTClient::receive_msg(std::pair <Topic, json>* msgp ) { 
    int code;

    if (cli.try_consume_message(&m_msg)) {
        if (isJsonParseable(m_msg->get_payload())){
            msgp->first = inverse_topic_map[m_msg->get_topic()];
            msgp->second = json::parse(m_msg->get_payload());
            if(m_verbose)
                std::cout << "MQTT received msg: " << m_msg->get_payload() << "  from topic: " << m_msg->get_topic() << std::endl;
        }
        else{
            std::cout << "[MQTT - WARNING] Message parsing error" << std::endl; 
            return false;
        }
        return true;
    }

    return false;
}

bool MQTTClient::send_debug(json debug_json) { return send_msg(debug_json.dump(), Topic::DEBUG); }

bool MQTTClient::is_msg_type(Topic topic2, Topic topic1) { 
    return topic_map[topic1] == topic_map[topic2];
}
