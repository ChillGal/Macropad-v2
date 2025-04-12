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
#define SSD1306_I2C_ADDRESS    0x3C // Default address is 0x20. Range from 0x20-0x27, bit ordering is 0 0 1 0 0 A2 A1 A0

#define SSD1306_POWERON         0xAE
#define SSD1306_POWEROFF        0xAF

typedef struct {
    
    // I2C instance/Handle
    i2c_inst_t *i2c_instance;
    uint8_t ssd1306_i2c_addr;
    uint8_t height;
    uint8_t width;

} SSD1306;

uint8_t SSD1306_Initialise(SSD1306 *dev, i2c_inst_t *i2c_instance, uint8_t ssd1306_address, uint8_t ssd1306_height, uint8_t ssd1306_width);

void SSD1306_DisplayPowerOn(SSD1306 *dev);

uint8_t SSD1306_ReadRegister(SSD1306 *dev, uint8_t reg_address);
void SSD1306_WriteRegister(SSD1306 *dev, uint8_t reg_address, uint8_t *data);
#endif