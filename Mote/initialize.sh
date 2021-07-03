#!/bin/bash

# sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/run_cooja.sh

sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/run_tunslip.sh

#CoAP
sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/run_tunslip2.sh
sudo x-terminal-emulator -e python /home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/bridge.py

#MQTT

sudo x-terminal-emulator -e python /home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/mqtt-bridge.py
