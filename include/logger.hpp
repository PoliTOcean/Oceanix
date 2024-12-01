#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

    enum logLevel {logINFO, logDEBUG, logWARNING, logERROR};

    class Logger {
        private:
            std::string logLevelToString(logLevel level);
            std::string generateLogString(std::string className, logLevel logtype, std::string message);    
            virtual void printLog(logLevel logtype, std::string message);    

        public:
            void printLog(std::string className, logLevel logtype, std::string message);
    };

#endif //LOGGER_H