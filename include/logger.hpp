#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <ctime>

#define LOG_ALL (logLevel)0 //This can be passed to construct the Logger objects, so it defaults to the lowest value


    class MQTTClient;
    enum logLevel {logDEBUG, logINFO, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;       //Name of the unit that logs (example: "CONTROL")
            logLevel minimumLogLevel;   //log messages with logLevel lower than this won't be logged

            static bool logTypeCout;             //To specify the logging channels to use (COUT, FILE, MQTT)
            static bool logTypeFile;
            static bool logTypeMQTT;
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
            static void configLogTypeCout(bool value);
            static void configLogTypeFile(bool value);
            static void configLogTypeMQTT(bool value);

            static void setLogFileDir(std::string logFileDir);  //To initialize Logger::logFileDir
            static void setMQTTClient(MQTTClient *mqttclient);
            static void closeLogFile();
    
    };

#endif //LOGGER_H