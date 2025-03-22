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

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance) {
    uint8_t i2c_instance = 0;
    return i2c_instance;
}

uint8_t MCP23017_ReadIO(MCP23017 *dev) {
    return 0;
}

uint8_t MCP23017_WriteIO(MCP23017 *dev) {
    return 0;
}

uint8_t MCP23017_SetIOMODE(MCP23017 *dev) {
    return 0;
}

uint8_t MCP23017_SetIOMODE(MCP23017 *dev) {
    return 0;
}

uint8_t MCP23017_ReadRegister(MCP23017 *dev, uint8_t register, uint8_t *data) {
    return i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, data, 1, false);
}

uint8_t MCP23017_ReadRegisters(MCP23017 *dev, uint8_t register, uint8_t *data, uint8_t length) {
    return i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, data, length, false);
}

uint8_t MCP23017_WriteRegister(MCP23017 *dev, uint8_t register, uint8_t *data) {
    return i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, data, sizeof(data), false);
}
