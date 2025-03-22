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

void MCP23017_SetIOMODE(MCP23017 *dev, uint8_t *mode) {

}

void MCP23017_SetPullups(MCP23017 *dev, uint8_t *pullup) {

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

