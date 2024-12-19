#ifndef CONFIGCLASS_H
#define CONFIGCLASS_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "json.hpp"
#include "config.hpp"
#include "utils.hpp"

using json = nlohmann::json;

enum class ConfigType { ALL, GENERAL, MOTORS, CONTROLLER };

class Config {
    public:
        Config(std::string config_file_path);
        
        json& get_config(ConfigType config_type);
        void print_config();
        void load_base_config();
        void write_base_config(); 
        void change_config(json msg_config);
    
    private:
        json m_json_config; 
        std::string m_config_file_path;

        std::string find_config_file(std::string config_file_path);

};

#endif // CONFIGCLASS_H
