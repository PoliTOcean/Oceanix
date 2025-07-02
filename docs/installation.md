# Oceanix Installation Guide

Welcome to the installation guide for Oceanix. This document will walk you through the steps required to set up and install Oceanix on your system. Please follow the instructions carefully to ensure a smooth installation process.

## Prerequisites

Before you begin, make sure you have the following prerequisites:

- CMake (version 3.10 or higher)
- GCC or Clang with C++17 support
- [Optional] Eclipse Paho MQTT C and C++ libraries (for MQTT communication)
- OpenSSL and libuv (if required by your MQTT library)
- Python 3 (only for utils)
- Internet connection
- Compatible operating system: **Linux** (tested on Ubuntu and Raspberry Pi OS)

## Installation Steps

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/PoliTOcean/Oceanix.git
    cd Oceanix
    ```

2. **Install Dependencies and Build the Project** (recommended):
    ```bash
    chmod +x install.sh
    ./install.sh
    ```

3. **OR Build the Project Manually**:
    ```bash
    mkdir -p build
    cd build
    cmake ..
    make
    ```

4. **(Optional) Use Devcontainer or Codespaces**:
    - Oceanix is fully configured for development in a [devcontainer](https://containers.dev/) or GitHub Codespaces. This provides a pre-configured environment with all dependencies.
    - Open the repository in Codespaces or with VS Code Remote - Containers to get started instantly.

## Running Oceanix

After building, run the main executable:

```bash
./build/oceanix
```

For test mode (simulation without hardware):

```bash
./build/oceanix test
```

See [../README.md](../README.md) and [usage.md](usage.md) for more details.

## Troubleshooting

- **Build fails:** Ensure all dependencies are installed and your compiler supports C++17.
- **MQTT issues:** Check your MQTT broker settings and network connectivity.
- **Configuration errors:** Validate your JSON files in `config/`.
- **Hardware not detected:** Make sure all serial connections are correct and permissions are set.

For more help, open an issue on GitHub or contact the maintainers.
