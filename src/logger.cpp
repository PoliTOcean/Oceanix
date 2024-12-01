#include "logger.hpp"



Logger::Logger(){

}

std::string Logger::logLevelToString(logLevel level) {
    switch (level) {
        case logINFO:       return "[INFO ]";
        case logDEBUG:      return "[DEBUG]";
        case logWARNING:    return "[WARN ]";
        case logERROR:      return "[ERROR]";
        default:      return "[UNKNOWN]";
    }
}

std::string Logger::generateLogString(std::string className, logLevel logtype, std::string message){
        return "["+ className + "]" + logLevelToString(logtype) + message;
}

void Logger::printLog(std::string className, logLevel logtype, std::string message){
    //clog is like cout, key difference is that you can redirect the output of clog to a different destination
    std::clog << generateLogString(className, logtype, message) << std::endl;
}