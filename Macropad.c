#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "mcp23017.h"
#include <malloc.h>
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 6
#define I2C_SCL 7



int main()
{
    stdio_init_all();

    // // SPI initialisation. This example will use SPI at 1MHz.
    // spi_init(SPI_PORT, 1000*1000);
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    // gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    // gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // // Chip select is active-low, so we'll initialise it to a driven-high state
    // gpio_set_dir(PIN_CS, GPIO_OUT);
    // gpio_put(PIN_CS, 1);
    // // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // // I2C Initialisation. Using it at 400Khz.
    // i2c_init(I2C_PORT, 400*1000);
    
    // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    // // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // while (true) {
    //     printf("Hello, world!\n");
    //     sleep_ms(1000);
    // }
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    MCP23017 mcp;
    uint8_t *ptr = 0;
    uint8_t result = MCP23017_Initialise(&mcp, I2C_PORT, 0x20);
    uint8_t *dirptr = malloc(sizeof(uint8_t) * 2);
    dirptr[0] = 255;
    dirptr[1] = 255;
    MCP23017_SetIODirection(&mcp,dirptr);

    uint8_t *pullupptr = malloc(sizeof(uint8_t) * 2);
    pullupptr[0] = 255;
    pullupptr[1] = 255;
    MCP23017_SetPullups(&mcp,pullupptr);
    ptr = MCP23017_GetIO(&mcp);
    printf("%d", result);
    int sleep_time = 200; // 10ms default
    if (result == 1) {
        sleep_time = 1000;
    }
    const uint LED_PIN = 25; // LED pin is fixed at 25
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    if (&ptr[0] == 0){
        gpio_put(LED_PIN, 1);
    }
    else {
        gpio_put(LED_PIN, 0);
    }

    // while (true) {
    //     gpio_put(LED_PIN, 0);
    //     sleep_ms(sleep_time);
    //     gpio_put(LED_PIN, 1);
    //     sleep_ms(sleep_time);
    // }
}
