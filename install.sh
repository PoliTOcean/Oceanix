#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

#SUDO = "sudo"

mkdir -p tmp
cd tmp

# Check for SUDO
if ! command_exists sudo; then
    echo "SUDO is not installed. Running commands without SUDO."
    #SUDO="sudo"
fi

# Check for Git
if ! command_exists git; then
    echo "Git is not installed. Installing Git..."
    sudo apt-get update
    sudo apt-get install -y git
else
    echo "Git is already installed."
fi

# Check for CMake
if ! command_exists cmake; then
    echo "CMake is not installed. Installing CMake..."
    sudo apt-get update
    sudo apt-get install -y build-essential cmake pkg-config
    sudo cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++
else
    echo "CMake is already installed."
fi

# Check for GCC
if ! command_exists gcc; then
    echo "GCC is not installed. Installing GCC..."
    sudo apt-get update
    sudo apt-get install -y gcc g++
else
    echo "GCC is already installed."
fi

# Check for OpenSSL
if ! dpkg -s libssl-dev >/dev/null 2>&1; then
    echo "OpenSSL is not installed. Installing OpenSSL..."
    sudo apt-get update
    sudo apt-get install -y libssl-dev
else
    echo "OpenSSL is already installed."
fi

# Check for libuv
if ! dpkg -s libuv1-dev >/dev/null 2>&1; then
    echo "libuv is not installed. Installing libuv..."
    sudo apt-get update
    sudo apt-get install -y libuv1-dev
else
    echo "libuv is already installed."
fi

# Check for Eclipse Paho MQTT C and C++ libraries
if ! dpkg -s libpaho-mqtt-dev libpaho-mqttpp-dev >/dev/null 2>&1; then
    echo "Eclipse Paho MQTT libraries are not installed. Installing Eclipse Paho MQTT libraries..."
    sudo git clone https://github.com/eclipse/paho.mqtt.cpp.git
    sudo git clone https://github.com/eclipse/paho.mqtt.c.git
    cd paho.mqtt.c

    # Create a build directory
    sudo mkdir build
    cd build

    # Configure and build
    sudo cmake .. -DPAHO_WITH_SSL=ON -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=OFF
    sudo make -j$(nproc)

    # Install the library
    sudo make install
    sudo ldconfig

    cd ../../paho.mqtt.cpp

    # Create a build directory
    sudo mkdir build
    cd build

    # Configure and build
    sudo cmake .. -DPAHO_BUILD_DOCUMENTATION=OFF -DPAHO_WITH_SSL=ON
    sudo make -j$(nproc)

    # Install the library
    sudo make install
    sudo ldconfig

    cd ../../

    echo "Verify MQTT install..."
    sudo ls /usr/local/lib | grep paho
else
    echo "Eclipse Paho MQTT libraries are already installed."
fi

# Check for mosquitto
if ! command_exists mosquitto; then
    echo "mosquitto is not installed. Installing mosquitto..."
    sudo apt-get update
    sudo apt install mosquitto mosquitto-clients -y
    sudo cp ../.devcontainer/mosquitto /etc/init.d
    sudo service mosquitto start
else
    echo "mosquitto is already installed."
fi

cd ..

# Create a build directory
mkdir -p build

# Run CMake
echo "Running CMake..."
cmake -S . -B build

# Run Make
echo "Running Make..."
cmake --build build

echo "Installation and build process completed."