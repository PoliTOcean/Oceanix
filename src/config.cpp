#include "config.hpp"

std::map <ConfigType, std::string> config_map {
   {ConfigType::ALL, "All"},
   {ConfigType::GENERAL, "General"},
   {ConfigType::MOTORS, "Motors"},
   {ConfigType::CONTROLLER, "Controller"},
};


Config::Config(std::string config_file_path, logLevel minimumLoglevel) : logger(Logger("CONFIG ", minimumLoglevel)) {
    m_config_file_path = find_config_file(config_file_path);
    
    load_base_config();
}

void Config::change_config(json msg_config) { 
    if (!checkJsonFormat(msg_config, m_json_config))
        logger.log(logWARNING, "Config json not parseble or wrong keys/key types!");
    else
        logger.log(logINFO, "Config json updated!");
        m_json_config = msg_config;
}    

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
        json_config_file << std::setw(4) << m_json_config;
        json_config_file.close();
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

json& Config::get_config(ConfigType config_type) {
    if (config_map[config_type] == config_map[ConfigType::ALL]) 
        return m_json_config;
    return m_json_config[config_map[config_type]];
}

std::string Config::find_config_file(std::string config_file_path) {
    std::string paths[] = {
        config_file_path,                // Current path
        "./config.json",                 // Current directory
        "../config/config.json",         // Relative path
        "config/config.json",
        "Oceanix/config/config.json",
        std::getenv("OCEANIX_CONFIG_PATH") ? std::getenv("OCEANIX_CONFIG_PATH") : ""     // Environment variable (if set)
    };

    for (const auto& path : paths) {
        if (!path.empty()) { // Check if environment variable is set
            std::ifstream file(path);
            if (file.good()) {
                return path;
            }
        }
    }
    // If no file is found, return an empty string or handle the error as needed
    return config_file_path;
}