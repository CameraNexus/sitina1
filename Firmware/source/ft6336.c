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
#include <stdint.h>
#include "i2c.h"
#include "ft6336.h"

#define FT_I2C_HOST LPI2C4

static uint8_t ft6336_read_reg(uint8_t reg) {
    uint8_t rdval = 0x00;
    i2c_write_byte(FT_I2C_HOST, FT6X36_ADDR, reg);
    i2c_read_byte(FT_I2C_HOST, FT6X36_ADDR, &rdval);
    return rdval;
}

void ft6336_init(void) {
    uint8_t rdval = 0x00;

    rdval = ft6336_read_reg(FT6X36_REG_PANEL_ID);
    if (rdval != FT6X36_VENDID) {
        printf("Wrong vendor ID: %02x (expected: %02x)\n", rdval, FT6X36_VENDID);
        return;
    }
    rdval = ft6336_read_reg(FT6X36_REG_CHIPID);
    printf("Touch IC chip ID: %02x\n", rdval);

    i2c_write_reg(FT_I2C_HOST, FT6X36_ADDR, FT6X36_REG_DEVICE_MODE, 0x00);
    i2c_write_reg(FT_I2C_HOST, FT6X36_ADDR, FT6X36_REG_THRESHOLD, FT6X36_DEFAULT_THRESHOLD);
    i2c_write_reg(FT_I2C_HOST, FT6X36_ADDR, FT6X36_REG_TOUCHRATE_ACTIVE, 0x0E);
}

void ft6336_scan(uint16_t *x, uint16_t *y, uint8_t *touched) {
    uint8_t touch_point_count;
    touch_point_count = ft6336_read_reg(FT6X36_REG_NUM_TOUCHES);

    uint8_t rdval;
    uint16_t temp;
    if (touch_point_count != 0) {
        *touched = 1;
        rdval = ft6336_read_reg(FT6X36_REG_P1_XH);
        temp = ((uint16_t)rdval & 0x0f) << 8;
        rdval = ft6336_read_reg(FT6X36_REG_P1_XL);
        temp |= (uint16_t)rdval;
        *x = temp;
        rdval = ft6336_read_reg(FT6X36_REG_P1_YH);
        temp = ((uint16_t)rdval & 0x0f) << 8;
        rdval = ft6336_read_reg(FT6X36_REG_P1_YL);
        temp |= (uint16_t)rdval;
        *y = temp;
    }
    else {
        *touched = 0;
    }

    return;
}
