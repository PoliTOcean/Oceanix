#include "logger.hpp"
#include "mqtt_client.hpp"
#include <filesystem>

//Static fields declaration
bool Logger::logTypeCout = false;
bool Logger::logTypeFile = false;
bool Logger::logTypeMQTT = false;

std::string Logger::logFileDir = "/log"; //Default
std::string Logger::logFileFullPath;
std::ofstream Logger::logFile;
MQTTClient *Logger::mqtt_client = NULL;

Logger::Logger(std::string unitName, logLevel minimumLogLevel) 
    : unitName(unitName), 
    minimumLogLevel(minimumLogLevel) {}

void Logger::configLogTypeCout(bool value){
    Logger::logTypeCout = value;
}

void Logger::configLogTypeFile(bool value){
    Logger::logTypeFile = value;
}

void Logger::configLogTypeMQTT(bool value){
    Logger::logTypeMQTT = value;
}

void Logger::setLogFileDir(std::string logFileDir){
    Logger::logFileDir = logFileDir;
}

void Logger::setMQTTClient(MQTTClient *mqtt_client){
    Logger::mqtt_client = mqtt_client;
}

std::string Logger::logLevelToString(logLevel loglevel) {
    switch (loglevel) {
        case logINFO:       return "[INFO ]";
        case logDEBUG:      return "[DEBUG]";
        case logWARNING:    return "[WARN ]";
        case logERROR:      return "[ERROR]";
        default:      return "[UNKNOWN]";
    }
}

std::string Logger::generateLogString(logLevel loglevel, std::string message){
        return "["+ unitName + "]" + logLevelToString(loglevel) + message;
}

void Logger::log(logLevel loglevel, std::string message){
    //Don't log anything if this logmessage loglevel is lower than the minimum one
    if(loglevel >= minimumLogLevel){
        
        std::string logString = generateLogString(loglevel, message) + "\n";

        if(Logger::logTypeCout){
            std::cout << logString;
        }

        if(Logger::logTypeFile){
            if(!Logger::logFile.is_open()){
                //The logFile stream isn't associated with an existing file
                Logger::createLogFile();
            }
            Logger::logFile << logString;
        }

        if(Logger::logTypeMQTT){
            //TO DO
            if(Logger::mqtt_client != NULL && Logger::unitName != MQTT_LOG_NAME){
                Logger::mqtt_client->send_msg(logString, Topic::LOG);
            }
        }
    }
}

void Logger::setLogLevel(logLevel new_level){
    minimumLogLevel = new_level;
}

void Logger::createLogFile(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    std::filesystem::path dirPath = std::filesystem::path(logFileDir);
    
    if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath); // Creates all directories in the path if they do not exist
    }

    
    strftime(buffer,sizeof(buffer),"%d-%m-%Y_%H:%M:%S",timeinfo);
    std::string timestamp_string(buffer);

    Logger::logFileFullPath = Logger::logFileDir + "/log_" + timestamp_string;

    Logger::logFile.open(Logger::logFileFullPath);
}

void Logger::closeLogFile(){
    Logger::logFile.close();
}
