## INC691-2018
All files are provided in this repository are used in INC691 class only.

----------

## What is the ECC-PIC24-CLI?
The **ecc-pic24-cli** is a command-line interface application used for embedded firmware development. Specifically, it is used as a development tool for the **ecc-pic24-board** 
## How to use the ECC-PIC24-CLI?
1.  Add the ecc-pic24-cli's directory, e.g.; *c:\\directory\\that\\contains\cli*, to the system path
2.  Run **CMD** or another console/terminal application, and change directory to the working directory, the directory that contains **main.c** and **config.cfg**
3.  Run the command *ecc-pic24-cli -help* to print help messages
----------
## What is the ECC-PIC24-BOARD?
The **ecc-pic24-board** is an microcontroller development board designed for embedded learners. The board composes of many inputs and outputs
![enter image description here](http://shadowwares.com/Shared/Images/github/ecc-pic24-board.png)


 - 4 Light Emitting Diodes (LEDs) used as Digital outputs
 - 4 Push Button Switches (PBSes) used as Digital inputs
 - 4 Potentiometers (POTs) used as Analog inputs (0-3.3 Volts)
 - 1 Buzzer used as PWM controlled sound generator
 - 1 Light Dependent Resistor (LDR) used as an ambient light sensor
 - 1 Digital Temperature Sensor used as an ambient temperature sensor
 - 1 USB-to-UART used as communication channel. A host computer and **ecc-pic24-cli** can communicate to the board as a UART device. This module is connected to the MCU through the UART1
 - 1 ESP8266, the **WiFi module**. The module is programmed to work in AT-command mode. The MCU communicate to this module using UART2. In addition, the module can be directly connected to a host computer by the jumpers
 - 3.3 Volts on-board regulator. The power supply of the board can be the USB or external dc-power supply (5-9 volts dc is preferred) 
 

