#include "logger.hpp"
#include "mqtt_client.hpp"
#include <filesystem>

//Static fields declaration
bool Logger::logTypeCout = false;
bool Logger::logTypeFile = false;
bool Logger::logTypeMQTT = false;

std::string Logger::logFileDir = "log/"; //Default
std::string Logger::logFileDirStatus = "log/status/"; //Default
std::vector<std::string> Logger::status_file_keys = {   "timestamp", "Zacc", "Zspeed", "controller_state.DEPTH", "controller_state.PITCH", "controller_state.ROLL", "depth",
                                                "error_integral.PITCH", "error_integral.ROLL", "error_integral.Z","force_pitch", "force_roll", "force_z",
                                                "motor_thrust.FDX", "motor_thrust.FSX", "motor_thrust.RDX", "motor_thrust.RSX", "motor_thrust.UPFDX", "motor_thrust.UPFSX",
                                                "motor_thrust.UPRDX", "motor_thrust.UPRSX", "motor_thrust_max_xy", "motor_thrust_max_z", "pwm.FDX", "pwm.FSX", "pwm.RDX",
                                                "pwm.RSX", "pwm.UPFDX", "pwm.UPFSX", "pwm.UPRDX", "pwm.UPRSX", "reference_pitch", "reference_roll", "reference_z",
                                                "pitch", "roll", "yaw", "angular_x", "angular_y", "angular_z", "internal_temperature", "external_temperature", "imu_state",
                                                "bar_state", "AXES"};


std::vector<std::string> Logger::transformed_status_file_keys;
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
        else return timestamp + "," + message;
}

void Logger::log(logLevel loglevel, std::string message){
    //Don't log anything if this logmessage loglevel is lower than the minimum one
    if(loglevel >= minimumLogLevel){

        std::string logString = generateLogString(loglevel, message) + "\n";

        if(Logger::logTypeMQTT && loglevel > logSTATUS){
            if(Logger::mqtt_client != NULL && Logger::unitName != MQTT_LOG_NAME){
                if(loglevel > logSTATUS) Logger::mqtt_client->send_msg(logString, Topic::LOG);
                //else Logger::mqtt_client->send_msg(logString, Topic::STATUS);
            }
        }
        
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
                    
                    Logger::logFileStatus << Logger::status_file_keys[0];
                    for(int i=1; i<Logger::status_file_keys.size(); i++){
                        Logger::logFileStatus << "," << Logger::status_file_keys[i];
                    }
                    Logger::logFileStatus << "\n";
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

