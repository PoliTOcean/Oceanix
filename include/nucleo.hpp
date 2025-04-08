#ifndef NUCLEO_H
#define NUCLEO_H

#include <stdlib.h>
#include <cstdlib> 
#include <optional>
#include "json.hpp"
#include "nucleo_protocol.hpp"
#include "nucleo.hpp"
#include <pigpio.h>
#include "logger.hpp"

#define RST_PIN 17

const int motorN = 8;
const int arm_package_size = 1;

using json = nlohmann::json;                                                                             

class Nucleo {
    public:
        Nucleo(uint8_t address, int baudrate, uint8_t version, uint8_t sub_version, logLevel minimumLoglevel, int64_t heartbeat_interval, bool verbose = false, bool test_mode = false);

        bool is_connected();
        COMM_STATUS init(uint8_t frequency, int n_tries);
        std::pair<COMM_STATUS, std::optional<std::vector<uint8_t>>> get_heartbeat(); // use auto
        void update_buffer();

        ssize_t send_pwm(uint16_t* pwm_values);
        ssize_t send_arm(std::string arm_values);

    private:
        Protocol m_protocol;
        uint16_t m_arm_value[arm_package_size];
        bool m_test_mode;
        int64_t last_hb_timestamp;
        int64_t heartbeat_interval;
        std::map <std::string, int> m_arm_value_mapper;
        Logger logger;

        bool connect();
        void disconnect();
        void m_init_mapper();

        
};

#endif // NUCLEO_H
