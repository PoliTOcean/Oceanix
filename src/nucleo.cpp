#include "nucleo.hpp"

Nucleo::Nucleo(uint8_t address, int baudrate, uint8_t version, uint8_t sub_version, bool verbose, bool test_mode)
    : m_protocol(version, sub_version, address, baudrate, verbose), m_test_mode(test_mode) {
    
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
    return m_protocol.is_connected();
}

bool Nucleo::connect() {
    if (m_test_mode) {
        return true; // Simulate successful connection in test mode
    }
    return m_protocol.connect();
}

COMM_STATUS Nucleo::init(uint8_t frequency) {
    if (m_test_mode) {
        return COMM_STATUS::OK; // Simulate successful initialization in test mode
    }
    return m_protocol.init(frequency);
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
    if (m_test_mode) {
        // Simulate sending arm command in test mode
        return arm_package_size;
    }
    m_arm_value[0] = m_arm_value_mapper[arm_value];
    return m_protocol.send_packet(1, m_arm_value, arm_package_size);
}

void Nucleo::m_init_mapper() {
    m_arm_value_mapper["ROTATE_WRIST_CCW"] = 0;
    m_arm_value_mapper["ROTATE_WRIST_CW"] = 1;
    m_arm_value_mapper["STOP_WRIST"] = 2;
    m_arm_value_mapper["OPEN_NIPPER"] = 3;
    m_arm_value_mapper["CLOSE_NIPPER"] = 4;
    m_arm_value_mapper["STOP_NIPPER"] = 5;
    m_arm_value_mapper["TORQUE_WRIST_ON"] = 9;
    m_arm_value_mapper["TORQUE_WRIST_OFF"] = 7;
    m_arm_value_mapper["None"] = 6;
}