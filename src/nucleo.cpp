#include "nucleo.hpp"

Nucleo::Nucleo(uint8_t address, int baudrate, uint8_t version, uint8_t sub_version, logLevel minimumLoglevel, int64_t heartbeat_interval, int64_t starting_frequency, bool test_mode)
    : m_protocol(version, sub_version, address, baudrate, !minimumLoglevel), m_test_mode(test_mode), m_torque_state(false), last_hb_timestamp(0), heartbeat_interval(heartbeat_interval), starting_frequency(starting_frequency),
    logger(Logger(NUCLEO_LOG_NAME, minimumLoglevel)) {
    m_init_mapper();
}

/**
* is_connected - Check if the Raspberry is connected to the Nucleo.
* 
* @return: true if is connected, false otherwise.
*/
bool Nucleo::is_connected() {
    if (m_test_mode) {
        return true; // Simulate always connected in test mode
    }

    if(!m_protocol.is_connected()){
        disconnect();
        logger.log(logERROR, "The serial port is closed");
        return false;
    }

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto new_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    packet_t heartbeat_packet = get_heartbeat();
    bool connected;

    if(heartbeat_packet.first == COMM_STATUS::OK){
        connected = true;
        logger.log(logINFO, "Heartbeat received with status: OK");
    	last_hb_timestamp = new_timestamp;
    }
    else if(heartbeat_packet.first != COMM_STATUS::SERIAL_NOT_IN_BUFFER){
        logger.log(logINFO, "Heartbeat received with a status different from OK");
    }
    if((new_timestamp - last_hb_timestamp) > (heartbeat_interval*4)){
        logger.log(logINFO,"disconnected");
        connected = false;
    }
    else connected = true; 
        
    if(connected == false) init(5);

    return connected;
}

// bool Nucleo::connect() {
//     if (m_test_mode) {
//         return true; // Simulate successful connection in test mode
//     }
//     return m_protocol.connect();
// }

COMM_STATUS Nucleo::init(int n_tries) {
    if (m_test_mode) {
        return COMM_STATUS::OK; // Simulate successful initialization in test mode
    }

    COMM_STATUS init_status;
    for(int i=0; i<n_tries; i++){
        m_protocol.connect();
        init_status = (m_protocol.init(starting_frequency));
        if(init_status == COMM_STATUS::OK){
            const auto now = std::chrono::system_clock::now().time_since_epoch();
            last_hb_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
            logger.log(logINFO,"INIT SUCCESS");
            break; //Exits from the for cycle
        }
        logger.log(logERROR,"INIT FAILED");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return init_status;
}

void Nucleo::disconnect() {
    if (m_test_mode) {
        // Simulate disconnection in test mode
        return;
    }
    m_protocol.disconnect();
}

std::pair<COMM_STATUS, std::optional<std::vector<uint8_t>>> Nucleo::get_heartbeat() {
    if (m_test_mode) {
        // Simulate a heartbeat response in test mode
        std::vector<uint8_t> simulated_data = {0x00, 0x01, 0x00, 0x00}; // Example data
        return {COMM_STATUS::OK, simulated_data};
    }
    return m_protocol.get_heartbeat();
}

void Nucleo::update_buffer() {
    if (m_test_mode) {
        // Simulate buffer update in test mode
        return;
    }
    m_protocol.update_buffer();
}

ssize_t Nucleo::send_pwm(uint16_t* pwm_values) {
    if (m_test_mode) {
        // Simulate sending PWM values in test mode
        return sizeof(pwm_values);
    }
    return m_protocol.send_packet(0, pwm_values, motorN);
}

ssize_t Nucleo::send_arm(std::string arm_value) {
    // check for commans not in the mapper
    if (arm_value == "TORQUE_WRIST_TOGGLE")
        arm_value = m_torque_state ? "TORQUE_WRIST_OFF" : "TORQUE_WRIST_ON";

    // Check if the arm_value is valid
    if (m_arm_value_mapper.find(arm_value) == m_arm_value_mapper.end()) {
        logger.log(logERROR, "Invalid arm value: " + arm_value);
        return -1; // Invalid arm value
    }
    // Check if torque commands are being used
    if (arm_value == "TORQUE_WRIST_ON") {
        m_torque_state = true;
        // If torque is turned on, we need to send a command to close the nipper
        m_arm_value[0] = m_arm_value_mapper["CLOSE_NIPPER"];
        m_protocol.send_packet(1, m_arm_value, arm_package_size);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    } else if (arm_value == "TORQUE_WRIST_OFF") {
        m_torque_state = false;
        // If torque is turned off, we need to send a command to stop the nipper
        m_arm_value[0] = m_arm_value_mapper["STOP_NIPPER"];
        m_protocol.send_packet(1, m_arm_value, arm_package_size);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    if (m_test_mode) {
        return arm_package_size;
    }

    m_arm_value[0] = m_arm_value_mapper[arm_value];
    return m_protocol.send_packet(1, m_arm_value, arm_package_size);
}

bool Nucleo::is_torque_on() {
    return m_torque_state;
}

void Nucleo::m_init_mapper() {
    m_arm_value_mapper["ROTATE_WRIST_CCW"] = 0;
    m_arm_value_mapper["ROTATE_WRIST_CW"] = 1;
    m_arm_value_mapper["STOP_WRIST"] = 2;
    m_arm_value_mapper["OPEN_NIPPER"] = 3;
    m_arm_value_mapper["CLOSE_NIPPER"] = 4;
    m_arm_value_mapper["STOP_NIPPER"] = 5;
    m_arm_value_mapper["TORQUE_WRIST_ON"] = 6;
    m_arm_value_mapper["TORQUE_WRIST_OFF"] = 7;
    m_arm_value_mapper["None"] = 9;
}
