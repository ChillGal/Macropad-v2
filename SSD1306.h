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

#ifndef _SSD1306_H
#define _SSD1306_H

// I2C address
#define SSD1306_I2C_ADDRESS    0x3c // Default address is 0x20. Range from 0x20-0x27, bit ordering is 0 0 1 0 0 A2 A1 A0

typedef struct {
    
    // I2C instance/Handle
    i2c_inst_t *i2c_instance;
    uint8_t ssd1306_i2c_addr;

} SSD1306;

uint8_t SSD1306_Initialise(SSD1306 *dev, i2c_inst_t *i2c_instance, uint8_t ssd1306_address);


#endif