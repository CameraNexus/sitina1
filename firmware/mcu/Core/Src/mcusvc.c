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
#include "stm32g0xx_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include "i2c.h"
#include "power.h"
#include "mcusvc.h"

static enum {
    STATE_IDLE,
    STATE_HDR_RX_WAIT,
    STATE_FB_RX_WAIT
} state;

static uint8_t rx_buffer[64];
extern I2C_HandleTypeDef hi2c2;

#define CMD_NOP             0x00
#define CMD_SET_LCD_PWR     0x01
#define CMD_SET_LCD_BL      0x02
#define CMD_FRAMEBUF_XFER   0x03
#define CMD_SET_LED         0x04
#define CMD_GET_BUTTONS     0x80
#define CMD_GET_ROTENC      0x81

void mcusvc_init(void) {
    state = STATE_IDLE;
}

void mcusvc_tick(void) {
    switch (state) {
    case STATE_IDLE:
        if (HAL_I2C_Slave_Receive_IT(&hi2c2, rx_buffer, 5) == HAL_OK) {
            state = STATE_HDR_RX_WAIT;
        }
        break;
    case STATE_HDR_RX_WAIT:
        if (HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_READY) {
            // Buffer ready
            // By default enter IDLE state
            state = STATE_IDLE;
            switch (rx_buffer[0]) {
            case CMD_SET_LED:
                if (rx_buffer[1])
                    GPIOB->BSRR = GPIO_PIN_8;
                else
                    GPIOB->BRR = GPIO_PIN_8;
                break;
            case CMD_SET_LCD_PWR:
                if (rx_buffer[1])
                    power_lcd_on();
                else
                    power_lcd_off();
                break;
            case CMD_SET_LCD_BL:
                power_lcd_set_brightness(rx_buffer[1]);
                break;
            }
        }
        break;
    }
}
