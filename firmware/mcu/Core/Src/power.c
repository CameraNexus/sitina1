//
// Sitina 1
// Copyright 2024 Wenting Zhang
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
#include "stm32g0xx_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include "i2c.h"
#include "power.h"

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

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim3;

#define AXP_I2C_HOST    &hi2c2
#define AXP_I2C_ADDR    0x34

#define PWROK_GPIO      GPIOA
#define PWROK_PIN       GPIO_PIN_15

#define CCDLOADSW_GPIO  GPIOB
#define CCDLOADSW_PIN   GPIO_PIN_1

#define CCDDCDCEN_GPIO  GPIOB
#define CCDDCDCEN_PIN   GPIO_PIN_2

#define FPGARST_GPIO    GPIOB
#define FPGARST_PIN     GPIO_PIN_6

#define LENSPWREN_GPIO  GPIOA
#define LENSPWREN_PIN   GPIO_PIN_1

#define LCD_BL_GPIO     GPIOB
#define LCD_BL_PIN      GPIO_PIN_7

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

void axp_set_gpio(int gpio, int val) {
    uint8_t reg =
            (gpio == 0) ? 0x90 :
            (gpio == 1) ? 0x92 :
            (gpio == 2) ? 0x94 : 0x00;
    if (reg == 0x00) return;
    i2c_write_reg(AXP_I2C_HOST, AXP_I2C_ADDR, reg, val ? 0x01 : 0x00);
}

bool power_is_power_on(void) {
    return ((PWROK_GPIO->IDR & PWROK_PIN) != 0);
}

// PMIC
void power_pmic_init(void) {
    axp_set_volt(AXP_DCDC2, 1800); // FPGA AUX supply
    axp_set_volt(AXP_DCDC3, 3300); // Main 3.3V supply
    axp_set_volt(AXP_LDO2, 3300); // SD card supply
    axp_set_volt(AXP_LDO3, 3300);
    axp_set_volt(AXP_LDO4, 3300); // 3.3V analog supply

    axp_set_gpio(1, 1); // Turn on power LED

    axp_read_volt(AXP_DCDC2);
    axp_read_volt(AXP_DCDC3);
    axp_read_volt(AXP_LDO2);
    axp_read_volt(AXP_LDO3);
    axp_read_volt(AXP_LDO4);
}

void power_set_sd_iovcc(bool en1v8) {
    axp_set_volt(AXP_LDO3, en1v8 ? 1800 : 3300);
}

// Power switches
void power_power_on_ccd(void) {
    CCDLOADSW_GPIO->BSRR = CCDLOADSW_PIN;
    HAL_Delay(100);
    CCDDCDCEN_GPIO->BSRR = CCDDCDCEN_PIN;
}

void power_power_off_ccd(void) {
    CCDDCDCEN_GPIO->BRR = CCDDCDCEN_PIN;
    HAL_Delay(10);
    CCDLOADSW_GPIO->BRR = CCDLOADSW_PIN;
}

void power_power_on_lens(void) {
    LENSPWREN_GPIO->BSRR = LENSPWREN_PIN;
}

void power_power_off_lens(void) {
    LENSPWREN_GPIO->BRR = LENSPWREN_PIN;
}

void power_release_fpga_reset(void) {
    FPGARST_GPIO->BSRR = FPGARST_PIN;
}

void power_fpga_reset(void) {
    FPGARST_GPIO->BRR = FPGARST_PIN;
}

// LCD
static void power_lcd_bl_on(void) {
    LCD_BL_GPIO->BSRR = LCD_BL_PIN;
}

static void power_lcd_bl_off(void) {
    LCD_BL_GPIO->BRR = LCD_BL_PIN;
}

void power_lcd_on(void) {
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    power_lcd_bl_on();
}

void power_lcd_set_brightness(uint8_t val) {
    TIM3->CCR1 = 255 - val;
}

void power_lcd_off(void) {
    power_lcd_bl_off();
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}
