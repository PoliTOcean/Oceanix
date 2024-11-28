#ifndef PROTOCOL_UTILS_H
#define PROTOCOL_UTILS_H


#include <chrono>
#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <sstream>
#include <optional>
#include <unordered_map>
#include <vector>
#include <algorithm> 


#define END_SEQ 0xEE
#define ESCAPE_CHAR 0x7E

#define NUM_SEQ 4
#define COMM_SEQ 0xAA
#define HB_SEQ 0xBB
#define SENS_SEQ 0xCC
#define INIT_SEQ 0xFF
#define RESERVED_BUFFER_KEY 0xDE

static const uint8_t start_bytes[NUM_SEQ] = { INIT_SEQ, COMM_SEQ, HB_SEQ, SENS_SEQ };
static const uint8_t bytes_to_escape[NUM_SEQ + 2] = { INIT_SEQ, COMM_SEQ, HB_SEQ, SENS_SEQ, END_SEQ, ESCAPE_CHAR };

enum COMM_TYPE {
    MOTOR,
    ARM,
    SENSOR
};

enum COMM_STATUS {
    OK,
    SERIAL_NOT_IN_BUFFER,
    SERIAL_INCOMPLETE_COMMUNICATION,
    SERIAL_NOT_ESTABLISHED,
    PI_OLD_VERSION,
    NUCLEO_OLD_VERSION,
    NUCLEO_TIMEOUT,
    NUCLEO_INVALID_ANSWER,
    CRC_FAILED
};

enum HB_OK_STATUS {
    READY,
    PROCESSING_PWM,
};

enum HB_ERROR_STATUS {
    NOT_READY,
    THRUSTER_FAIL
};

// See documentation for description of types list
enum SENSOR_TYPE {
    VOLTAGE_AND_CURRENT,
    TEMPERATURE,
    FLOOD,
    PH,
    DEPTH
};

typedef std::pair<COMM_STATUS, std::optional<std::vector<uint8_t>>> packet_t;

typedef std::vector<uint8_t> keys_t;

typedef struct {
    uint8_t id;
    uint8_t i2c_address;
    uint8_t interval_key;
    SENSOR_TYPE type;
} sensor_config_t;

typedef struct {
    uint8_t address; 
    uint8_t version; 
    uint8_t sub_version; 
    int baudrate; 
    bool verbose;
} protocol_config_t;


uint8_t calculate_CRC_8(const std::vector<uint8_t>& data);

COMM_STATUS verify_response_CRC_8(const std::vector<uint8_t>& res);

void add_escape_char(std::vector<uint8_t>& vec);

void remove_escape_char(std::vector<uint8_t>& input);

keys_t get_keys(std::unordered_map<uint8_t, packet_t> buffer);

bool is_valid_packet(std::vector<uint8_t> packet);

void remove_reserved_key(bool verbose, std::string content, std::unordered_map<uint8_t, packet_t> &buffer);

#endif // PROTOCOL_UTILS_H