#!/bin/bash

# sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/run_cooja.sh

sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/Server/run_tunslip.sh

sudo x-terminal-emulator -e /home/user/GitHub/TemperatureControlWithIoT/Server/run_tunslip2.sh

sudo x-terminal-emulator -e python bridge.py
