# Macropad-v2
20 key RGB ortholinear macropad with extra IO based on RP2040 with MCP23017.

## Features
### Complete
- Nothing yet

### In progress
- Add MCP23017 support and figure out interrupt handling
- Add Neopixel support

### To do 
- Add SD card support
- Add SSD1306 support
- Add USB HID support
- Add USB Serial support and support for key configuration over serial - maybe via local web server?
- Add Information(build instructions, pin configurations, specifications, etc) to README
- Add PCB design files 

### Known issues
- N/A

## Setup and build instructions
### Setup
This project was developed in VS Code with the Rasperry PI Pico Project extension.
Follow the setup guide within the official Raspberry Pi Pico SDK documentation to set up the environment.
https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html

### Build
Pending

### MCP23017 driver progress
Based on IOCON.BANK = 0 in datasheet
#### Registers Done
- IO Direction
- IO Polarity
- IO Interrupt Enable
- IO Default Values
- IO Interrupt Control (Interrupt on change)
- IO Values
- IO Pullups
#### Registers needing implementation
- Interrupt Flag
- Interrupt Captured Value
- Output Latching
- IO Expander Configuration
