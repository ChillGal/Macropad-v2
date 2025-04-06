/*
 *
 *  MCP23017 GPIO Expander Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 22/03/2025
 *  Updated: 26/03/2025
 *  Revision: 0.1
 *  Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf
 * 
*/

#ifndef _MCP23017_H
#define _MCP23017_H

// I2C address
#define MCP23017_I2C_ADDRESS    0x20 // Default address is 0x20. Range from 0x20-0x27, bit ordering is 0 0 1 0 0 A2 A1 A0

// Register map - A and B refer to Banks A and B
#define MCP23017_REG_IODIRA     0x00 // IO direction
#define MCP23017_REG_IODIRB     0x01 
#define MCP23017_REG_IPOLA      0x02 // IO polarity
#define MCP23017_REG_IPOLB      0x03
#define MCP23017_REG_GPINTENA   0x04 // Enable GPIO interrupts
#define MCP23017_REG_GPINTENB   0x05
#define MCP23017_REG_DEFVALA    0x06 // Default Value
#define MCP23017_REG_DEFVALB    0x07
#define MCP23017_REG_INTCONA    0x08 // Interrupt on GPIO change aka Interrupt control
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
    uint8_t mcp23017_i2c_addr;
    uint16_t io_configuration;
    uint16_t io_direction;
    uint16_t io_polarity;
    uint16_t io_pullup;
    uint16_t io_interrupt;

} MCP23017;

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance, uint8_t MCP23017_ADDRESS);

// IO
uint8_t MCP23017_GetIO(MCP23017 *dev);
void MCP23017_SetIO(MCP23017 *dev, uint8_t *data);
uint8_t MCP23017_GetSingleIO(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleIO(MCP23017 *dev, uint8_t value, uint8_t gpio);

// IO direction
uint8_t MCP23017_GetIODirection(MCP23017 *dev, uint8_t bank);
void MCP23017_SetIODirection(MCP23017 *dev, uint8_t *direction);
uint8_t MCP23017_GetSingleIODirection(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleIODirection(MCP23017 *dev, uint8_t direction, uint8_t gpio);

// IO polarity
uint8_t MCP23017_GetIOPolarity(MCP23017 *dev, uint8_t bank);
void MCP23017_SetIOPolarity(MCP23017 *dev, uint8_t *polarity);
uint8_t MCP23017_GetSingleIOPolarity(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleIOPolarity(MCP23017 *dev, uint8_t polarity, uint8_t gpio);

// IO pullups
uint8_t MCP23017_GetPullups(MCP23017 *dev, uint8_t bank);
void MCP23017_SetPullups(MCP23017 *dev, uint8_t *pullup);
uint8_t MCP23017_GetSinglePullup(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSinglePullup(MCP23017 *dev, uint8_t pullup, uint8_t gpio);

// Interrupt on IO change
uint8_t MCP23017_GetInterruptChange(MCP23017 *dev, uint8_t bank);
void MCP23017_SetInterruptChange(MCP23017 *dev, uint8_t *interrupt);
uint8_t MCP23017_GetSingleInterruptChange(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleInterruptChange(MCP23017 *dev, uint8_t interrupt, uint8_t gpio);


// Default values to compare against for interrupt on change
uint8_t MCP23017_GetDefaults(MCP23017 *dev, uint8_t bank);
void MCP23017_SetDefaults(MCP23017 *dev, uint8_t *defaults);
uint8_t MCP23017_GetSingleDefault(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleDefault(MCP23017 *dev, uint8_t defaults, uint8_t gpio);

// GPIO interrupt enable
uint8_t MCP23017_GetInterruptEnable(MCP23017 *dev, uint8_t bank);
void MCP23017_SetInterruptEnable(MCP23017 *dev, uint8_t *interrupt);
uint8_t MCP23017_GetSingleInterruptEnable(MCP23017 *dev, uint8_t gpio);
void MCP23017_SetSingleInterruptEnable(MCP23017 *dev, uint8_t interrupt, uint8_t gpio);

// Direct register manipulation

// Read a single byte from specified register <reg_address>
uint8_t MCP23017_ReadRegister(MCP23017* dev, uint8_t reg_address);

// Read <length> bytes from specified register <reg_address>
uint8_t *MCP23017_ReadRegisters(MCP23017 *dev, uint8_t reg_address, uint8_t length);

// Write a single byte of data <data> to specified register <reg_address>
void MCP23017_WriteRegister(MCP23017 *dev, uint8_t reg_address, uint8_t *data);
#endif