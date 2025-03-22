/*
 *
 *  MCP23017 GPIO Expander Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 22/03/2025
 *  Updated: 22/03/2025
 *  Revision: 0.1
 * 
*/

#ifndef _MCP23017_H
#define _MCP23017_H

#include "hardware/i2c.h" // I2C abstraction

// I2C address
#define MCP23017_I2C_ADDRESS    (0x20 << 1) // Default address is 0x20. Range from 0x20-0x27, bit ordering is 0 0 1 0 0 A2 A1 A0

// Register map - A and B refer to Banks A and B
#define MCP23017_REG_IODIRA     0x00 // IO direction
#define MCP23017_REG_IODIRB     0x01 
#define MCP23017_REG_IPOLA      0x02 // IO polarity
#define MCP23017_REG_IPOLB      0x03
#define MCP23017_REG_GPINTENA   0x04 // Enable GPIO interrupts
#define MCP23017_REG_GPINTENB   0x05
#define MCP23017_REG_DEFVALA    0x06 // Default Value
#define MCP23017_REG_DEFVALB    0x07
#define MCP23017_REG_INTCONA    0x08 // Interrupt on GPIO change
#define MCP23017_REG_INTCONB    0x09
#define MCP23017_REG_IOCONA     0x0A // IO expander configuration
#define MCP23017_REG_IOCONB     0x0B
#define MCP23017_REG_GPPUA      0x0C // GPIO pullups
#define MCP23017_REG_GPPUB      0x0D
#define MCP23017_REG_INTFA      0x0E // Interrupt flag
#define MCP23017_REG_INTFB      0x0F
#define MCP23017_REG_INTCAPA    0x10 // Interrupt captured value
#define MCP23017_REG_INTCAPB    0x11
#define MCP23017_REG_GPIOA      0x12 // GPIO values
#define MCP23017_REG_GPIOB      0x13
#define MCP23017_REG_OLATA      0x14 // Output Latching
#define MCP23017_REG_OLATB      0x15

typedef struct {
    
    // I2C instance/Handle
    i2c_inst_t *i2c_instance;

    // GPIO levels
    uint8_t GPIOA[8];
    uint8_t GPIOB[8];
    
} MCP23017;

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance, uint8_t MCP23017_ADDRESS);

uint8_t MCP23017_ReadIO(MCP23017 *dev);
void MCP23017_WriteIO(MCP23017 *dev, uint8_t *data);
void MCP23017_SetIOMODE(MCP23017 *dev, uint8_t *mode);
void MCP23017_SetPullups(MCP23017 *dev, uint8_t *pullup);

// Direct register manipulation

// Read a single byte from specified register <reg_address>
uint8_t MCP23017_ReadRegister(MCP23017 *dev, uint8_t reg_address); 

// Read <length> bytes from specified register <reg_address>
uint8_t MCP23017_ReadRegisters(MCP23017 *dev, uint8_t reg_address, uint8_t length);

// Write a single byte of data <data> to specified register <reg_address>
void MCP23017_WriteRegister(MCP23017 *dev, uint8_t reg_address, uint8_t *data);
#endif