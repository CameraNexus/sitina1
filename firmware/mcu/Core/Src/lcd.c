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
#include "stm32f1xx_hal.h"
#include <string.h>
#include "lcd.h"
#include "font.h"

#define LCD_BL_GPIO     GPIOA
#define LCD_BL_PIN      GPIO_PIN_15

#define LCD_CS_GPIO     GPIOB
#define LCD_CS_PIN      GPIO_PIN_12

#define LCD_BL_ON()     LCD_BL_GPIO->BSRR = LCD_BL_PIN
#define LCD_BL_OFF()    LCD_BL_GPIO->BRR = LCD_BL_PIN

#define LCD_CS_SEL()    LCD_CS_GPIO->BSRR = LCD_CS_PIN
#define LCD_CS_DESEL()  LCD_CS_GPIO->BRR = LCD_CS_PIN

#define SPI_SEND(buf, size) HAL_SPI_TransmitReceive(&hspi2, buf, dummy, size, HAL_MAX_DELAY)

extern TIM_HandleTypeDef htim1;
extern SPI_HandleTypeDef hspi2;

extern const unsigned char image_splash[2048];

uint8_t framebuffer[2048];
static uint8_t dummy[16]; // For SPI RX, unused
static int vcom;

void lcd_set_brightness(uint8_t val) {
    TIM1->CCR1 = 255 - val;
}

void lcd_mlcd_delay() {
    volatile int x = 100;
    while (x--);
}

static uint8_t revbits(uint8_t x) {
    uint32_t i = (uint32_t)x;
    uint32_t o;
    asm("rbit %0,%1" : "=r"(o) : "r"(i));
    return (o >> 24);
}

void lcd_set_pixel(int x, int y, int c) {
    uint8_t *ptr = &framebuffer[y * 16 + x / 8];
    uint8_t mask = 0x80 >> (x % 8);
    if (c)
        *ptr |= mask;
    else
        *ptr &= ~mask;
}

void lcd_disp_char(int x, int y, char c) {
    for (int yy = 0; yy < 12; yy++) {
        for (int xx = 0; xx < 8; xx++) {
            if ((charMap_ascii[c][yy] >> xx) & 0x01) {
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

void lcd_update() {
    uint8_t cmd[2] = {0x80, 0x00};

    // TODO
    vcom = !vcom;
    if (vcom)
        cmd[0] |= 0x40;
    LCD_CS_SEL();
    for (int i = 0; i < 128; i++) {
        cmd[1] = revbits(i + 1);
        SPI_SEND(cmd, 2);
        SPI_SEND(&framebuffer[i*16], 16);
    }
    dummy[0] = 0x00;
    dummy[1] = 0x00;
    SPI_SEND(dummy, 2);
    LCD_CS_DESEL();
}

void lcd_init(void) {
    // LCD BL and RST shares the same pin
    // Set the brightness to minimum first, do the reset sequence
    // Then set to desired brightness
    lcd_set_brightness(0);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    LCD_BL_ON();
    HAL_Delay(100);
    LCD_BL_OFF();
    HAL_Delay(100);
    LCD_BL_ON();
    lcd_set_brightness(200);

    //LCD_BL_OFF();

    vcom = 1;
    memset(framebuffer, 0x00, 2048);
    lcd_update();
}

