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

uint8_t SSD1306_Initialise(SSD1306 *dev, i2c_inst_t *i2c_instance, uint8_t ssd1306_address) {
    // Checks HW I2C is functional + Valid SSD1306 address range
    if (i2c_instance == NULL || ssd1306_address < 0b00111100 || ssd1306_address > 0b00111101) { // 0x20 to 0x27
        return 1;
    }

    // Setup struct
    dev->i2c_instance= i2c_instance;
    dev->ssd1306_i2c_addr = 0;
    return 0;
}