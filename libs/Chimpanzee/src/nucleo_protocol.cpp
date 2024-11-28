#include "nucleo_protocol.hpp"

// Constructor

Protocol::Protocol(uint8_t version, uint8_t sub_version, uint8_t address, int baudrate, bool verbose) {
    m_version = version;
    m_sub_version = sub_version;
    m_address = address;
    m_verbose = verbose;
    m_buffer = {};

    m_serial.set_baudrate(baudrate);
    m_serial.set_verbose(verbose);
    m_serial.connect_serial();
}

Protocol::Protocol(protocol_config_t protocol_config) {
    m_version = protocol_config.version;
    m_sub_version = protocol_config.sub_version;
    m_address = protocol_config.address;
    m_verbose = protocol_config.verbose;
    m_buffer = {};

    m_serial.set_baudrate(protocol_config.baudrate);
    m_serial.set_verbose(protocol_config.verbose);
    m_serial.connect_serial();
}

void Protocol::decode_packet(std::vector<uint8_t> &packet, std::vector<uint8_t> &keys) {
    uint8_t key, start_index, end_index;

    // Decoding according to protocol rules
    switch (packet[0]) {
        case INIT_SEQ:
            key = packet[0];
            start_index = 0;
            break;
        case COMM_SEQ:
            if (packet.size() < 5) return;
            key = packet[3]; 
            start_index = 3;
            break;
        case HB_SEQ:
            key = packet[0];
            start_index = 2;
            break;
    }
    end_index = packet.size() - 2;
    
    // Is in the map already?
    if (std::find(keys.begin(), keys.end(), key) != keys.end()) return;
    keys.push_back(key);

    if (m_verbose) std::cout << "[CHIMPANZEE] COLLECT -> " << std::hex << static_cast<int>(key) << std::dec << std::endl;

    // Is CRC 8 correct?
    if (verify_response_CRC_8(packet) != COMM_STATUS::OK) m_buffer[key] = { COMM_STATUS::CRC_FAILED, std::nullopt };

    // THE PACKET IS READY
    std::vector<uint8_t> slice(packet.begin() + start_index, packet.begin() + end_index);

    m_buffer[key] = { COMM_STATUS::OK, slice }; 
}


bool Protocol::handle_buffer_reconstruction(std::vector<uint8_t> &packet, std::vector<uint8_t> &keys) {
    // Is the buffer empty?
    if (m_buffer.find(RESERVED_BUFFER_KEY) == m_buffer.end()) {
        m_buffer[RESERVED_BUFFER_KEY] = {COMM_STATUS::OK, packet};
        return false;
    }

    // Update the buffer and check validity
    std::vector<uint8_t> buffer_entry = m_buffer[RESERVED_BUFFER_KEY].second.value();
    buffer_entry.insert(buffer_entry.end(), packet.begin(), packet.end());
    if (is_valid_packet(buffer_entry)) {
        m_buffer[RESERVED_BUFFER_KEY] = {COMM_STATUS::OK, buffer_entry};
        remove_reserved_key(m_verbose, "REBUILT: ", m_buffer);
        decode_packet(buffer_entry, keys);
        return true;
    } 
    m_buffer[RESERVED_BUFFER_KEY] = {COMM_STATUS::OK, buffer_entry};
    return false;    
}

void Protocol::handle_packet_stream(std::vector<std::vector<uint8_t>> &packets) {
    if (packets.size() == 0) return;
    
    std::vector<uint8_t> keys = {};
    int end = 0;

    // FROM INDEX 0 -> END, HERE WE TRY TO REBUILD THE BUFFER

    // Rebuild first packets (These lines are important in the unlucky case of 2 or more incomplete packet in stream)
    // If the raw_buffer is empty we have no reason to rebuild buffer from top, for this reason the first condition
    for (end = 0; m_buffer.find(RESERVED_BUFFER_KEY) != m_buffer.end() && end < packets.size(); end++) {
        // If the packet is correct, stop searching for a match, erase buffer
        if (is_valid_packet(packets[end])) {
            remove_reserved_key(m_verbose, "ERASED: ", m_buffer);
            break;
        }

        // Try to rebuild first packet: TOP -> DOWN => if happens break
        if (handle_buffer_reconstruction(packets[end], keys)) break;


        // Too many keys in the buffer? Packet loss (or inadeguate reconstruction algorithm)!!!
        if (m_buffer[RESERVED_BUFFER_KEY].second.value().size() > 10) {
            remove_reserved_key(m_verbose, "ERASED: ", m_buffer); 
            break;        
        }
    }

    // We reset the keys vector, because there may be newer packet in the buffer
    keys = {};     

    // Last element is complete? If not put in buffer
    if (!is_valid_packet(packets[packets.size() - 1])) handle_buffer_reconstruction(packets[packets.size() - 1], keys);

    // READ PACKETs FINALLY
    for (int i = packets.size() - 1; i >= end; i--) if (is_valid_packet(packets[i])) decode_packet(packets[i], keys);
}

