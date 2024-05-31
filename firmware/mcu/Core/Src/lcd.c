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
#include "lcd.h"

#define LCD_BL_GPIO     GPIOA
#define LCD_BL_PIN      GPIO_PIN_15

#define LCD_BL_ON()     LCD_BL_GPIO->BSRR = LCD_BL_PIN
#define LCD_BL_OFF()    LCD_BL_GPIO->BRR = LCD_BL_PIN

extern TIM_HandleTypeDef htim1;

void lcd_set_brightness(uint8_t val) {
    TIM1->CCR1 = 255 - val;
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
}

