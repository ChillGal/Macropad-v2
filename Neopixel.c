/*
 *
 *  Neopixel SK6812-Mini Driver
 *  
 *  Author: Jennifer Chan
 *  Created: 23/03/2025
 *  Updated: 23/03/2025
 *  Revision: 0.1
 *  Datasheet: https://cdn-shop.adafruit.com/product-files/2686/SK6812MINI_REV.01-1-2.pdf
 * 
*/

// Create Neopixel instance
// Set colour
// Set brightness
// Clear 
// Show
// Fill
// Set length

// 24 bit data structure consisting of G7->0 R7->0 B7->0

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "Neopixel.h"
#include "Neopixel.pio"

typedef struct {
    uint8_t red;
    uint8_t blue;
    uint8_t green;
} Pixel;


uint8_t Neopixel_Initialise(PIO *pio) {
    uint sm = pio_claim_unused_sm(pio, true);
    //uint offset = pio_add_program(pio, &neopixel_program)
}