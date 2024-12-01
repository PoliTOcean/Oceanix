#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <stdint.h>

#define LOG_ALL (logLevel)0 //This can be passed to construct the Logger objects, so it defaults to the lowest value

#define LOG_TYPE_COUT (uint8_t)(1 << 0)
#define LOG_TYPE_FILE (uint8_t)(1 << 1)
#define LOG_TYPE_MQTT (uint8_t)(1 << 2)

    enum logLevel {logINFO, logDEBUG, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;
            logLevel minimumLogLevel;
            static uint8_t logType;

            std::string logLevelToString(logLevel level);
            std::string generateLogString(logLevel logtype, std::string message);        

        public:
            Logger(std::string unitName, logLevel minimumLogLevel);            
            void log(logLevel logtype, std::string message);
            static void configLogType(uint8_t logType);
    };

#endif //LOGGER_H