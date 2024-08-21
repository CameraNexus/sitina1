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
// File : lcd.c
// Brief: LCD and related drivers
//
#include "stm32g0xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include "lcd.h"
#include "font.h"
#include "key.h"

#define LCD_CS_GPIO     GPIOA
#define LCD_CS_PIN      GPIO_PIN_4

#define LCD_CS_SEL()    LCD_CS_GPIO->BSRR = LCD_CS_PIN
#define LCD_CS_DESEL()  LCD_CS_GPIO->BRR = LCD_CS_PIN

#define SPI_SEND(buf, size) \
    HAL_SPI_Transmit(&hspi1, buf, size, HAL_MAX_DELAY)
#define SPI_SEND_DMA(buf,size) \
    HAL_SPI_Transmit_DMA(&hspi1, buf, size)
#define SPI_SEND_RECV(txbuf, rxbuf, size) \
    HAL_SPI_TransmitReceive(&hspi1, txbuf, rxbuf, size, HAL_MAX_DELAY)

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

uint8_t framebuffer[18*128+2];
static uint8_t rx[2];
static int vcom;

static bool lcd_chunk2_pending;
static bool lcd_chunk3_pending;
static bool lcd_update_pending;

void lcd_mlcd_delay() {
    volatile int x = 100;
    while (x--);
}

static uint8_t revbits(uint8_t x) {
#if 0
    uint32_t i = (uint32_t)x;
    uint32_t o;
    asm("rbit %0,%1" : "=r"(o) : "r"(i));
    return (o >> 24);
#else
    uint8_t o = 0;
    for (int i = 0; i < 8; i++) {
        o <<= 1;
        if (x & 0x01) {
            o |= 0x01;
        }
        x >>= 1;
    }
    return o;
#endif
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    // De-select LCD
    LCD_CS_DESEL();
}

void lcd_set_pixel(int x, int y, int c) {
    // Screen needs to be rotated
    y = 127 - y;
    x = 127 - x;
    uint8_t *ptr = &framebuffer[y * 18 + x / 8 + 2];
    uint8_t mask = 0x80 >> (x % 8);
    if (c)
        *ptr |= mask;
    else
        *ptr &= ~mask;
}

void lcd_disp_char(int x, int y, char c) {
    for (int yy = 0; yy < 12; yy++) {
        for (int xx = 0; xx < 8; xx++) {
            if ((charMap_ascii[(uint32_t)c][yy] >> xx) & 0x01) {
                lcd_set_pixel(x + xx, y + yy, 1);
            }
            else {
                lcd_set_pixel(x + xx, y + yy, 0);
            }
        }
    }
}

void lcd_disp_string(int x, int y, char *str) {
    while (*str) {
        lcd_disp_char(x, y, *str);
        x += 8;
        str++;
    }
}

void lcd_fill_rect(int x0, int y0, int x1, int y1, int c) {
    // very inefficient, though usually MCU doesn't need to draw on screen
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            lcd_set_pixel(x, y, c);
        }
    }
}

void lcd_clear(uint8_t color) {
    uint8_t cl = color ? 0xff : 0x00;
    for (int i = 0; i < 128; i++) {
        memset(&framebuffer[i*18+2], cl, 16);
    }
}

void lcd_update(void) {
    uint8_t *txbuf;
    uint32_t txsize;

    if (lcd_chunk2_pending) {
        txbuf = &framebuffer[43*18];
        txsize = 43*18+2-2;
        lcd_chunk2_pending = false;
        lcd_chunk3_pending = true;
    }
    else if (lcd_chunk3_pending) {
        txbuf = &framebuffer[86*18];
        txsize = 42*18+2-2;
        lcd_chunk3_pending = false;
    }
    else if (lcd_update_pending) {
        txbuf = &framebuffer[0];
        txsize = 43*18+2-2;
        lcd_update_pending = false;
        lcd_chunk2_pending = true;
        GPIOB->BSRR = GPIO_PIN_8;
    }
    else {
        // No screen update, set VCOM value and readback key scan result
        txbuf = &framebuffer[0];
        txsize = 0;
    }

    if (txsize == 0) {
        // No data payload, just 2 byte header for VCOM update and keyscan
        *txbuf = (vcom) ? 0x40 : 0x00;
        LCD_CS_SEL();
        SPI_SEND_RECV(txbuf, rx, 2);
        LCD_CS_DESEL();
    }
    else {
        // With data payload
        *txbuf = (vcom) ? 0xC0 : 0x80;
        LCD_CS_SEL();
        SPI_SEND_RECV(txbuf, rx, 2);
        SPI_SEND_DMA(txbuf + 2, txsize);
        // Deselect in finish callback
    }
    uint16_t rxval = (uint16_t)rx[0] << 8 | rx[1];
    key_scan(rxval);
}

void lcd_schedule_update(void) {
    lcd_update_pending = true;
}

void lcd_schedule_com_inversion(void) {
    vcom = !vcom;
}

void lcd_stby_com_inversion(void) {
    uint8_t cmd[2] = {0x00, 0x00};
    vcom = !vcom;
    if (vcom)
        cmd[0] |= 0x40;
    LCD_CS_SEL();
    SPI_SEND(cmd, 2);
    LCD_CS_DESEL();
}

void lcd_stby_update(void) {
    framebuffer[0] = (vcom) ? 0xC0 : 0x80;
    LCD_CS_SEL();
    SPI_SEND(framebuffer, 18*128+2);
    LCD_CS_DESEL();
}

void lcd_init(void) {
    vcom = 1;
    memset(framebuffer, 0x00, 18*128+2);
    for (int i = 0; i < 128; i++) {
        framebuffer[i * 18 + 1] = revbits(i + 1);
    }
//    HAL_DMA_RegisterCallback(&hdma_spi1_tx, HAL_DMA_XFER_CPLT_CB_ID,
//            lcd_dma_callback);
}

