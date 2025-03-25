/*
 *
 *  MCP23017 GPIO Expander Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 22/03/2025
 *  Updated: 25/03/2025
 *  Revision: 0.1
 *  Datasheet: https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf
 * 
*/

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
#include "pico/stdlib.h"
#include <stdlib.h>

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance, uint8_t MCP23017_ADDRESS) {
    
    // Checks HW I2C is functional + Valid MCP23017 address range
    if (i2c_instance == NULL || MCP23017_ADDRESS < 0b0100000 || MCP23017_ADDRESS > 0b0100111) { // 0x20 to 0x27
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
uint8_t *MCP23017_GetIO(MCP23017 *dev) {
    uint8_t *data = malloc(sizeof(uint8_t) * 2);
    data[0] = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA); // Bank A
    data[1] = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOB); // Bank B
    return data;
}

// Writes all 16 IO at once
void MCP23017_SetIO(MCP23017 *dev, uint8_t *data) {
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOA, &data[0]);
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOB, &data[1]);
}

uint8_t MCP23017_GetSingleIO(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOB);
    }
    bitmask = bitmask << gpio;
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}   

void MCP23017_SetSingleIO(MCP23017 *dev, uint8_t io, uint8_t gpio) {
    uint8_t current_io = 0;
    uint8_t bitmask = 1;

    // Handle other values outside of 0 or 1
    if (io > 1) {
        io = 1;
    }
    if (gpio <= 7) { // Bank A
        current_io = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRA);
    }
    else { // Bank B
        gpio =- 8;
        current_io = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare current value vs 
    if ((io == 1) && (current_io != (current_io | bitmask))) { // Change from 0 to 1
        current_io = (current_io | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IODIRA, &current_io);
    }
    else if ((io == 0) && (current_io != (current_io - bitmask))){ // Change from 1 to 0
        current_io = (current_io - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IODIRB, &current_io);
    }
    else {
        return; // Do nothing
    }
}

uint8_t MCP23017_GetIODirection(MCP23017 *dev, uint8_t bank) {
    // Determine Bank to read
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_GPIOB);
    }
}

// Set all IO direction
void MCP23017_SetIODirection(MCP23017 *dev, uint8_t *direction) {
    MCP23017_WriteRegister(dev,MCP23017_REG_IODIRA, &direction[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_IODIRB, &direction[1]);
}

// Get value of IO specified by <gpio>
uint8_t MCP23017_GetSingleIODirection(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRB);
    }
    bitmask = bitmask << gpio;
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

// <gpio> refers to the physical IO pin described in datasheet
void MCP23017_SetSingleIODirection(MCP23017 *dev, uint8_t direction, uint8_t gpio) {
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
    }
    if (gpio <= 7) { // Bank A
        current_direction = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRA);
    }
    else { // Bank B
        gpio =- 8;
        current_direction = MCP23017_ReadRegister(dev, MCP23017_REG_IODIRB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

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

// Get IO polarity based on Bank - Bank A is IO 0-7, Bank B is IO 8-15
uint8_t MCP23017_GetIOPolarity(MCP23017 *dev, uint8_t bank) {
    // Determine Bank to read
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_IPOLB);
    }
}

// Set all IO polarity
void MCP23017_SetIOPolarity(MCP23017 *dev, uint8_t *polarity) {
    MCP23017_WriteRegister(dev,MCP23017_REG_IPOLA, &polarity[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_IPOLB, &polarity[1]);
}

// Get IO polarity based on GPIO
uint8_t MCP23017_GetSingleIOPolarity(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA);
    }
    bitmask = bitmask << gpio;
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

// <polarity> can be 0 for no value inversion or 1 for inversion, <gpio> refers to the physical IO pin described in datasheet
void MCP23017_SetSingleIOPolarity(MCP23017 *dev, uint8_t polarity, uint8_t gpio) {
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
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

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

uint8_t MCP23017_GetPullups(MCP23017 *dev, uint8_t bank) {
    // Determine Bank to read
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_IPOLB);
    }
}

