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
// File : mcusvc.c
// Brief: Service provided by MCU
//
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "mcusvc.h"

#define CMD_NOP             0x00
#define CMD_SET_LCD_PWR     0x01
#define CMD_SET_LCD_BL      0x02
#define CMD_FRAMEBUF_XFER   0x03
#define CMD_SET_LED         0x04
#define CMD_GET_BUTTONS     0x80
#define CMD_GET_ROTENC      0x81

typedef struct {
    uint8_t cmd;
    uint8_t param1;
    uint8_t param2;
    uint8_t param3;
    uint8_t param4;
} DATA_PKT;

#define PKT_SIZE            sizeof(DATA_PKT)

#define MCU_I2C             &i2c0
#define MCU_I2C_ADDR        0x20

void mcusvc_init(void) {
    // nothing
}

void mcusvc_set_lcd_power(bool en) {
    DATA_PKT pkt = {
        .cmd = CMD_SET_LCD_PWR,
        .param1 = en ? 1 : 0
    };
    i2c_write_payload(MCU_I2C, MCU_I2C_ADDR, (uint8_t *)&pkt, PKT_SIZE);
}

void mcusvc_set_lcd_bl(uint8_t brightness) {
    DATA_PKT pkt = {
        .cmd = CMD_SET_LCD_BL,
        .param1 = brightness
    };
    i2c_write_payload(MCU_I2C, MCU_I2C_ADDR, (uint8_t *)&pkt, PKT_SIZE);
}

void mcusvc_set_led(bool on) {
    DATA_PKT pkt = {
        .cmd = CMD_SET_LED,
        .param1 = on
    };
    i2c_write_payload(MCU_I2C, MCU_I2C_ADDR, (uint8_t *)&pkt, PKT_SIZE);
}
