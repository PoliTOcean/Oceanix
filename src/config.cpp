#include "config.hpp"

std::map <ConfigType, std::string> config_map {
   {ConfigType::ALL, "All"},
   {ConfigType::GENERAL, "General"},
   {ConfigType::MOTORS, "Motors"},
   {ConfigType::CONTROLLER, "Controller"},
};


Config::Config(std::string config_file_path) : logger(Logger("CONFIG ")) {
    m_config_file_path = config_file_path;
    
    load_base_config();
}

void Config::change_config(std::string msg_string) { m_json_config = json::parse(msg_string); }
    
void Config::load_base_config() {
    std::ifstream json_config_file(m_config_file_path);

    if (json_config_file.is_open()) {
        logger.log(logINFO, "Reading config file");
        
        json_config_file >> m_json_config;
        json_config_file.close();
    }
    else
        logger.log(logERROR, "Error opening config file!");
}

void Config::write_base_config() {
    std::ofstream json_config_file(m_config_file_path);

    if (json_config_file.is_open()) {
        logger.log(logINFO, "Writing config file");


        json_config_file << m_json_config;
        json_config_file.close();
        load_base_config();
    }
    else
        logger.log(logERROR, "Error opening config file!");
}

void Config::print_config() {
    std::ostringstream logMessage;
    logMessage << "Print configuration json (indented): " << '\n'
              << m_json_config.dump(1, ' ', true) << std::endl;
    logger.log(logINFO, logMessage.str());
}

json Config::get_config(ConfigType config_type) {
    if (config_map[config_type] == config_map[ConfigType::ALL]) 
        return m_json_config;
    return m_json_config[config_map[config_type]];
}