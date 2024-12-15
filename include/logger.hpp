#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <ctime>

#define LOG_ALL (logLevel)0 //This can be passed to construct the Logger objects, so it defaults to the lowest value

#define LOG_TYPE_COUT (uint8_t)(1 << 0)
#define LOG_TYPE_FILE (uint8_t)(1 << 1)
#define LOG_TYPE_MQTT (uint8_t)(1 << 2)


    class MQTTClient;
    enum logLevel {logDEBUG, logINFO, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;       //Name of the unit that logs (example: "CONTROL")
            logLevel minimumLogLevel;   //log messages with logLevel lower than this won't be logged

            static uint8_t logType;             //To specify the combination of logging channels to use (COUT, FILE, MQTT)
            static std::string logFileDir;      //Defaults to "log/" (done in logger.cpp)
            static std::string logFileFullPath; //Will append to logFileDir the filename
            static std::ofstream logFile;       
            static MQTTClient *mqtt_client;

            std::string logLevelToString(logLevel level);
            std::string generateLogString(logLevel logtype, std::string message);        

            void createLogFile();

        public:
            Logger(std::string unitName, logLevel minimumLogLevel);    
            void log(logLevel logtype, std::string message);
            static void configLogType(uint8_t logType);         //To initialize Logger::logType
            static void setLogFileDir(std::string logFileDir);  //To initialize Logger::logFileDir
            static void setMQTTClient(MQTTClient *mqttclient);
            static void closeLogFile();
    
    };

#endif //LOGGER_H