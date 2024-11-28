#ifndef SERIAL_H_
#define SERIAL_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <unordered_map>
#include <filesystem>

class Serial {
    public:
        Serial();
        
        std::string get_device() const { return m_device; }
        int get_fd() const { return m_fd; }
        void set_verbose(bool val);
        void set_baudrate(int baudrate);

        bool check_connection();
        
        void disconnect_serial();
        
        /**
        * Connect serial communication, device is assigned automatically
        * @return True: successful connection, False: error in connection
        */
        int connect_serial();
        
        /**
        * Get the amount of incoming bytes from serial communication  
        * 
        */
        int get_available_data();


        void set_device(std::string serial_interface);
        std::string get_device();

        
        

        /**
         * Get bytes until terminal byte sequence
         * @param terminal Final byte
         * @return Byte vector
         */
        std::vector<std::vector<uint8_t>> get_byte_vectors(uint8_t terminal, uint8_t escape);

        ssize_t send_byte_array(std::vector<uint8_t> bytes);

    private:
        int m_fd; // File Descriptor
        bool m_connected; 
        std::string m_device;
        bool m_verbose;
        int m_baudrate;
};


#endif // SERIAL_H_