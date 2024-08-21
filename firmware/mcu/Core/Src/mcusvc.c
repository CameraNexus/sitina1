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
#include "lcd.h"
#include "key.h"
#include "mcusvc.h"

static enum {
    STATE_IDLE,
    STATE_HDR_RX_WAIT,
    STATE_FB_RX_WAIT,
	STATE_TX_WAIT
} state;

static uint8_t rx_buffer[2048];
extern I2C_HandleTypeDef hi2c1;

#define MCUSVC_I2C			&hi2c1

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
	HAL_StatusTypeDef result;
    switch (state) {
    case STATE_IDLE:
        if (HAL_I2C_Slave_Receive_DMA(MCUSVC_I2C, rx_buffer, 5) == HAL_OK) {
            state = STATE_HDR_RX_WAIT;
        }
        break;
    case STATE_HDR_RX_WAIT:
        if (HAL_I2C_GetState(MCUSVC_I2C) == HAL_I2C_STATE_READY) {
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
            case CMD_FRAMEBUF_XFER:
            	result = HAL_I2C_Slave_Receive_DMA(MCUSVC_I2C, rx_buffer, 2048);
            	assert(result == HAL_OK);
            	state = STATE_FB_RX_WAIT;
            	break;
            case CMD_GET_BUTTONS: {
            	uint16_t btn = key_get_buttons();
            	rx_buffer[1] = btn & 0xff;
            	rx_buffer[2] = (btn >> 8) & 0xff;
            	rx_buffer[3] = 0x00;
            	rx_buffer[4] = 0x00;
            	result = HAL_I2C_Slave_Transmit_DMA(MCUSVC_I2C, rx_buffer, 5);
            	assert(result == HAL_OK);
            	state = STATE_TX_WAIT;
            	}
            	break;
            case CMD_GET_ROTENC: {
            	int8_t rotenc = key_get_rotenc();
            	key_set_rotenc(0);
            	*(int8_t *)&rx_buffer[1] = rotenc;
            	rx_buffer[2] = 0x00;
            	rx_buffer[3] = 0x00;
            	rx_buffer[4] = 0x00;
            	result = HAL_I2C_Slave_Transmit_DMA(MCUSVC_I2C, rx_buffer, 5);
				assert(result == HAL_OK);
				state = STATE_TX_WAIT;
            	}
            	break;
            }
        }
        break;
    case STATE_FB_RX_WAIT:
    	if (HAL_I2C_GetState(MCUSVC_I2C) == HAL_I2C_STATE_READY) {
    		uint16_t *wrptr = (uint16_t *)&framebuffer[2];
    		uint16_t *rdptr = (uint16_t *)rx_buffer;
    		for (int i = 0; i < 128; i++) {
    			for (int j = 0; j < 8; j++) {
    				*wrptr++ = *rdptr++;
    			}
    			wrptr++; // skip 2 bytes on WR
    		}
    		lcd_schedule_update();
    		state = STATE_IDLE;
    	}
    	break;
    case STATE_TX_WAIT:
    	if (HAL_I2C_GetState(MCUSVC_I2C) == HAL_I2C_STATE_READY) {
			state = STATE_IDLE;
		}
		break;
    }
}
