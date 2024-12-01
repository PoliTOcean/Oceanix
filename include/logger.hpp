#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

    enum logLevel {logINFO, logDEBUG, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;

            std::string logLevelToString(logLevel level);
            std::string generateLogString(logLevel logtype, std::string message);        

        public:
            Logger(std::string unitName);            
            void log(logLevel logtype, std::string message);

    
    };

#endif //LOGGER_H