// Set all IO pullups
void MCP23017_SetPullups(MCP23017 *dev, uint8_t *pullup) {
    MCP23017_WriteRegister(dev,MCP23017_REG_GPPUA, &pullup[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_GPPUB, &pullup[1]);
}

// Get single IO determined by <gpio>
uint8_t MCP23017_GetSinglePullup(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPPUA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPPUB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSinglePullup(MCP23017 *dev, uint8_t pullup, uint8_t gpio) {
    uint8_t current_pullup = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (pullup > 1) {
        pullup = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_pullup = MCP23017_ReadRegister(dev, MCP23017_REG_GPPUA);
    }
    else { // Bank B
        gpio =- 8;
        current_pullup = MCP23017_ReadRegister(dev, MCP23017_REG_GPPUB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((pullup == 1) && (current_pullup != (current_pullup | bitmask))) { // Change from 0 to 1
        current_pullup = (current_pullup | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_GPPUA, &current_pullup);
    }
    else if ((pullup == 0) && (current_pullup != (current_pullup - bitmask))){ // Change from 1 to 0
        current_pullup = (current_pullup - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_GPPUB, &current_pullup);
    }
    else {
        return; // Do nothing
    }
}

uint8_t MCP23017_GetInterruptChange(MCP23017 *dev, uint8_t bank) {
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_INTCONA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_INTCONB);
    }
}

void MCP23017_SetInterruptChange(MCP23017 *dev, uint8_t *interrupt) {
    MCP23017_WriteRegister(dev,MCP23017_REG_INTCONA, &interrupt[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_INTCONB, &interrupt[1]);
}

uint8_t MCP23017_GetSingleInterruptChange(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTCONA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTCONB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleInterruptChange(MCP23017 *dev, uint8_t interrupt, uint8_t gpio) {
    uint8_t current_interrupt = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (interrupt > 1) {
        interrupt = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_interrupt = MCP23017_ReadRegister(dev, MCP23017_REG_INTCONA);
    }
    else { // Bank B
        gpio =- 8;
        current_interrupt = MCP23017_ReadRegister(dev, MCP23017_REG_INTCONB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((interrupt == 1) && (current_interrupt != (current_interrupt | bitmask))) { // Change from 0 to 1
        current_interrupt = (current_interrupt | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_INTCONA, &current_interrupt);
    }
    else if ((interrupt == 0) && (current_interrupt != (current_interrupt - bitmask))){ // Change from 1 to 0
        current_interrupt = (current_interrupt - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_INTCONB, &current_interrupt);
    }
    else {
        return; // Do nothing
    }
}

uint8_t MCP23017_GetDefaults(MCP23017 *dev, uint8_t bank) {
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALB);
    }
}

void MCP23017_SetDefaults(MCP23017 *dev, uint8_t *defaults) {
    MCP23017_WriteRegister(dev,MCP23017_REG_DEFVALA, &defaults[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_DEFVALB, &defaults[1]);
}

uint8_t MCP23017_GetSingleDefault(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleDefault(MCP23017 *dev, uint8_t defaults, uint8_t gpio) {
    uint8_t current_defaults = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (defaults > 1) {
        defaults = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_defaults = MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALA);
    }
    else { // Bank B
        gpio =- 8;
        current_defaults = MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((defaults == 1) && (current_defaults != (current_defaults | bitmask))) { // Change from 0 to 1
        current_defaults = (current_defaults | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_DEFVALA, &current_defaults);
    }
    else if ((defaults == 0) && (current_defaults != (current_defaults - bitmask))){ // Change from 1 to 0
        current_defaults = (current_defaults - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_DEFVALB, &current_defaults);
    }
    else {
        return; // Do nothing
    }
}

uint8_t MCP23017_GetInterruptEnable(MCP23017 *dev, uint8_t bank) {
    if (bank = 0) {
        return MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENA);
    }
    else {
        return MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENB);
    }
}

void MCP23017_SetInterruptEnable(MCP23017 *dev, uint8_t *interrupt) {
    MCP23017_WriteRegister(dev,MCP23017_REG_GPINTENA, &interrupt[0]);
    MCP23017_WriteRegister(dev,MCP23017_REG_GPINTENB, &interrupt[1]);
}

uint8_t MCP23017_GetSingleInterruptEnable(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleInterruptEnable(MCP23017 *dev, uint8_t interrupt, uint8_t gpio) {
    uint8_t current_enable_interrupt = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (interrupt > 1) {
        interrupt = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_enable_interrupt = MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENA);
    }
    else { // Bank B
        gpio =- 8;
        current_enable_interrupt = MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((interrupt == 1) && (current_enable_interrupt != (current_enable_interrupt | bitmask))) { // Change from 0 to 1
        current_enable_interrupt = (current_enable_interrupt | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_GPINTENA, &current_enable_interrupt);
    }
    else if ((interrupt == 0) && (current_enable_interrupt != (current_enable_interrupt - bitmask))){ // Change from 1 to 0
        current_enable_interrupt = (current_enable_interrupt - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_GPINTENB, &current_enable_interrupt);
    }
    else {
        return; // Do nothing
    }
}

// Reads 1 byte into <data> from the register specified by <reg_address>
uint8_t MCP23017_ReadRegister(MCP23017 *dev, uint8_t reg_address) {
    uint8_t data = 0;
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &reg_address,1, true);
    i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &data, 1, false);
    return data;
}

// Maybe not needed?
uint8_t *MCP23017_ReadRegisters(MCP23017 *dev, uint8_t reg_address, uint8_t length) {
    uint8_t *data = malloc(sizeof(uint8_t) * length);
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &reg_address, 1, true);
    for (int i = 0; i < length; i++) {
        i2c_read_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &data[i], length, false);
    }
    return data;
}

// Writes all of <data> to the register specified by <reg_address>
void MCP23017_WriteRegister(MCP23017 *dev, uint8_t reg_address, uint8_t *data) {
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, &reg_address, 1, true);
    i2c_write_blocking(dev->i2c_instance, MCP23017_I2C_ADDRESS, data, sizeof(data), false);
}