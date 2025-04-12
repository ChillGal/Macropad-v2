/*
 *
 *  SSD1306 I2C Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 12/04/2025
 *  Updated: 12/04/2025
 *  Revision: 0.0.1
 *  Datasheet: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
 * 
*/

#include <stdio.h>
#include "hardware/i2c.h"
#include "SSD1306.h"
#include "pico/stdlib.h"
//#include "pico/malloc.h"
#include <malloc.h>

uint8_t SSD1306_Initialise(SSD1306 *dev, i2c_inst_t *i2c_instance, uint8_t ssd1306_address, uint8_t ssd1306_height, uint8_t ssd1306_width) {
    // Checks HW I2C is functional + Valid SSD1306 address range
    if (i2c_instance == NULL || ssd1306_address < 0b00111100 || ssd1306_address > 0b00111101) { // 0x3c to 0x3d
        return 1;
    }

    // Setup struct
    dev->i2c_instance= i2c_instance;
    dev->ssd1306_i2c_addr = ssd1306_address;
    dev->height = ssd1306_height;
    dev->width = ssd1306_width;

    return 0;
}

void SSD1306_DisplayPowerOn(SSD1306 *dev) {
    uint8_t *data = malloc(sizeof(uint8_t));
    *data = SSD1306_POWERON;
    SSD1306_WriteRegister(dev, 0x00, data);
    free(data);
}


// Reads 1 byte into <data> from the register specified by <reg_address>
uint8_t SSD1306_ReadRegister(SSD1306 *dev, uint8_t reg_address) {
    uint8_t *data = malloc(sizeof(uint8_t));
    uint8_t *reg = malloc(sizeof(uint8_t));
    *reg = reg_address;
    i2c_write_blocking(dev->i2c_instance, dev->ssd1306_i2c_addr, reg, 1, true);
    free(reg);
    i2c_read_blocking(dev->i2c_instance, dev->ssd1306_i2c_addr, data, 1, false);
    uint8_t out = *data;
    free(data);
    return out;
}

// Writes all of <data> to the register specified by <reg_address>
void SSD1306_WriteRegister(SSD1306 *dev, uint8_t reg_address, uint8_t *data) {
    uint8_t *reg = malloc(sizeof(uint8_t));
    *reg = reg_address;
    i2c_write_blocking(dev->i2c_instance, dev->ssd1306_i2c_addr, reg, 1, true);
    free(reg);
    i2c_write_blocking(dev->i2c_instance, dev->ssd1306_i2c_addr, data, sizeof(*data), false);
}