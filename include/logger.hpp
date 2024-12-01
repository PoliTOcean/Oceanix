#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

#define LOG_ALL (logLevel)0 //This can be passed to construct the Logger objects, so it defaults to the lowest value

    enum logLevel {logINFO, logDEBUG, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;
            logLevel minimumLogLevel;

            std::string logLevelToString(logLevel level);
            std::string generateLogString(logLevel logtype, std::string message);        

        public:
            Logger(std::string unitName, logLevel minimumLogLevel);            
            void log(logLevel logtype, std::string message);

    
    };

#endif //LOGGER_H