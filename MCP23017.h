/*
 *
 *  MCP23017 GPIO Expander Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 22/03/2025
 *  Revision: 0.1
 * 
*/

#ifndef _MCP23017_H
#define _MCP23017_H

#include "hardware/i2c.h" // I2C abstraction

#define MCP23017_I2C_ADDRESS    0x20 // Default address is 0x20. Range from 0x20-0x27, bit ordering is 0 0 1 0 0 A2 A1 A0

// Register Map - A and B refer to Banks A and B
#define MCP23017_REG_IODIRA     0x00 // IO direction
#define MCP23017_REG_IODIRB     0x01 
#define MCP23017_REG_IPOLA      0x02 // IO polarity
#define MCP23017_REG_IPOLB      0x03
#define MCP23017_REG_GPINTENA   0x04 // Enable GPIO interrupts
#define MCP23017_REG_GPINTENB   0x05
#define MCP23017_REG_DEFVALA    0x06 // Default Value
#define MCP23017_REG_DEFVALB    0x07
#define MCP23017_REG_INTCONA    0x08 // Interrupt on GPIO Change
#define MCP23017_REG_INTCONB    0x09
#define MCP23017_REG_IOCON      0x0A // IO expander configuration
#define MCP23017_REG_IOCON      0x0B
#define MCP23017_REG_GPPUA      0x0C // General purpose IO
#define MCP23017_REG_GPPUB      0x0D
#define MCP23017_REG_INTFA      0x0E // Interrupt flag
#define MCP23017_REG_INTFB      0x0F
#define MCP23017_REG_INTCAPA    0x10 // Interrupt captured value
#define MCP23017_REG_INTCAPB    0x11
#define MCP23017_REG_GPIOA      0x12 // GPIO values
#define MCP23017_REG_GPIOB      0x13
#define MCP23017_REG_OLATA      0x14 // Open Latching
#define MCP23017_REG_OLATB      0x15


#endif