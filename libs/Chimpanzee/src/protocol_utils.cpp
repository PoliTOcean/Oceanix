#include "protocol_utils.hpp"

// Utils -> Private

void print_vec(std::vector<uint8_t> val) {
    for (int i = 0; i < val.size(); i++) std::cout << std::hex << static_cast<int>(val[i]) << " ";
    std::cout << std::dec << std::endl;
}

uint8_t calculate_CRC_8(const std::vector<uint8_t>& data) {
    uint8_t crc = 0x00;
    uint8_t polynomial = 0x07; 
    for (auto byte : data) {
        crc ^= byte; 
        for (int i = 0; i < 8; i++) {
            if (crc & 0x80) crc = (crc << 1) ^ polynomial;
            else crc <<= 1;
        }
    }
    return crc;
}

COMM_STATUS verify_response_CRC_8(const std::vector<uint8_t>& res) {
    uint8_t res_crc = res[res.size() - 2];

    std::vector<uint8_t> res_to_verify;
    
    // These lines exclude ESCAPE_CHAR from CRC calculation
    for (size_t i = 0; i < res.size() - 2; i++) {
        if (res[i] != ESCAPE_CHAR) {
            res_to_verify.push_back(res[i]);
        } 
        else if (i + 1 < res.size() - 2 && res[i + 1] == ESCAPE_CHAR) {
            res_to_verify.push_back(res[i + 1]);
            i++;
        }
    } 

    uint8_t crc_to_verify = calculate_CRC_8(res_to_verify);

    return res_crc != crc_to_verify ? COMM_STATUS::CRC_FAILED : COMM_STATUS::OK; 
}

void add_escape_char(std::vector<uint8_t>& vec) {
    for (int i = vec.size() - 1; i > 0; i--) {
        if (std::find(bytes_to_escape, bytes_to_escape + NUM_SEQ + 2, vec[i]) != bytes_to_escape + NUM_SEQ + 2) {
            vec.insert(vec.begin() + i, ESCAPE_CHAR);    
        }
    }
}

keys_t get_keys(std::unordered_map<uint8_t, packet_t> buffer) {
    keys_t available_keys;
    for (auto pair : buffer) if (pair.first != RESERVED_BUFFER_KEY) available_keys.push_back(pair.first);
    return available_keys;
}

bool is_valid_packet(std::vector<uint8_t> packet) {
    size_t dim = packet.size();
    return dim > 4 // Minimum dimension (minum packet without byte stuffing)
    && std::find(start_bytes, start_bytes + NUM_SEQ, packet[0]) != start_bytes + NUM_SEQ // Is a start of packet?
    && packet[dim - 1] == END_SEQ; // Ends with END_SEQ?
}

void remove_reserved_key(bool verbose, std::string content, std::unordered_map<uint8_t, packet_t> &buffer) {
    if (verbose) {
        std::cout << content << std::endl;
        print_vec(buffer[RESERVED_BUFFER_KEY].second.value());
    }
    buffer.erase(RESERVED_BUFFER_KEY); 
}
