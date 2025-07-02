# Usage

This guide explains how to run Oceanix, use its different modes, and interpret logs.

## Running Oceanix

1. Open your terminal.
2. Navigate to the build directory:
    ```sh
    cd Oceanix/build
    ```
3. Run the main program:
    ```sh
    ./oceanix
    ```
4. To run in test mode (simulation, no hardware required):
    ```sh
    ./oceanix test
    ```

## Command-Line Options

- `./oceanix` — Run with hardware (default mode)
- `./oceanix test` — Run in simulation/test mode (no hardware required)

## Log Files

Log files are created in the `log/` directory. Each run generates a new log file with a timestamp. Check these files for errors, warnings, and system status.
In `status/` directory a detailed log about the state of the ROV is written every time new thrust values are sent to the motors. Very useful for analyzing all the sensors afterwards. Can be enabled with a value = 0 in the config: `main_loglevel/`.

## Using in Devcontainer or Codespaces

If you are using a devcontainer or GitHub Codespaces, the environment is pre-configured. To enable test mode in Codespaces:

```sh
source Oceanix_test_mode.sh
```

## Troubleshooting

- **Program does not start:** Check that all dependencies are installed and the build completed successfully.
- **No hardware detected:** Use test mode or check your serial connections and permissions.
- **MQTT not working:** Verify broker address and network connectivity.

For more details, see [../README.md](../README.md) and [installation.md](installation.md).
