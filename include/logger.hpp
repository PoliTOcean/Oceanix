#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <ctime>
#include <chrono>
#include <vector>

#define MQTT_LOG_NAME       "MQTThst"
#define BAR02_LOG_NAME      "BAR02  "
#define CONFIG_LOG_NAME     "CONFIG "
#define CONTROLLER_LOG_NAME "CONTROL"
#define MAIN_LOG_NAME       "MAIN   "
#define MOTORS_LOG_NAME     "MOTORS "
#define IMU_LOG_NAME        "IMU    "
#define NUCLEO_LOG_NAME     "NUCLEO "

#define LOG_ALL (logLevel)0 //This can be passed to construct the Logger objects, so it defaults to the lowest value


    class MQTTClient;
    enum logLevel {logSTATUS, logINFO, logWARNING, logERROR};

    class Logger {
        private:
            std::string unitName;       //Name of the unit that logs (example: "CONTROL")
            logLevel minimumLogLevel;   //log messages with logLevel lower than this won't be logged

            static bool logTypeCout;             //To specify the logging channels to use (COUT, FILE, MQTT)
            static bool logTypeFile;
            static bool logTypeMQTT;
            static std::string logFileDir;      //Defaults to "log/" (done in logger.cpp)
            static std::string logFileDirStatus; //Default

            static std::ofstream logFile;       
            static std::ofstream logFileStatus;      
            static MQTTClient *mqtt_client;

            
            std::string logLevelToString(logLevel level);
            std::string generateLogString(logLevel logtype, std::string message);        
            
            std::ofstream createLogFile(std::string newFileDir, std::string newFileName);

        public:

            static std::vector<std::string> status_file_keys;
            static std::vector<std::string> transformed_status_file_keys; // Used to read the flattened dictionary inside main


            Logger(std::string unitName, logLevel minimumLogLevel);    
            void log(logLevel logtype, std::string message);
            void setLogLevel(logLevel new_level);
            
            static void configLogTypeCout(bool value);
            static void configLogTypeFile(bool value);
            static void configLogTypeMQTT(bool value);

            static void setLogFileDir(std::string logFileDir);  //To initialize Logger::logFileDir
            static void setMQTTClient(MQTTClient *mqttclient);
        
            
            static void closeLogFiles();
            
    };

#endif //LOGGER_H
