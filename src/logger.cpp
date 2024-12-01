#include "logger.hpp"

Logger::Logger(std::string unitName, logLevel minimumLogLevel) 
    : unitName(unitName), 
    minimumLogLevel(minimumLogLevel) {}

std::string Logger::logLevelToString(logLevel level) {
    switch (level) {
        case logINFO:       return "[INFO ]";
        case logDEBUG:      return "[DEBUG]";
        case logWARNING:    return "[WARN ]";
        case logERROR:      return "[ERROR]";
        default:      return "[UNKNOWN]";
    }
}

std::string Logger::generateLogString(logLevel logtype, std::string message){
        return "["+ unitName + "]" + logLevelToString(logtype) + message;
}

void Logger::log(logLevel logtype, std::string message){
    //Don't log anything is this logmessage loglevel is lower than the minimum one
    if(logtype >= minimumLogLevel){
        //clog is like cout, key difference is that you can redirect the output of clog to a different destination
        std::clog << generateLogString(logtype, message) << std::endl;
    }
}