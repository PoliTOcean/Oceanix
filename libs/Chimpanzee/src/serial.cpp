#include <iostream>
#include <chrono>
#include "serial.hpp"
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>


#define MAX_PACKETS 100
#define MAX_PACKET_SIZE 256


const std::vector<std::string> serial_prefixes = {"/dev/ttyS"};

Serial::Serial() {
    m_connected = false;
    m_fd = -1;
    m_verbose = false;
    m_baudrate = 9600;
}


// wiringPi sucks
// #include "wiringSerial.h"
// Stolen from WiringPi:
int serialOpen (const char *device, const int baud)
{
  struct termios options ;
  speed_t myBaud ;
  int     status, fd ;

  switch (baud)
  {
    case      50:	myBaud =      B50 ; break ;
    case      75:	myBaud =      B75 ; break ;
    case     110:	myBaud =     B110 ; break ;
    case     134:	myBaud =     B134 ; break ;
    case     150:	myBaud =     B150 ; break ;
    case     200:	myBaud =     B200 ; break ;
    case     300:	myBaud =     B300 ; break ;
    case     600:	myBaud =     B600 ; break ;
    case    1200:	myBaud =    B1200 ; break ;
    case    1800:	myBaud =    B1800 ; break ;
    case    2400:	myBaud =    B2400 ; break ;
    case    4800:	myBaud =    B4800 ; break ;
    case    9600:	myBaud =    B9600 ; break ;
    case   19200:	myBaud =   B19200 ; break ;
    case   38400:	myBaud =   B38400 ; break ;
    case   57600:	myBaud =   B57600 ; break ;
    case  115200:	myBaud =  B115200 ; break ;
    case  230400:	myBaud =  B230400 ; break ;
    case  460800:	myBaud =  B460800 ; break ;
    case  500000:	myBaud =  B500000 ; break ;
    case  576000:	myBaud =  B576000 ; break ;
    case  921600:	myBaud =  B921600 ; break ;
    case 1000000:	myBaud = B1000000 ; break ;
    case 1152000:	myBaud = B1152000 ; break ;
    case 1500000:	myBaud = B1500000 ; break ;
    case 2000000:	myBaud = B2000000 ; break ;
    case 2500000:	myBaud = B2500000 ; break ;
    case 3000000:	myBaud = B3000000 ; break ;
    case 3500000:	myBaud = B3500000 ; break ;
    case 4000000:	myBaud = B4000000 ; break ;

    default:
      return -2 ;
  }

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY | O_ASYNC )) == -1)
    return -1 ;

  fcntl (fd, F_SETFL, O_RDWR | O_NONBLOCK | O_NOCTTY | O_NDELAY );

// Get and modify current options:

    tcgetattr (fd, &options) ;

    cfmakeraw   (&options) ;
    cfsetispeed (&options, myBaud) ;
    cfsetospeed (&options, myBaud) ;

    options.c_cflag |= (CLOCAL | CREAD) ;
    options.c_cflag &= ~PARENB ;
    options.c_cflag &= ~CSTOPB ;
    options.c_cflag &= ~CSIZE ;
    options.c_cflag |= CS8 ;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
    options.c_oflag &= ~OPOST ;
    options.c_oflag = 0;

    options.c_cc [VMIN] = 0;
    options.c_cc[VTIME] = 100;	// Ten seconds (100 deciseconds)

    tcsetattr (fd, TCSAFLUSH, &options);

    ioctl (fd, TIOCMGET, &status);

    status |= TIOCM_DTR;
    status |= TIOCM_RTS;

    ioctl (fd, TIOCMSET, &status);

    usleep(10000);	// 10mS

    return fd ;
}


int Serial::connect_serial() {
    std::string serial_interface;
    int max_tries = 10;

    std::cout << "[SERIAL] Trying connecting to serial..." << std::endl;
 
    // Try to connect to different serial interfaces
    for (const std::string serial_prefix: serial_prefixes) {
        for (int i = 0; i < max_tries; i++) {
            
            serial_interface = serial_prefix + std::to_string(i);
            m_fd = serialOpen(serial_interface.c_str(), m_baudrate);
            if (m_fd > -1) { 

                m_device = serial_interface;
                std::cout << "[SERIAL] " << "Connected to: " << serial_interface << std::endl;                
                tcflush(m_fd, TCIOFLUSH);
                m_connected = true;
                return true;

            }
        }
    }
    m_connected = false;
    return false;
}

int Serial::get_available_data() {
    if (!check_connection()) return -1;
    int num;
    ioctl(m_fd, FIONREAD, &num);
    return num; 
}

void Serial::set_verbose(bool val) {
    m_verbose = val;
}

void Serial::set_baudrate(int baudrate) {
    m_baudrate = baudrate;
}

void Serial::set_device(std::string serial_interface) {
    m_device = serial_interface;
}

std::string Serial::get_device() {
    return m_device;    
}

bool Serial::check_connection() {
    return access(m_device.c_str(), F_OK) == 0;
}

void Serial::disconnect_serial() {
    close(m_fd);
}

void print_vec__(std::vector<uint8_t> val) {
    for (int i = 0; i < val.size(); i++) std::cout << "  " << std::hex << static_cast<int>(val[i]) << " ";
    std::cout << std::dec << std::endl;
}

// This function get ALL the packets in the buffer, RIP dynamic allocation
std::vector<std::vector<uint8_t>> Serial::get_byte_vectors(uint8_t terminal, uint8_t escape) {
    if (!check_connection()) return {};
    uint8_t buffer[MAX_PACKETS][MAX_PACKET_SIZE] = {};
    size_t packet_lengths[MAX_PACKETS] = { 0 };
    ssize_t packet_count = 0;
    ssize_t bytes_read = 0;
    uint8_t z;
    bool esc_mode = false;

    while (packet_count < MAX_PACKETS - 1 && read(m_fd, &z, 1) > 0) { 
        if (z == escape && !esc_mode) {
            esc_mode = true;
            continue;
        }

        if (packet_lengths[packet_count] < MAX_PACKET_SIZE) {
            buffer[packet_count][packet_lengths[packet_count]++] = z;
        }
        else break;

        if (z == terminal && !esc_mode) {
            if ((++packet_count) < MAX_PACKETS) {
                packet_lengths[packet_count] = 0;
            }
        }

        if (esc_mode) esc_mode = false;
    }

    // EDGE CASES
    if (packet_count == 0 && packet_lengths[0] == 0) packet_count = -1;
    if (packet_lengths[packet_count] == 0) packet_count -= 1;

    if (packet_count == MAX_PACKETS - 2) std::cerr << "\033[31m" << "[SERIAL] WARNING: CONSIDER TO ACCELERATE BUFFER UPDATING => MAX PACKETs REACHED => OLD PACKET READ" << "\033[0m" << std::endl;

    std::vector<std::vector<uint8_t>> msg;

    for (ssize_t i = 0; i <= packet_count; i++) {
        msg.emplace_back(buffer[i], buffer[i] + packet_lengths[i]);
    }

    if (m_verbose) {
        std::cout << "[SERIAL] RECEIVED " << msg.size() << " PACKETS" << std::endl;   
        for (size_t i = 0; i < msg.size(); i++) print_vec__(msg[i]);
    }

    return msg;
}


ssize_t Serial::send_byte_array(std::vector<uint8_t> bytes) {
    tcflush(m_fd, TCOFLUSH);
    ssize_t written_byte = write(m_fd, bytes.data(), bytes.size());
    if (m_verbose) {
        std::cout << "[SERIAL] SENT: " << std::endl;
        print_vec__(bytes);
    }
    return written_byte;
}

