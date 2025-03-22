// Start by adding open and close I2C with error handling
// Read + write registers
// bit number for pins
// Register address for pins
// Set input/output mode
// Read GPIO
// write GPIO
// Pullup
#include "hardware/i2c.h"
#include "MCP23017.h"

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance, uint8_t MCP23017_ADDRESS) {
    
    // Checks HW I2C is functional + Valid MCP23017 address range
    if (i2c_instance == NULL || MCP23017_ADDRESS < 0b0100000 || MCP23017_ADDRESS > 0b0100111) {
        return 1;
    }

    // Setup struct
    dev->i2c_instance= i2c_instance;
    dev->GPIOA[0] = 0;
    dev->GPIOA[1] = 0;
    dev->GPIOA[2] = 0;
    dev->GPIOA[3] = 0;

    dev->GPIOA[4] = 0;
    dev->GPIOA[5] = 0;
    dev->GPIOA[6] = 0;
    dev->GPIOA[7] = 0;

    dev->GPIOB[0] = 0;
    dev->GPIOB[1] = 0;
    dev->GPIOB[2] = 0;
    dev->GPIOB[3] = 0;

    dev->GPIOB[4] = 0;
    dev->GPIOB[5] = 0;
    dev->GPIOB[6] = 0;
    dev->GPIOB[7] = 0;
    return 0;
}

// Read all 16 GPIOs at once
uint8_t MCP23017_ReadIO(MCP23017 *dev) {
    uint8_t data[2] = 0;
    data[0] = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA); // Bank A
    data[1] = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOB); // Bank B
    return data;
}

// Writes all 16 IO at once
void MCP23017_WriteIO(MCP23017 *dev, uint8_t *data) {
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOA, &data[0]);
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOB, &data[1]);
}

// Set all IO direction
void MCP23017_SetIODirection(MCP23017 *dev, uint8_t *direction) {
    MCP23017_WriteRegister(dev,MCP23017_REG_IODIRA, &direction[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_IODIRB, &direction[1]);
}

void MCP23017_SetSingleIODirection(MCP23017 *dev, uint8_t *direction, uint8_t gpio) {
    /*
    1. Read current direction
    2. Check if they need updating
    3. Update if required
    */
   uint8_t current_direction = 0;
   uint8_t bitmask = 1;

   // Handle other values outside of 0 or 1
   if (direction > 1) {
    direction = 1;

    if (gpio <= 7) { // Bank A
        current_direction = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRA);
    }
    else { // Bank B
        gpio =- 8;
        current_direction = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRB);
    }
    for (uint8_t i = 0; i < gpio; i++) { // Shift bit to correct IO to change
        bitmask << i;
    }

    
    // Compare direction vs bank direction
    if ((direction == 1) && (current_direction != (current_direction | bitmask))) { // Change from 0 to 1
        current_direction = (current_direction | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IODIRA, &current_direction);
    }
    else if ((direction == 0) && (current_direction != (current_direction - bitmask))){ // Change from 1 to 0
        current_direction = (current_direction - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IODIRB, &current_direction);
    }
    else {
        return; // Do nothing
    }

}
}

// Set all IO polarity
void MCP23017_SetIOPolarity(MCP23017 *dev, uint8_t *polarity) {
    MCP23017_WriteRegister(dev,MCP23017_REG_IPOLA, &polarity[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_IPOLB, &polarity[1]);
}

// <polarity> can be 0 for no value inversion or 1 for inversion, <gpio> refers to the physical IO pin described in datasheet
void MCP23017_SetSingleIOPolarity(MCP23017 *dev, uint8_t *polarity, uint8_t gpio) {
    /*
    1. Read current polarity
    2. Check if they need updating
    3. Update if required
    */
    uint8_t current_polarity = 0; // Can't represent as byte? Should be 0000 0000
    uint8_t bitmask = 1; // Can't represent as byte? Should be 0000 0001

    // Handle other values outside of 0 or 1
    if (polarity > 1) {
        polarity = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_polarity = MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA);
    }
    else { // Bank B
        gpio =- 8;
        current_polarity = MCP23017_ReadRegister(dev, MCP23017_REG_IPOLB);
    }
    for (uint8_t i = 0; i < gpio; i++) { // Shift bit to correct IO to change
        bitmask << i;
    }

    // Compare polarity vs bank polarity
    if ((polarity == 1) && (current_polarity != (current_polarity | bitmask))) { // Change from 0 to 1
        current_polarity = (current_polarity | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IPOLA, &current_polarity);
    }
    else if ((polarity == 0) && (current_polarity != (current_polarity - bitmask))){ // Change from 1 to 0
        current_polarity = (current_polarity - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IPOLB, &current_polarity);
    }
    else {
        return; // Do nothing
    }
}

void MCP23017_SetPullups(MCP23017 *dev, uint8_t *pullup) {

}

void MCP23017_SetSinglePullup(MCP23017 *dev, uint8_t *pullup, uint8_t gpio) {

}

// Reads 1 byte into <data> from the register specified by <reg_address>
uint8_t MCP23017_ReadRegister(MCP23017 *dev, uint8_t reg_address) {
    uint8_t data;
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &reg_address,1, true);
    i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &data, 1, false);
    return data;
}

// Maybe not needed?
uint8_t MCP23017_ReadRegisters(MCP23017 *dev, uint8_t reg_address, uint8_t length) {
    uint8_t data[length];
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &reg_address, 1, true);
    i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &data, length, false);
    return data;
}

// Writes all of <data> to the register specified by <reg_address>
void MCP23017_WriteRegister(MCP23017 *dev, uint8_t reg_address, uint8_t *data) {
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, reg_address, 1, true);
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, data, sizeof(data), false);
}