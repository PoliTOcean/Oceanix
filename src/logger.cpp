#include "logger.hpp"

Logger::Logger(std::string unitName, logLevel minimumLogLevel) 
    : unitName(unitName), 
    minimumLogLevel(minimumLogLevel) {}

uint8_t Logger::logType = 0;
void Logger::configLogType(uint8_t logType){
    Logger::logType = logType;
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
    //Don't log anything is this logmessage loglevel is lower than the minimum one
    if(loglevel >= minimumLogLevel){
        
        if(Logger::logType & LOG_TYPE_COUT){
            //clog is like cout, key difference is that you can redirect the output of clog to a different destination
            std::clog << generateLogString(loglevel, message) << std::endl;
        }

        if(Logger::logType & LOG_TYPE_FILE){
            //TO DO
            ;
        }

        if(Logger::logType & LOG_TYPE_MQTT){
            //TO DO
            ;
        }
    }
}