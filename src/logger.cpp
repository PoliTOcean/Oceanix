#include "logger.hpp"
#include "mqtt_client.hpp"
#include <filesystem>

//Static fields declaration
bool Logger::logTypeCout = false;
bool Logger::logTypeFile = false;
bool Logger::logTypeMQTT = false;

std::string Logger::logFileDir = "log/"; //Default
std::string Logger::logFileDirStatus = "log/status/"; //Default
std::ofstream Logger::logFile;
std::ofstream Logger::logFileStatus;
MQTTClient *Logger::mqtt_client = NULL;

Logger::Logger(std::string unitName, logLevel minimumLogLevel) 
    : unitName(unitName) {
    setLogLevel(minimumLogLevel);
}

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
    Logger::logFileDirStatus = logFileDir + "/status/";
}

void Logger::setMQTTClient(MQTTClient *mqtt_client){
    Logger::mqtt_client = mqtt_client;
}

std::string Logger::logLevelToString(logLevel loglevel) {
    switch (loglevel) {
        case logINFO:       return "[INFO ]";
        case logSTATUS:      return "[STATS]";
        case logWARNING:    return "[WARN ]";
        case logERROR:      return "[ERROR]";
        default:      return "[UNKNOWN]";
    }
}

std::string Logger::generateLogString(logLevel loglevel, std::string message){
        const auto now = std::chrono::system_clock::now().time_since_epoch();
        const auto timestamp = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());

        if(loglevel > logSTATUS) return timestamp + "["+ unitName + "]" + logLevelToString(loglevel) + message;
        else return timestamp + " " + message;
}

void Logger::log(logLevel loglevel, std::string message){
    //Don't log anything if this logmessage loglevel is lower than the minimum one
    if(loglevel >= minimumLogLevel){

        if(Logger::logTypeMQTT){
            if(Logger::mqtt_client != NULL && Logger::unitName != MQTT_LOG_NAME){
                if(loglevel > logSTATUS) Logger::mqtt_client->send_msg(message, Topic::LOG);
                //else Logger::mqtt_client->send_msg(message, Topic::STATUS);
            }
        }
        
        std::string logString = generateLogString(loglevel, message) + "\n";

        if(Logger::logTypeCout && loglevel > logSTATUS){
            std::cout << logString;
        }

        if(Logger::logTypeFile){
            if(loglevel > logSTATUS){
                if(!Logger::logFile.is_open()){
                    //The logFile stream isn't associated with an existing file
                    Logger::logFile = Logger::createLogFile(Logger::logFileDir, "log_");
                }
                Logger::logFile << logString;
            }
            else{
                if(!Logger::logFileStatus.is_open()){
                    //The logFile stream isn't associated with an existing file
                    Logger::logFileStatus = Logger::createLogFile(Logger::logFileDirStatus, "log_status_");
                }
                Logger::logFileStatus << logString;
            }
        }
    }
}

void Logger::setLogLevel(logLevel new_level){
    if(new_level < LOG_ALL) new_level = LOG_ALL;
    if(new_level > logERROR) new_level = logERROR;
    minimumLogLevel = new_level;
}

std::ofstream Logger::createLogFile(std::string newFileDir, std::string newFileName){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    std::ofstream newFile;
    std::string newFileFullName;

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    std::filesystem::path dirPath = std::filesystem::path(newFileDir);
    
    if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath); // Creates all directories in the path if they do not exist
    }

    
    strftime(buffer,sizeof(buffer),"%d-%m-%Y_%H:%M:%S",timeinfo);
    std::string timestamp_string(buffer);

    newFileFullName = newFileDir + newFileName + timestamp_string;
    std::cout << newFileFullName << "\n";
    newFile.open(newFileFullName);
    return newFile;
}

void Logger::closeLogFiles(){
    Logger::logFile.close();
    Logger::logFileStatus.close();
}
