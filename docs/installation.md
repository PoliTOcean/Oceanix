# Installation Guide

Welcome to the installation guide for EVA. This document will walk you through the steps required to set up and install EVA_rasp on your system. Please follow the instructions carefully to ensure a smooth installation process.

## Prerequisites

Before you begin, make sure you have the following prerequisites:

- CMake (version 3.0 or higher)
- GCC (GNU Compiler Collection)
- OpenSSL
- libuv
- Eclipse Paho MQTT C and C++ libraries
- Internet connection
- Compatible operating system (Linux)
- python (only for utils)

## Installation Steps

Follow these steps to install EVA_rasp:

1. **Clone the Repository**: Clone the EVA_rasp repository from GitHub to your local machine:
    ```bash
    git clone https://github.com/repo
    ```

2. **Navigate to the Directory**: Change to the directory where the repository was cloned:
    ```bash
    cd Oceanix
    ```

3. **Install Dependencies and Build the Project**: make executable and install the required dependencies using the provided installation script:
    ```bash
    chmod +x install.sh
    ./install.sh
    ```

4. **OR Build the Project Manually**: Create a build directory, run CMake, and then Make to build the project:
    ```bash
    mkdir -p build
    cd build
    cmake ..
    make
    ```


## Utils installation
In the utils folder there is a Python GUI to send basic test command to Oceanix, to update the configuration and see some parameters in real-time.

1. **Install Dependencies and Build the Project**:
    ```bash
    cd utils
    python3 -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
    python main_app.py
    ```

Let's get started!