// Public functions

bool Protocol::is_connected() {
    return m_serial.check_connection();
}

bool Protocol::connect() {
    if (m_serial.check_connection()) return true;
    return m_serial.connect_serial();
}

COMM_STATUS Protocol::init(uint8_t interval, uint8_t max_retries, uint8_t time_between_retries) {
    if (!m_serial.check_connection()) return COMM_STATUS::SERIAL_NOT_ESTABLISHED;
    
    // Build INIT packet
    std::vector<uint8_t> packet = {INIT_SEQ, m_address, m_version, m_sub_version, interval};
    packet.push_back(calculate_CRC_8(packet));
    packet.push_back(END_SEQ); 
 
    
    ssize_t written_bytes = m_serial.send_byte_array(packet);
    
    if (written_bytes == -1) return COMM_STATUS::SERIAL_NOT_ESTABLISHED;    

    size_t retry = 0;
    keys_t available_keys = update_buffer();

    while (std::find(available_keys.begin(), available_keys.end(), INIT_SEQ) == available_keys.end() && retry < max_retries) {
        std::this_thread::sleep_for(std::chrono::milliseconds(time_between_retries));
        available_keys = update_buffer();
        retry++;
    }

    packet_t res = get_packet(INIT_SEQ, END_SEQ);
 
    if (res.first != COMM_STATUS::OK) return res.first; 

    std::vector<uint8_t> res_val = res.second.value();

    if (m_version < res_val[2] || (m_version == res_val[2] && m_sub_version < res_val[3])) return COMM_STATUS::PI_OLD_VERSION;

    return static_cast<COMM_STATUS>(res_val[4]); // 4 -> init index
}

ssize_t Protocol::send_packet(uint8_t command, uint16_t *packet_array, size_t packet_array_length) {
    if (!m_serial.check_connection()) return -1;
    
    std::vector<uint8_t> packet = {COMM_SEQ, m_address, command};

    // Transform uint16_t -> 2 uint8_t
    for (int i = 0; i < packet_array_length; i++) {
        uint8_t high = packet_array[i] >> 8;
        uint8_t low = (packet_array[i] & 0x00FF);
        packet.push_back(low);
        packet.push_back(high);
    }

    uint8_t crc = calculate_CRC_8(packet);
    
    packet.push_back(crc);

    // Byte stuffing 
    add_escape_char(packet);
    
    packet.push_back(END_SEQ);

    return m_serial.send_byte_array(packet);
}

packet_t Protocol::get_packet(uint8_t start_byte, uint8_t end_byte) {
    if (!m_serial.check_connection()) return {COMM_STATUS::SERIAL_NOT_ESTABLISHED, std::nullopt}; 
    
    // In order to avoid this type of error, use update_buffer keys in start_byte
    if (m_buffer.find(start_byte) == m_buffer.end()) return {COMM_STATUS::SERIAL_NOT_IN_BUFFER, std::nullopt};

    packet_t packet = m_buffer[start_byte];

    m_buffer.erase(start_byte);

    return packet;
}

keys_t Protocol::update_buffer() { 
    std::vector<std::vector<uint8_t>> packets = m_serial.get_byte_vectors(END_SEQ, ESCAPE_CHAR);
    
    handle_packet_stream(packets);

    return get_keys(m_buffer);
}

bool Protocol::set_sensor(sensor_config_t sensor) {
    if (sensor.id == RESERVED_BUFFER_KEY) {
        std::cerr << "This ID is reserved" << std::endl;
        return false;
    }
    uint16_t p_sensor[2];

    p_sensor[0] = (static_cast<uint16_t>(sensor.id) << 8) | sensor.i2c_address;
    p_sensor[1] = (static_cast<uint16_t>(sensor.interval_key) << 8) | sensor.type;

    return send_packet(COMM_TYPE::SENSOR, p_sensor, 2) != -1;
}

packet_t Protocol::get_sensor(uint8_t ID) {
    if (ID == RESERVED_BUFFER_KEY) {
        std::cerr << "This ID is reserved" << std::endl;
        return {COMM_STATUS::SERIAL_NOT_IN_BUFFER, std::nullopt};
    }
    return get_packet(ID);
}

packet_t Protocol::get_heartbeat() {
    return get_packet(HB_SEQ);
}

void Protocol::disconnect() {
    m_serial.disconnect_serial();
}

Protocol::~Protocol() {
    disconnect();
}