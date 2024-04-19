//
// SitinaNe
// Copyright 2023 Wenting Zhang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// File : power.h
// Brief: Board power control
//
#include "fsl_clock.h"
#include "fsl_semc.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "power.h"
#include "i2c.h"

#define BOARD_POWER_LED_GPIO    GPIO11
#define BOARD_POWER_LED_PIN     15

typedef enum {
    AXP_DCDC2 = 0,
    AXP_DCDC3 = 1,
    AXP_LDO2 = 2,
    AXP_LDO3 = 3,
    AXP_LDO4 = 4
} AXP_RAIL;

static const uint8_t axp_rail_reg[] = {
    0x23, // dcdc2
    0x27, // dcdc3
    0x28, // ldo2 / 4
    0x29, // ldo3
    0x28  // ldo2 / 4
};

static const char *axp_rail_name[] = {
    "DCDC2",
    "DCDC3",
    "LDO2",
    "LDO3",
    "LDO4"
};

static const int axp_ldo4volts[] = {1250, 1300, 1400, 1500, 1600, 1700,
        1800, 1900, 2000, 2500, 2700, 2800, 3000, 3100, 3200, 3300};

#define AXP_I2C_HOST    LPI2C4
#define AXP_I2C_ADDR    0x34

void axp_read_volt(AXP_RAIL rail) {
    uint8_t val;
    i2c_read_reg(AXP_I2C_HOST, AXP_I2C_ADDR, axp_rail_reg[rail], &val);
    int volt;
    if (rail == AXP_LDO2) {
        volt = 1800 + 100 * ((val >> 4) & 0x0f);
    }
    else if (rail == AXP_LDO4) {
        volt = axp_ldo4volts[(val & 0x0f)];
    }
    else {
        volt = 700 + val * 25;
    }
    printf("%s: %d mV\r\n", axp_rail_name[rail], volt);
}

void axp_set_volt(AXP_RAIL rail, uint32_t microvolt) {
    uint8_t val;

    if (rail == AXP_LDO2) {
        i2c_read_reg(AXP_I2C_HOST, AXP_I2C_ADDR, axp_rail_reg[rail], &val);
        val &= ~0xf0;
        val |= ((microvolt - 1800) / 100) << 4;
    }
    else if (rail == AXP_LDO4) {
        i2c_read_reg(AXP_I2C_HOST, AXP_I2C_ADDR, axp_rail_reg[rail], &val);
        val &= ~0x0f;
        for (int i = 0; i < (sizeof(axp_ldo4volts) / sizeof(axp_ldo4volts[0])); i++) {
            if (microvolt == axp_ldo4volts[i]) {
                val |= (i & 0xf);
                break;
            }
        }
    }
    else {
        val = (microvolt - 700) / 25;
    }
    i2c_write_reg(AXP_I2C_HOST, AXP_I2C_ADDR, axp_rail_reg[rail], val);
}


void power_init(void) {
    axp_set_volt(AXP_LDO2, 1800);
    axp_set_volt(AXP_DCDC2, 2000);
    axp_set_volt(AXP_LDO3, 3300);

    const gpio_pin_config_t pinConfig = {kGPIO_DigitalOutput, 1, kGPIO_NoIntmode};
    GPIO_PinInit(BOARD_POWER_LED_GPIO, BOARD_POWER_LED_PIN, &pinConfig);

    axp_read_volt(AXP_DCDC2);
    axp_read_volt(AXP_DCDC3);
    axp_read_volt(AXP_LDO2);
    axp_read_volt(AXP_LDO3);
    axp_read_volt(AXP_LDO4);

}

void power_set_sd_iovcc(bool en1v8) {
    axp_set_volt(AXP_LDO3, en1v8 ? 1800 : 3300);
}
