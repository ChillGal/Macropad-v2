/*
 *
 *  MCP23017 GPIO Expander Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 22/03/2025
 *  Updated: 12/04/2025
 *  Revision: 0.0.1
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
#include <stdio.h>
#include "hardware/i2c.h"
#include "MCP23017.h"
#include "pico/stdlib.h"
//#include "pico/malloc.h"
#include <malloc.h>

uint8_t MCP23017_Initialise(MCP23017 *dev, i2c_inst_t *i2c_instance, uint8_t mcp23017_address) {
    // Checks HW I2C is functional + Valid MCP23017 address range
    if (i2c_instance == NULL || mcp23017_address < 0b0100000 || mcp23017_address > 0b0100111) { // 0x20 to 0x27
        return 1;
    }

    // Setup struct
    dev->i2c_instance= i2c_instance;
    dev->mcp23017_i2c_addr = mcp23017_address;
    dev->io_value = 0;
    dev->io_configuration = 0;
    dev->io_direction = 0;
    dev->io_polarity = 0;
    dev->io_pullup = 0;
    dev->io_interrupt_chg = 0;
    dev->io_interrupt_en = 0;
    dev->io_output_latch = 0;
    dev->io_interrupt_flag = 0;
    dev->io_interrupt_cap = 0;
    dev->expander_config = 0;

    return 0;
}

// Read all 16 GPIOs at once
uint16_t MCP23017_GetIO(MCP23017 *dev) {
    uint16_t ioVal = (MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_GPIOB);
    dev->io_value = ioVal;
    return ioVal;
}

// Writes all 16 IO at once
void MCP23017_SetIO(MCP23017 *dev, uint16_t *data) {
    uint8_t *ptrIO = malloc(sizeof(uint8_t));
    *ptrIO = *data >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOA, ptrIO);
    *ptrIO = *data;
    MCP23017_WriteRegister(dev, MCP23017_REG_GPIOB, ptrIO);
    free(ptrIO);
}

uint8_t MCP23017_GetSingleIO(MCP23017 *dev, uint8_t gpio) {
    uint8_t data;
    uint8_t bitmask = 1; // 0000 0001
    
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_GPIOA);
    }
    else { // Bank B
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

uint16_t MCP23017_GetIODirection(MCP23017 *dev) {
    uint16_t ioDir = (MCP23017_ReadRegister(dev, MCP23017_REG_IODIRA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_IODIRB);
    dev->io_direction = ioDir;
    return ioDir;
}

// Set all IO direction
void MCP23017_SetIODirection(MCP23017 *dev, uint16_t *direction) {
    uint8_t *ptrDir = malloc(sizeof(uint8_t));
    *ptrDir = *direction >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_IODIRA, ptrDir);
    *ptrDir = *direction;
    MCP23017_WriteRegister(dev, MCP23017_REG_IODIRB, ptrDir);
    free(ptrDir);
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
uint16_t MCP23017_GetIOPolarity(MCP23017 *dev) {
    uint16_t ioPol = (MCP23017_ReadRegister(dev, MCP23017_REG_IPOLA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_IPOLB);
    dev->io_polarity = ioPol;
    return ioPol;

}

// Set all IO polarity
void MCP23017_SetIOPolarity(MCP23017 *dev, uint16_t *polarity) {
    uint8_t *ptrPol = malloc(sizeof(uint8_t));
    *ptrPol = *polarity >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_IPOLA, ptrPol);
    *ptrPol = *polarity;
    MCP23017_WriteRegister(dev, MCP23017_REG_IPOLB, ptrPol);
    free(ptrPol);
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

uint16_t MCP23017_GetPullups(MCP23017 *dev) {
    uint16_t ioPull = (MCP23017_ReadRegister(dev, MCP23017_REG_GPPUA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_GPPUB);
    dev->io_value = ioPull;
    return ioPull;
}

// Set all IO pullups
void MCP23017_SetPullups(MCP23017* dev, uint16_t* pullup) {
    uint8_t *ptrPullup = malloc(sizeof(uint8_t));
    *ptrPullup = *pullup >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_GPPUA, ptrPullup);
    *ptrPullup = *pullup;
    MCP23017_WriteRegister(dev, MCP23017_REG_GPPUB, ptrPullup);
    free(ptrPullup);
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

uint16_t MCP23017_GetInterruptChange(MCP23017 *dev) {
    uint16_t intChg = (MCP23017_ReadRegister(dev, MCP23017_REG_INTCONA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_INTCONB);
    dev->io_interrupt_chg = intChg;
    return intChg;
}

void MCP23017_SetInterruptChange(MCP23017 *dev, uint16_t *interrupt) {
    uint8_t *ptrIntChg = malloc(sizeof(uint8_t));
    *ptrIntChg = *interrupt >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_INTCONA, ptrIntChg);
    *ptrIntChg = *interrupt;
    MCP23017_WriteRegister(dev, MCP23017_REG_INTCONB, ptrIntChg);
    free(ptrIntChg);
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

uint16_t MCP23017_GetDefaults(MCP23017 *dev) {
    uint16_t defVal = (MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_DEFVALB);
    dev->io_interrupt_chg = defVal;
    return defVal;
}

void MCP23017_SetDefaults(MCP23017 *dev, uint16_t *defaults) {
    uint8_t *ptrDefVal = malloc(sizeof(uint8_t));
    *ptrDefVal = *defaults >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_DEFVALA, ptrDefVal);
    *ptrDefVal = *defaults;
    MCP23017_WriteRegister(dev, MCP23017_REG_DEFVALB, ptrDefVal);
    free(ptrDefVal);
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

uint16_t MCP23017_GetInterruptEnable(MCP23017 *dev) {
    uint16_t intEn = (MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_GPINTENB);
    dev->io_interrupt_en = intEn;
    return intEn;
}

void MCP23017_SetInterruptEnable(MCP23017 *dev, uint16_t *interrupt) {
    uint8_t *ptrIntEn = malloc(sizeof(uint8_t));
    *ptrIntEn = *interrupt >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_GPINTENA, ptrIntEn);
    *ptrIntEn = *interrupt;
    MCP23017_WriteRegister(dev, MCP23017_REG_GPINTENB, ptrIntEn);
    free(ptrIntEn);
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

uint16_t MCP23017_GetOutputLatch(MCP23017 *dev) {
    uint16_t OutLatch = (MCP23017_ReadRegister(dev, MCP23017_REG_OLATA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_OLATB);
    dev->io_output_latch = OutLatch;
    return OutLatch;
}

void MCP23017_SetOutputLatch(MCP23017 *dev, uint16_t *OutputLatch) {
    uint8_t *ptrOutLatch = malloc(sizeof(uint8_t));
    *ptrOutLatch = *OutputLatch >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_OLATA, ptrOutLatch);
    *ptrOutLatch = *OutputLatch;
    MCP23017_WriteRegister(dev, MCP23017_REG_OLATB, ptrOutLatch);
    free(ptrOutLatch);
}

uint8_t MCP23017_GetSingleOutputLatch(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_OLATA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_OLATB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleOutputLatch(MCP23017 *dev, uint8_t interrupt, uint8_t gpio) {
    uint8_t current_output_latch = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (interrupt > 1) {
        interrupt = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_output_latch = MCP23017_ReadRegister(dev, MCP23017_REG_OLATA);
    }
    else { // Bank B
        gpio =- 8;
        current_output_latch = MCP23017_ReadRegister(dev, MCP23017_REG_OLATB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((interrupt == 1) && (current_output_latch != (current_output_latch | bitmask))) { // Change from 0 to 1
        current_output_latch = (current_output_latch | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_OLATA, &current_output_latch);
    }
    else if ((interrupt == 0) && (current_output_latch != (current_output_latch - bitmask))){ // Change from 1 to 0
        current_output_latch = (current_output_latch - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_OLATB, &current_output_latch);
    }
    else {
        return; // Do nothing
    }
}

uint16_t MCP23017_GetIOExpanderConfiguration(MCP23017 *dev) {
    uint16_t IOExpConfig = (MCP23017_ReadRegister(dev, MCP23017_REG_IOCONA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_IOCONB);
    dev->expander_config = IOExpConfig;
    return IOExpConfig;
}

void MCP23017_SetIOExpanderConfiguration(MCP23017 *dev, uint16_t *IOConfiguration) {
    uint8_t *ptrIOExpConfig = malloc(sizeof(uint8_t));
    *ptrIOExpConfig = *IOConfiguration >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_IOCONA, ptrIOExpConfig);
    *ptrIOExpConfig = *IOConfiguration;
    MCP23017_WriteRegister(dev, MCP23017_REG_IOCONB, ptrIOExpConfig);
    free(ptrIOExpConfig);
}

uint8_t MCP23017_GetSingleIOExpanderConfiguration(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IOCONA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_IOCONB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleIOExpanderConfiguration(MCP23017 *dev, uint8_t configuration, uint8_t gpio) {
    uint8_t current_io_expander_config = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (configuration > 1) {
        configuration = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_io_expander_config = MCP23017_ReadRegister(dev, MCP23017_REG_IOCONA);
    }
    else { // Bank B
        gpio =- 8;
        current_io_expander_config = MCP23017_ReadRegister(dev, MCP23017_REG_IOCONB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((configuration == 1) && (current_io_expander_config != (current_io_expander_config | bitmask))) { // Change from 0 to 1
        current_io_expander_config = (current_io_expander_config | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IOCONA, &current_io_expander_config);
    }
    else if ((configuration == 0) && (current_io_expander_config != (current_io_expander_config - bitmask))){ // Change from 1 to 0
        current_io_expander_config = (current_io_expander_config - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_IOCONB, &current_io_expander_config);
    }
    else {
        return; // Do nothing
    }
}

uint16_t MCP23017_GetInterruptCapture(MCP23017 *dev) {
    uint16_t IntCap = (MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPB);
    dev->io_interrupt_cap = IntCap;
    return IntCap;
}

void MCP23017_SetInterruptCapture(MCP23017 *dev, uint16_t *InterruptCapture) {
    uint8_t *ptrIntCap = malloc(sizeof(uint8_t));
    *ptrIntCap = *InterruptCapture >> 8;
    // Bit ordering: AAAA AAAA BBBB BBBB
    MCP23017_WriteRegister(dev, MCP23017_REG_INTCAPA, ptrIntCap);
    *ptrIntCap = *InterruptCapture;
    MCP23017_WriteRegister(dev, MCP23017_REG_INTCAPB, ptrIntCap);
    free(ptrIntCap);
}

uint8_t MCP23017_GetSingleInterruptCapture(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

void MCP23017_SetSingleInterruptCapture(MCP23017 *dev, uint8_t interrupt, uint8_t gpio) {
    uint8_t current_interrupt_capture = 0;
    uint8_t bitmask = 1; // 0000 0001

    // Handle other values outside of 0 or 1
    if (interrupt > 1) {
        interrupt = 1;
    }
    
    // Set the bitmask and get current polarity
    if (gpio <= 7) { // Bank A
        current_interrupt_capture = MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPA);
    }
    else { // Bank B
        gpio =- 8;
        current_interrupt_capture = MCP23017_ReadRegister(dev, MCP23017_REG_INTCAPB);
    }
    bitmask = bitmask << gpio; // Shift bit to correct IO to change

    // Compare polarity vs bank polarity
    if ((interrupt == 1) && (current_interrupt_capture != (current_interrupt_capture | bitmask))) { // Change from 0 to 1
        current_interrupt_capture = (current_interrupt_capture | bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_INTCAPA, &current_interrupt_capture);
    }
    else if ((interrupt == 0) && (current_interrupt_capture != (current_interrupt_capture - bitmask))){ // Change from 1 to 0
        current_interrupt_capture = (current_interrupt_capture - bitmask);
        MCP23017_WriteRegister(dev,MCP23017_REG_INTCAPB, &current_interrupt_capture);
    }
    else {
        return; // Do nothing
    }
}

// Read only Registers
// Read only, Updated when an interrupt occurs, remains unchanged until cleared by reading GPIO or INTCAP
uint16_t MCP23017_GetInterruptFlag(MCP23017 *dev) {
    uint16_t intFlag = (MCP23017_ReadRegister(dev, MCP23017_REG_INTFA) << 8) + MCP23017_ReadRegister(dev, MCP23017_REG_INTFB);
    dev->io_interrupt_flag = intFlag;
    return intFlag;
}
// Read only, Updated when an interrupt occurs, remains unchanged until cleared by reading GPIO or INTCAP
uint8_t MCP23017_GetSingleInterruptFlag(MCP23017 *dev, uint8_t gpio) {
    uint8_t data = 0;
    uint8_t bitmask = 1; // 0000 0001
    // Read Bank and extract bit
    if (gpio < 7) { // Bank A
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTFA);
    }
    else{ // Bank B
        gpio =-8; // shift to keep within 0-7
        data = MCP23017_ReadRegister(dev, MCP23017_REG_INTFB);
    }
    bitmask = bitmask << gpio; // Bit shift
    if (bitmask == (data & bitmask)) { // Means bit is 1
        return 1;
    }
    else {
        return 0;
    }
}

// Reads 1 byte into <data> from the register specified by <reg_address>
uint8_t MCP23017_ReadRegister(MCP23017* dev, uint8_t reg_address) {
    uint8_t *data = malloc(sizeof(uint8_t));
    uint8_t *reg = malloc(sizeof(uint8_t));
    *reg = reg_address;
    i2c_write_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, reg, 1, true);
    free(reg);
    i2c_read_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, data, 1, false);
    uint8_t out = *data;
    free(data);
    return out;
}

// Maybe not needed?
uint8_t* MCP23017_ReadRegisters(MCP23017 *dev, uint8_t reg_address, uint8_t length) {
    uint8_t *data = malloc(sizeof(uint8_t) * length);
    uint8_t *reg = malloc(sizeof(uint8_t));
    *reg = reg_address;
    i2c_write_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, reg, 1, true);
    free(reg);
    i2c_read_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, data, length, false);
    return data;
}

// Writes all of <data> to the register specified by <reg_address>
void MCP23017_WriteRegister(MCP23017 *dev, uint8_t reg_address, uint8_t *data) {
    uint8_t *reg = malloc(sizeof(uint8_t));
    *reg = reg_address;
    i2c_write_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, reg, 1, true);
    free(reg);
    i2c_write_blocking(dev->i2c_instance, dev->mcp23017_i2c_addr, data, sizeof(*data), false);
}