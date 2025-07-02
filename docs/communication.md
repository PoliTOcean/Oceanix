# Communication Protocol

## Overview

This document describes the MQTT-based communication protocol between the **GUI** and **Oceanix**. All messages exchanged MUST follow a JSON-like structure, adhering to the specified topics and key-value conventions.

---

## MQTT Topics and Message Format

Each topic below defines the permissible keys and values, as well as the direction of communication (GUI -> Oceanix or Oceanix -> GUI). Message structures are detailed to ensure consistency.

### Topics and Message Structure

1. ### **axes/**
   - **Direction**: GUI → Oceanix
   - **Description**: Sends position and orientation values from the GUI to Oceanix.
   - **Keys**:
     - `X`: Integer from -32678 to 32678
     - `Y`: Integer from -32678 to 32678
     - `Z`: Integer from -32678 to 32678
     - `YAW`: Integer from -32678 to 32678
     - `PITCH`: Integer from -32678 to 32678
     - `ROLL`: Integer from -32678 to 32678
   - **Example Message**:
     ```json
     {
       "X": 0,
       "Y": 0,
       "Z": 0,
       "YAW": 0,
       "ROLL" : 0,
       "PITCH" : 0
     }
     ```

2. ### **state_commands/**
   - **Direction**: GUI → Oceanix
   - **Description**: Sends state commands to Oceanix, typically in response to user actions in the GUI.
   - **Structure**:
     - `command_name`: The name of the command, which may correspond to a button pressed in the GUI.
     - `value`: Optional
    - **Possible command_name**:
      - `ARM_ROV`: Arms or disarms the ROV. The value is irrelevant.
      - `CHANGE_CONTROLLER_STATUS`: Changes the controller status. The value is irrelevant.
      - `PITCH_REFERENCE_UPDATE`: Updates the pitch reference value.
      - `ROLL_REFERENCE_UPDATE`: Updates the roll reference value.
      - `DEPTH_REFERENCE_UPDATE`: Updates the depth reference value.
      - `PITCH_REFERENCE_OFFSET`: Offsets the current pitch reference value.
      - `ROLL_REFERENCE_OFFSET`: Offsets the current roll reference value.
      - `DEPTH_REFERENCE_OFFSET`: Offsets the current depth reference value.
      - `THRUST_MAX_OFFSET`: Offsets the maximum thrust value.
      - `WORK_MODE`: Enables and disables motors work mode, setting motors thrust_max_xy and thrust_max_z to 2.9 and 2.0 or reverting them to the default values.
      - `REQUEST_CONFIG`: Request current configuration, is sent on config/ topic
    - **Example Message**:
      ```json
      {
        "PITCH_REFERENCE_UPDATE": 10.5
      }
      ```

3. ### **arm_commands/**
   - **Direction**: GUI → Oceanix
   - **Description**: Sends commands for the arm control to Oceanix, typically in response to button presses in the GUI.
   - **Keys**:
     - `command`: command
     - `command_name`: The name of the command, indicating the arm action.
  - **Possible command_name**:
    - `ROTATE_WRIST_CCW`: Rotate the wrist counterclockwise.
    - `ROTATE_WRIST_CW`: Rotate the wrist clockwise.
    - `STOP_WRIST`: Stop the wrist movement.
    - `OPEN_NIPPER`: Open the nipper.
    - `CLOSE_NIPPER`: Close the nipper.
    - `STOP_NIPPER`: Stop the nipper movement.
    - `TORQUE_WRIST_ON`: Turn on the wrist torque.
    - `TORQUE_WRIST_OFF`: Turn off the wrist torque.
   - **Example Message**:
     ```json
     {
       "command": "ROTATE_WRIST_CCW"
     }
     ```

4. ### **status/**
   - **Direction**: GUI ← Oceanix
   - **Description**: Sends status information from Oceanix to the GUI at a configurable interval.
   - **Keys**:
     - `variable_name`: The name of the variable being monitored.
     - `value`: Current value of the variable
   - **Information Types**:
     - **Heartbeat**: Essential information required for GUI functionality.
    - **Keys**:
      - `rov_armed` : `["OK", "OFF"]`
      - `nucleo_connected` : `["OK", "OFF"]`
      - `work_mode` : `["OK", "OFF"]`
      - `controller_state`
        - `DEPTH` : `["ACTIVE", "READY", "OFF"]`
        - `ROLL` : `["ACTIVE", "READY", "OFF"]`
        - `PITCH` : `["ACTIVE", "READY", "OFF"]`
      - `force_z` : Newton
      - `force_roll` : Newton
      - `force_pitch` : Newton
      - `reference_z` : meters
      - `reference_roll` : DEG
      - `reference_pitch` : DEG
      - `depth` : meters
      - `roll` : DEG
      - `pitch` : DEG
      - `yaw` : DEG
      - `imu_state` : `["OK", "OFF"]`
      - `bar_state` : `["OK", "OFF"]`
      - `motor_thrust_max_xy` : Kgf
      - `motor_thrust_max_z` : Kgf
      - `motor_thrust`
        - `FSX` : Kgf
        - `FDX` : Kgf
        - `RSX` : Kgf
        - `RDX` : Kgf
        - `UPFSX` : Kgf
        - `UPFDX` : Kgf
        - `UPRSX` : Kgf
        - `UPRDX` : Kgf
      - `pwm`
        - `FSX`
        - `FDX`
        - `RSX`
        - `RDX`
        - `UPFSX`
        - `UPFDX`
        - `UPRSX`
        - `UPRDX`
     - **Optional Debug**: Additional data useful for debugging purposes.
   - **Example Message**:
     ```json
     {
       "rov_armed": "OK",
       "depth": 2.8
     }
     ```

5. ### **config/**
   - **Direction**: GUI ⇔ Oceanix
   - **Description**: Used to synchronize configuration data between GUI and Oceanix.
   - **Keys**:
     - `config_variable`: The name of the configuration variable.
     - `value`: The current or updated value of the configuration variable.
   - **Behavior**:
     - when the command is received `REQUEST_CONFIG` Oceanix sends the current configuration to the GUI.
     - The GUI can send updates to modify the configuration and send it back to Oceanix on this topic.
     - When a configuration is received before writing it's validity is checked.
   - **Example Message**:
     ```json
     {
       "controller_profile" : 1 
     }
     ```

6. ### **log/**
   - **Direction**: GUI → Oceanix
   - **Description**: Send log message from Oceanix. The log level can be changed in the config.
   - **Example Message**:
     ```
       1746649287972[CONFIG ][INFO ]Reading config file\n
     ```
---

## Future Extensions

This structure is designed for future modifications, including additional keys and topics that may be required as the protocol evolves.
