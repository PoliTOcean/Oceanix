#include "mqtt_client.hpp"

std::map <Topic, std::string> topic_map {
    {Topic::AXES, "axes/"},
    {Topic::COMMANDS, "state_commands/"},
    {Topic::ARM, "arm_commands/"},
    {Topic::CONFIG, "config/"},
    {Topic::DEBUG, "debug/"},
    {Topic::LOG, "log/"},

};

std::map <std::string, Topic> inverse_topic_map {
    {"axes/", Topic::AXES},
    {"state_commands/", Topic::COMMANDS},
    {"arm_commands/", Topic::ARM},
    {"config/", Topic::CONFIG},
    {"debug/", Topic::DEBUG},
    {"log/", Topic::LOG},
};

MQTTClient::MQTTClient(std::string server_address, std::string client_id, int QOS, logLevel minimumLoglevel):
    cli(server_address, client_id),
    logger(Logger(MQTT_LOG_NAME, minimumLoglevel)) {
    m_server_address = server_address;
    m_client_id = client_id;
    m_QOS = QOS;

    m_conn_opts = mqtt::connect_options_builder()
        .clean_session(true)
        .finalize();
}

bool MQTTClient::is_connected() { return cli.is_connected(); }

bool MQTTClient::mqtt_connect() {
    std::ostringstream logMessage;

    if (cli.is_connected()) return true;
        
    try {
        cli.start_consuming();

        logger.log(logINFO, "Connecting to the MQTT server...");
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
            cli.subscribe(topic_map[Topic::LOG], m_QOS)->wait();
        }
        logger.log(logINFO, "Connection established");
    }
    
    catch (const mqtt::exception& exc) {
        logMessage << "Error connecting and subscribing to MQTT Broker: " << exc;
        logger.log(logERROR, logMessage.str());
        return false;
    }
	return true;    
}

void MQTTClient::mqtt_disconnect() {
    if (cli.is_connected()) {
        logger.log(logINFO, "Shutting down and disconnecting from the MQTT server");
        cli.unsubscribe(topic_map[Topic::AXES])->wait();
        cli.unsubscribe(topic_map[Topic::COMMANDS])->wait();
        cli.unsubscribe(topic_map[Topic::ARM])->wait();
        cli.unsubscribe(topic_map[Topic::CONFIG])->wait();
        cli.unsubscribe(topic_map[Topic::DEBUG])->wait();
        cli.unsubscribe(topic_map[Topic::LOG])->wait();

        
        cli.stop_consuming();
		cli.disconnect()->wait();
	}
    else
        logger.log(logINFO, "Client is already disconnected");
}

bool MQTTClient::mqtt_reconnect() {
    mqtt_disconnect();
    logger.log(logINFO, "Reconnecting MQTT Client...");

    return mqtt_connect();
}                                                                                            

bool MQTTClient::send_msg(std::string msg, Topic topic) {
    std::ostringstream logMessage;
    
    if (cli.is_connected()) {
        cli.publish(topic_map[topic], msg);
        if(m_verbose){
            logMessage << "MQTT send msg: " << msg << "  to topic: " << topic_map[topic] << std::endl;
            logger.log(logINFO, logMessage.str());
        }
        return true;
    }
    else {
        logger.log(logWARNING, "Client is not connected");
        return false;
    }
}

bool MQTTClient::receive_msg(std::pair <Topic, json>* msgp ) { 
    int code;
    std::ostringstream logMessage;

    if (cli.try_consume_message(&m_msg)) {
        if (isJsonParseable(m_msg->get_payload())){
            msgp->first = inverse_topic_map[m_msg->get_topic()];
            msgp->second = json::parse(m_msg->get_payload());
            if(m_verbose){
                logMessage << "received msg: " << m_msg->get_payload() << "  from topic: " << m_msg->get_topic() << std::endl;
                logger.log(logINFO, logMessage.str());
            }
        }
        else{
            logger.log(logWARNING, "Message parsing error");
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