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
// This example will use I2C1 on GPIO6 (SDA) and GPIO7 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 6
#define I2C_SCL 7
#define LED_PIN 25 // LED pin is fixed at 25

void setup_i2c(i2c_inst_t i2cBus, uint8_t i2cSDA, uint8_t i2cSCL) {
    i2c_init(&i2cBus, 400*1000);
    gpio_set_function(i2cSDA, GPIO_FUNC_I2C);
    gpio_set_function(i2cSCL, GPIO_FUNC_I2C);
    gpio_pull_up(i2cSDA);
    gpio_pull_up(i2cSCL);
}

void i2c_scan(i2c_inst_t i2cBus) {
    printf("Scanning i2c\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        int ret;
        uint8_t rxdata;
        if ((addr & 0x78) == 0 || (addr & 0x78) == 0x78)
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(&i2cBus, addr, &rxdata, 1, false);
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}

int main() {
    stdio_init_all();
    printf("\n \n \n");
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

    printf("v0.0.0.1\n");
    printf("Initialising I2C\n");
    setup_i2c(*I2C_PORT, I2C_SDA, I2C_SCL);
    i2c_scan(*I2C_PORT);
    printf("Initialising MCP23017\n");
    uint8_t* send = malloc(sizeof(int));
    MCP23017 mcp;
    uint8_t result = MCP23017_Initialise(&mcp, I2C_PORT, 0x20);
    
    printf("Setting Direction\n");
    uint16_t *dirptr = malloc(sizeof(uint16_t));
    *dirptr = (255 << 8) + 255;
    *send = 0x00;
    // MCP23017_SetIODirection(&mcp, dirptr);
    MCP23017_SetIODirection(&mcp, dirptr);
    uint16_t da = MCP23017_GetIODirection(&mcp);
    printf("Direction Function Config: %d\n", da);
    printf("Direction Done\n\n");

    printf("Setting Pullups\n");
    uint16_t *pullupptr = malloc(sizeof(uint16_t));
    *pullupptr = 0;
    MCP23017_SetPullups(&mcp, pullupptr);
    uint16_t pu = MCP23017_GetPullups(&mcp);
    printf("Pullup config: %d\n", pu);
    printf("Pullups Done\n\n");

    printf("Setting IO\n");
    uint16_t* buffer = malloc(sizeof(uint16_t));
    *send = 0x12;
    *buffer = 0;
    printf("IO Done\n\n");

    int sleep_time = 10; // 200ms default

    // 22   = 0001 0110
    // 150  = 1001 0110
    // 152  = 1001 1000
    // 16   = 0001 0000
    // 24   = 0001 1000
    // 180  = 1011 0100
    // 75   = 0100 1011
    // 40   = 0010 1000
    // 215  = 1101 0111
    // 182  = 1011 0110
    // 73   = 0100 1001
    // 54   = 0011 0110

    printf("Loadup done\n");

    while (true) {
        *buffer = MCP23017_GetIO(&mcp);
        printf("GPIO: %d\n", *buffer);
        sleep_ms(sleep_time);
    }
}
