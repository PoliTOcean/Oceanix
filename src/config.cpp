#include "config.hpp"

std::map <ConfigType, std::string> config_map {
   {ConfigType::ALL, "All"},
   {ConfigType::GENERAL, "General"},
   {ConfigType::MOTORS, "Motors"},
   {ConfigType::CONTROLLER, "Controller"},
};


Config::Config(std::string config_file_path) {
    m_config_file_path = config_file_path;
    
    load_base_config();
}

void Config::change_config(std::string msg_string) { 
    if (!checkJsonFormat(msg_string, m_json_config))
        std::cout << "[CONFIG - WARNING] Config json not parseble or wrong keys/key types!" << std::endl;
    else
        m_json_config = json::parse(msg_string); 
}
    
void Config::load_base_config() {
    std::ifstream json_config_file(m_config_file_path);

    if (json_config_file.is_open()) {
        std::cout << "[CONFIG - INFO] Reading config file" << std::endl;
        
        json_config_file >> m_json_config;
        json_config_file.close();
    }
    else
        std::cout << "[CONFIG - ERROR] Error opening config file!" << std::endl;
}

void Config::write_base_config() {
    std::ofstream json_config_file(m_config_file_path);

    if (json_config_file.is_open()) {
        std::cout << "[CONFIG - INFO] Writing config file" << std::endl;
        
        json_config_file << m_json_config;
        json_config_file.close();
        load_base_config();
    }
    else
        std::cout << "[CONFIG - ERROR] Error opening config file!" << std::endl;
}

void Config::print_config() {
    std::cout << "[CONFIG - INFO] Print configuration json (indented): " << '\n'
              << m_json_config.dump(1, ' ', true) << std::endl;
}

json Config::get_config(ConfigType config_type) {
    if (config_map[config_type] == config_map[ConfigType::ALL]) 
        return m_json_config;
    return m_json_config[config_map[config_type]];
}
