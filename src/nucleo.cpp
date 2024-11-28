#include "nucleo.hpp"

Nucleo::Nucleo(uint8_t address, int baudrate, uint8_t version, uint8_t sub_version, bool verbose)
    : m_protocol(version, sub_version, address, baudrate, verbose) {
    
    m_init_mapper();
}

/**
* is_connected - Check if the Raspberry is connected to the Nucleo.
* 
* @return: true if is connected, false otherwise.
*/
bool Nucleo::is_connected() { return m_protocol.is_connected(); }
bool Nucleo::connect() { return m_protocol.connect(); }
COMM_STATUS Nucleo::init(uint8_t frequency) { return m_protocol.init(frequency); }
void Nucleo::disconnect() { m_protocol.disconnect(); }
std::pair<COMM_STATUS, std::optional<std::vector<uint8_t>>> Nucleo::get_heartbeat() { return m_protocol.get_heartbeat(); }
void Nucleo::update_buffer() { m_protocol.update_buffer(); }

ssize_t Nucleo::send_pwm(uint16_t* pwm_values) {
    return m_protocol.send_packet(0, pwm_values, motorN);
}

ssize_t Nucleo::send_arm(std::string arm_value) {
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
