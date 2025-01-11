# Meeple - ESP01

Repository for the control base of the game board project, using the ESP-01 module.

## Getting Started

### Prerequisites
To work with this project you need to install the following VSCode extensions:
- [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide): 
    Quick start guide can be found [here](https://docs.platformio.org/en/latest/integration/ide/vscode.html#quick-start).


### Import the project

- Clone this repository.
- Click on the PlatformIO icon in the left sidebar.
- Click on the "Open Project" button and select the peoject (folder tha contains the `platformio.ini` file).

### Install the dependencies
Dependencies will be automatically installed when you build the project for the first time, if not, you can install them manually:
- Click on the PlatformIO icon in the left sidebar. Or the home icon in the bottom bar (appears after starting the extension).

- Click on the "Library" button in the PlatformIO sidebar.
- Search for the following libraries and install them:
  - `PubSubClient` by Nick O'Leary

### Wifi Configuration
- When cloning the project, a file `src/config_example.h` will be created.
- Create a copy of this file and rename it to `src/config.h`.
- Fill in the `WIFI_SSID` and `WIFI_PASSWORD` fields with your wifi credentials.
- This new file is already included in the `.gitignore` file, so it will not be commited to the repository.


## Project Structure

- `src/main.cpp`: Main source code file
- `src/config.h`: Configuration file with wifi credentials
- `platformio.ini`: PlatformIO project configuration.