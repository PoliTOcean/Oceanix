
# Configuration

Oceanix is configured via JSON files in the `config/` directory. The main file is `config.json`, which contains all system parameters. You can customize controllers, motors, logging, MQTT, and more. Below is a description of the main sections and parameters as of the latest version.

## Main Configuration Sections

- **Controller**: Contains all parameters for the available controllers (MIMO, PP, etc.).
  - `MIMO`: State-space controller gains and matrices (Admatrix_Gain, Bdmatrix_Gain, etc.).
  - `PP`: Pole Placement controller gains and physical parameters (Kp, Ki, Kd, buoyancy, etc.).
- **General**: System-wide settings, logging, MQTT, and main operational parameters.
  - Includes controller selection, log levels, MQTT server, log file options, and more.
- **Motors**: Parameters for each motor (PWM zero, direction, thrust limits, slew rate, etc.).

## Key Parameters

### Controller

- `MIMO`: Contains all state-space matrices and gains for the MIMO controller. These are advanced parameters and should only be changed by experienced users.
- `PP`: Contains all gains and physical parameters for the Pole Placement controller (e.g., `Kp_pitch`, `Ki_roll`, `buoyancy`, etc.).

### General

- `controller_type`: Selects which controller to use (`PP` or `MIMO`).
- `controller_profile`: Numeric profile for controller tuning.
- `logFileDir`: Directory for log files (default: `log/`).
- `logTypeCOUT`, `logTypeFILE`, `logTypeMQTT`: Enable/disable logging to console, file, and MQTT.
- `mqtt_server_addr`: Address of the MQTT broker.
- `mqtt_client_id`: MQTT client ID.
- `imu_pitch_offset`, `imu_roll_offset`: Calibration offsets for IMU.
- `debug_interval`, `motor_interval`: Main loop and motor update intervals (ms).
- `*_loglevel`: Log level for each subsystem (0=STATUS, 1=INFO, 2=WARNING, 3=ERROR).

### Motors

Each motor has a coefficient and a PWM zero value (for calibration):

- `FDX_coeff`, `FDX_pwm_zero`, ... (one for each motor)
- `thrust_max_xy`, `thrust_max_xy_work`: Max thrust for XY motors (normal/work mode)
- `thrust_max_z`, `thrust_max_z_work`: Max thrust for Z motors (normal/work mode)
- `thrust_slew_rate_max`: Maximum allowed change in thrust per update

## Parameter Reference

### Controller Section

- **MIMO**: Advanced users only. Contains all state-space matrices and gains for the MIMO controller.
- **PP**: Main gains for pitch/roll, buoyancy, heave dynamics, and output limits.

### General Section

- **controller_type**: `"PP"` or `"MIMO"` — selects the active controller.
- **controller_profile**: Integer for selecting a tuning profile.
- **logFileDir**: Directory for log files.
- **logTypeCOUT**: Enable logging to console.
- **logTypeFILE**: Enable logging to file.
- **logTypeMQTT**: Enable logging to MQTT.
- **mqtt_server_addr**: MQTT broker address.
- **mqtt_client_id**: MQTT client ID.
- **imu_pitch_offset**, **imu_roll_offset**: IMU calibration offsets.
- **debug_interval**: Main debug print interval (ms).
- **motor_interval**: Motor update interval (ms).
- ***_loglevel**: Log level for each subsystem (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG).

### Motors Section

- **[MOTOR]_coeff**: Direction coefficient for each motor (e.g., `FDX_coeff`).
- **[MOTOR]_pwm_zero**: PWM zero calibration for each motor (e.g., `FDX_pwm_zero`).
- **thrust_max_xy**, **thrust_max_xy_work**: Max thrust for XY motors (normal/work mode).
- **thrust_max_z**, **thrust_max_z_work**: Max thrust for Z motors (normal/work mode).
- **thrust_slew_rate_max**: Max allowed change in thrust per update.

## Adding New Configuration Options

1. Add the new parameter to `config.json`.
2. Update the relevant C++ code in `src/config.cpp` and `include/config.hpp` to parse and use the new parameter.
3. Document the new parameter in this file.

For a full description of all available parameters, see comments in `config/config.json` and the code in `src/config.cpp`.
