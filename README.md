# Macropad-v2
20 key RGB ortholinear macropad with extra IO based on RP2040 with MCP23017.

## Features
### Complete
- Initial MCP23017 driver support

### In progress
- Add interrupt handling to MCP23017
- Add Neopixel support
- Add SSD1306 support

### To do 
- Add SD card support
- Add USB HID support
- Add USB Serial support and support for key configuration over serial - maybe via local web server?
- Add Information(build instructions, pin configurations, specifications, etc) to README
- Add PCB design files 
- Add Button press handling

### Known issues
- N/A

## Setup and build instructions
### Setup
This project was developed in VS Code with the Rasperry PI Pico Project extension.
Follow the setup guide within the official Raspberry Pi Pico SDK documentation to set up the environment.
https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html

### Build
Pending

## Implementation progress
### MCP23017 driver
Initial implementation is done. Further optimisation to be performed later
Based on IOCON.BANK = 0 in datasheet
#### Registers implemented
- IO Direction
- IO Polarity
- IO Interrupt Enable
- IO Default Values
- IO Interrupt Control (Interrupt on change)
- IO Values
- IO Pullups
- Interrupt Flag
- Interrupt Captured Value
- IO Expander Configuration

### SSD1306 driver
Intial implementation started.
