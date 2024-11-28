#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

SUDO = "SUDO"

# Check for SUDO
if ! command_exists sudo; then
    echo "SUDO is not installed. Running commands without SUDO."
    SUDO=""
fi

# Check for Git
if ! command_exists git; then
    echo "Git is not installed. Installing Git..."
    $SUDO apt-get update
    $SUDO apt-get install -y git
else
    echo "Git is already installed."
fi

# Check for CMake
if ! command_exists cmake; then
    echo "CMake is not installed. Installing CMake..."
    $SUDO apt-get update
    $SUDO apt-get install -y build-essential cmake pkg-config
    $SUDO cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++
else
    echo "CMake is already installed."
fi

# Check for GCC
if ! command_exists gcc; then
    echo "GCC is not installed. Installing GCC..."
    $SUDO apt-get update
    $SUDO apt-get install -y gcc g++
else
    echo "GCC is already installed."
fi

# Check for OpenSSL
if ! dpkg -s libssl-dev >/dev/null 2>&1; then
    echo "OpenSSL is not installed. Installing OpenSSL..."
    $SUDO apt-get update
    $SUDO apt-get install -y libssl-dev
else
    echo "OpenSSL is already installed."
fi

# Check for libuv
if ! dpkg -s libuv1-dev >/dev/null 2>&1; then
    echo "libuv is not installed. Installing libuv..."
    $SUDO apt-get update
    $SUDO apt-get install -y libuv1-dev
else
    echo "libuv is already installed."
fi

# Check for Eclipse Paho MQTT C and C++ libraries
if ! dpkg -s libpaho-mqtt-dev libpaho-mqttpp-dev >/dev/null 2>&1; then
    echo "Eclipse Paho MQTT libraries are not installed. Installing Eclipse Paho MQTT libraries..."
    $SUDO git clone https://github.com/eclipse/paho.mqtt.cpp.git
    $SUDO git clone https://github.com/eclipse/paho.mqtt.c.git
    $SUDO cd paho.mqtt.c

    # Create a build directory
    $SUDO mkdir build
    $SUDO cd build

    # Configure and build
    $SUDO cmake .. -DPAHO_WITH_SSL=ON -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=OFF
    $SUDO make -j$(nproc)

    # Install the library
    $SUDO make install
    $SUDO ldconfig

    $SUDO cd ../../paho.mqtt.cpp

    # Create a build directory
    $SUDO mkdir build
    $SUDO cd build

    # Configure and build
    $SUDO cmake .. -DPAHO_BUILD_DOCUMENTATION=OFF -DPAHO_WITH_SSL=ON
    $SUDO make -j$(nproc)

    # Install the library
    $SUDO make install
    $SUDO ldconfig

    $SUDO cd ../../

    echo "Verify MQTT install..."
    $SUDO ls /usr/local/lib | grep paho
else
    echo "Eclipse Paho MQTT libraries are already installed."
fi

# Clone the repository (if not already cloned)
# if [ ! -d "Oceanix" ]; then
#     echo "Cloning the repository..."
#     git clone https://github.com/your-repo/Oceanix.git
# fi

# Create a build directory
mkdir -p build
#cd build

# Run CMake
echo "Running CMake..."
cmake -S . -B build

# Run Make
echo "Running Make..."
cmake --build build

echo "Installation and build process completed."