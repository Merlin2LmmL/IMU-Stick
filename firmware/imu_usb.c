#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "tusb.h"

/* =========================
   Pin definitions
   ========================= */
#define SPI_PORT spi0

#define PIN_SCK   2
#define PIN_MISO  3
#define PIN_MOSI  4
#define PIN_CS    5
#define PIN_INT1  6

/* =========================
   ICM-45686 registers
   ========================= */
#define REG_WHO_AM_I      0x75
#define REG_PWR_MGMT0    0x4E
#define REG_ACCEL_CONFIG 0x50
#define REG_GYRO_CONFIG  0x4F
#define REG_ACCEL_XOUT_H 0x1F

#define WHO_AM_I_EXPECTED 0xE9  // Verify with datasheet revision

/* =========================
   SPI helpers
   ========================= */
static inline void cs_select() {
    gpio_put(PIN_CS, 0);
}

static inline void cs_deselect() {
    gpio_put(PIN_CS, 1);
}

uint8_t icm_read(uint8_t reg) {
    uint8_t tx[2] = { reg | 0x80, 0x00 };
    uint8_t rx[2];

    cs_select();
    spi_write_read_blocking(SPI_PORT, tx, rx, 2);
    cs_deselect();

    return rx[1];
}

void icm_write(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = { reg & 0x7F, val };

    cs_select();
    spi_write_blocking(SPI_PORT, tx, 2);
    cs_deselect();
}

/* =========================
   IMU init
   ========================= */
void icm_init() {
    sleep_ms(50);

    uint8_t who = icm_read(REG_WHO_AM_I);
    if (who != WHO_AM_I_EXPECTED) {
        printf("ICM WHO_AM_I mismatch: 0x%02X\n", who);
    }

    /* Enable accel + gyro (low noise mode) */
    icm_write(REG_PWR_MGMT0, 0x0F);

    /* ±4g accel, 1 kHz ODR */
    icm_write(REG_ACCEL_CONFIG, 0x04);

    /* ±500 dps gyro, 1 kHz ODR */
    icm_write(REG_GYRO_CONFIG, 0x04);
}

/* =========================
   Read accel + gyro
   ========================= */
void icm_read_motion(int16_t *ax, int16_t *ay, int16_t *az,
                     int16_t *gx, int16_t *gy, int16_t *gz) {

    uint8_t tx[13];
    uint8_t rx[13];

    tx[0] = REG_ACCEL_XOUT_H | 0x80;
    for (int i = 1; i < 13; i++) tx[i] = 0;

    cs_select();
    spi_write_read_blocking(SPI_PORT, tx, rx, 13);
    cs_deselect();

    *ax = (rx[1] << 8) | rx[2];
    *ay = (rx[3] << 8) | rx[4];
    *az = (rx[5] << 8) | rx[6];
    *gx = (rx[7] << 8) | rx[8];
    *gy = (rx[9] << 8) | rx[10];
    *gz = (rx[11] << 8) | rx[12];
}

/* =========================
   Main
   ========================= */
int main() {
    stdio_init_all();
    tusb_init();

    /* SPI init */
    spi_init(SPI_PORT, 1 * 1000 * 1000);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    icm_init();

    int16_t ax, ay, az, gx, gy, gz;

    while (true) {
        tud_task();

        if (tud_cdc_connected()) {
            icm_read_motion(&ax, &ay, &az, &gx, &gy, &gz);

            printf("%d,%d,%d,%d,%d,%d\n",
                   ax, ay, az, gx, gy, gz);
        }

        sleep_ms(5); // ~200 Hz
    }
}
