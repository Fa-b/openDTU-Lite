# Introduction
This is a modified version of the openDTU project. The original project can be found [here](https://github.com/tbnobody/OpenDTU).
All credit goes to the original authors. The modifications are no-brainers and are only meant to use the core of openDTU with a ESP8266 module.

Most functionality has been stripped away, only the bare minimum is left to get openDTU to run on a ESP8266 module and/or to make space for additional features.
ESP32 is now also supported, but not tested yet.
NRF and CMT modules are supported, but only the NRF portion has been tested so far (on ESP8266).

# Usage
Browse the include folder for privates_template.h, fill in the required information and rename the file to privates.h.
The privates.h file is ignored by git, so you can safely store your private information in there.

In the same folder, you can find the pin_mapping.h file. This file contains some default pin mappings for configurations selected through platformio.ini environment variables. Make sure to check this file and change the pin mappings if needed.

Also there is a file called settings.h. Since there is no web interface anymore, some settings are now hardcoded in this file. You can change the settings here.

# Configuration
In order to avoid compiling the whole project for every configuration, the platformio.ini file contains some build flags. These flags are used to select the desired features. The following flags are available:

| Flag | Description |
| --- | --- |
| `USE_NRF` | Use the NRF24L01+ module |
| `USE_CMT` | Use the CMT2300a module |
| `HM_SERIES` | Enable usage with HM Series type inverters |
| `HMS_SERIES` | Enable usage with HMS Series type inverters |
| `HMT_SERIES` | Enable usage with HMT Series type inverters |
| `FUSION_V2` | Use the Fusion v2 Board |

# License
The original project is licensed under the GNU GPL v2.0 license. This project is licensed under the same license.