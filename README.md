# Oceanix

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux-blue.svg)]()

Oceanix is the main control software for the ROV **EVA** of [PoliTOcean](https://politocean.it), the underwater robotics team at Politecnico di Torino. Designed for modularity and extensibility, Oceanix aims to support not only EVA but also all future ROVs developed by the team.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Communication Protocol](#communication-protocol)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [License](#license)
- [Contact](#contact)

## Overview

Oceanix is a modular C++ software stack for ROVs, providing:
- Real-time control of thrusters and actuators
- Sensor data acquisition (IMU, barometer, etc.)
- Multiple control algorithms (Pole Placement, MIMO, etc.)
- Communication with a GUI via MQTT
- Modular configuration via JSON files
- Logging and system monitoring

The software is designed for Linux-based embedded systems (e.g., Raspberry Pi) and communicates with hardware via serial and other standard interfaces.

## Project Structure

The repository is organized as follows:

- `src/` — Main source code (C++ files for controllers, sensors, MQTT, etc.)
- `include/` — Header files for all modules
- `libs/` — External or internal libraries (BAR, Chimpanzee, Controller, IMU, etc.)
- `config/` — JSON configuration files
- `build/` — Build output (created after compilation)
- `log/` — Log files generated during operation
- `docs/` — Documentation (usage, configuration, communication, etc.)
- `install.sh` — Installation script (run this first)
- `build_and_test.sh` — Script to build and run tests
- `CMakeLists.txt` — CMake build configuration

See the [docs/](docs/) folder for more detailed documentation on each topic.

## Features

- **Modular Controller Support:** Easily switch between different control algorithms. Add new controllers by implementing the abstract controller interface in `include/controller_abstract.hpp` and registering them in the main code.
- **Sensor Integration:** Reads and processes data from IMU and barometer sensors. New sensors can be added by extending the sensor interface in `include/sensor.hpp`.
- **Motor Control:** Supports up to 8 thrusters with slew rate limiting and mixing matrix. Motor logic is in `src/motor.cpp` and `include/motor.hpp`.
- **MQTT Communication:** Robust protocol for GUI integration and remote operation. See `src/mqtt_client.cpp` and [docs/communication.md](docs/communication.md).
- **Configurable:** All parameters are set via JSON configuration files in `config/`. See [docs/configuration.md](docs/configuration.md).
- **Logging:** Flexible logging to file, console, and MQTT. Logging logic is in `src/logger.cpp` and `include/logger.hpp`.
- **Simulation/Test Mode:** Run the software without hardware for development and debugging. Use `./build/oceanix test`.
- **Extensible Architecture:** Add new modules (controllers, sensors, actuators) by following the interface patterns in `include/`.
- **Comprehensive Documentation:** See the `docs/` folder for guides on usage, configuration, communication, and contributing.

## Installation

See [docs/installation.md](docs/installation.md) for detailed instructions, dependencies, and troubleshooting.

### Quick Start

```bash
git clone https://github.com/PoliTOcean/Oceanix.git
cd Oceanix
chmod +x install.sh
./install.sh
```

Or build manually (requires CMake and a C++17 compiler):

```bash
mkdir -p build
cd build
cmake ..
make
```

#### Dependencies

- CMake >= 3.10
- GCC or Clang with C++17 support
- [Optional] MQTT library (see [docs/installation.md](docs/installation.md))

If you encounter build issues, see the [Troubleshooting](#troubleshooting) section below.

## Usage

After installation, run the main executable:

```bash
./build/oceanix
```

For test mode (simulation without hardware):

```bash
./build/oceanix test
```

Log files will be created in the `log/` directory. For command-line options and advanced usage, see [docs/usage.md](docs/usage.md).

## Configuration

All configuration is handled via JSON files in the `config/` directory. See [docs/configuration.md](docs/configuration.md) for a full description of available parameters and how to add new configuration options.

## Communication Protocol

Oceanix communicates with the GUI and other systems using MQTT and JSON messages. See [docs/communication.md](docs/communication.md) for the full protocol specification and message formats.

## Contributing

Contributions are welcome! Please read [docs/contribute.md](docs/contribute.md) for our coding standards and contribution guidelines.

### How to Contribute

- Fork the repository and create a new branch for your feature or bugfix.
- Follow the code style described in [docs/contribute.md](docs/contribute.md).
- Write clear commit messages and document your code.
- Open a pull request and describe your changes.
- For major changes, open an issue first to discuss your proposal.

### Adding New Modules

- **Controllers:** Implement the abstract controller interface in `include/controller_abstract.hpp` and add your implementation in `src/` and `include/`.
- **Sensors:** Extend the sensor interface in `include/sensor.hpp` and add your implementation.
- **Configuration:** Add new parameters to the relevant JSON files in `config/` and document them in [docs/configuration.md](docs/configuration.md).

## Testing

The project is fully configured to be tested in a devcontainer, you can use GitHub ones or Docker locally.

Automated and manual tests can be run using:

```bash
./build_and_test.sh
```

Test mode (simulation) can be run with:

```bash
./build/oceanix test
```

See [docs/usage.md](docs/usage.md) for more details on testing and simulation.

## Logging and Debugging

Log files are stored in the `log/` directory. Logging can be configured via the JSON files in `config/`. For debugging, use the test mode and check logs for errors or warnings.

## Changelog

See [docs/changelog.md](docs/changelog.md) for a history of changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For questions, suggestions, or to get involved, visit [politocean.it](https://politocean.it) or open an issue on GitHub.

---
Made with ❤️ by the PoliTOcean Team.
