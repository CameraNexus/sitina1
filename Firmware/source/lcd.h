//
// st7701s.h: Control interface driver for ST7701S
//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
//
// The driver is partially adopted from
// https://github.com/blazer82/FT81x_Arduino_Driver
// Copyright 2020 Raphael Stäbler
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
#pragma once

#define ST7701_SWRESET   0x01  ///< Software reset
#define ST7701_RDDPM     0x0A  ///< Read Display Power Mode
#define ST7701_RDDCOLMOD 0x0C  ///< Read Display Pixel Format
#define ST7701_RDDIM     0x0D  ///< Read Display Image Mode
#define ST7701_RDDSM     0x0E  ///< Read Display Signal Mode
#define ST7701_RDDSDR    0x0F  ///< Read Display Self-diagnostic result
#define ST7701_SLPIN     0x10  ///< Sleep in
#define ST7701_SLPOUT    0x11  ///< Sleep out
#define ST7701_PTLON     0x12  ///< Partial mode on
#define ST7701_NORON     0x13  ///< Normal display mode on
#define ST7701_INVOFF    0x20  ///< Display inversion off (normal)
#define ST7701_INVON     0x21  ///< Display inversion on
#define ST7701_DISPOFF   0x28  ///< Display off
#define ST7701_DISPON    0x29  ///< Display on
#define ST7701_TEON      0x35  ///< Tearing effect line on
#define ST7701_MADCTL    0x36  ///< Display data access control
#define ST7701_COLMOD    0x3A  ///< Interface Pixel Format
#define ST7701_WRDISBV   0x51  ///< Write display brightness
#define ST7701_SDIR      0xC7  ///< Source direction control
#define ST7701_RDID1     0xDA  ///< Read ID1
#define ST7701_RDID2     0xDB  ///< Read ID2
#define ST7701_RDID3     0xDC  ///< Read ID3

#define ST7701_CND2BKxSEL 0xFF  ///< Command2 BKx Selection

#define ST7701_BK0_PVGAMCTRL 0xB0  ///< Positive Voltage Gamma Control
#define ST7701_BK0_NVGAMCTRL 0xB1  ///< Negative Voltage Gamma Control
#define ST7701_BK0_LNESET    0xC0  ///< Display Line setting
#define ST7701_BK0_PORCTRL   0xC1  ///< Porch control
#define ST7701_BK0_INVSEL    0xC2  ///< Inversion selection & Frame Rate Control
#define ST7701_BK0_RGBCTRL   0xC3  ///< RGB control
#define ST7701_BK0_PARCTRL   0xC5  ///< Partial mode Control
#define ST7701_BK0_SDIR      0xC7  ///< Source direction control

#define ST7701_BK1_VRHS    0xB0  ///< Vop amplitude setting
#define ST7701_BK1_VCOM    0xB1  ///< VCOM amplitude setting
#define ST7701_BK1_VGHSS   0xB2  ///< VGH Voltage setting
#define ST7701_BK1_TESTCMD 0xB3  ///< TEST Command Setting
#define ST7701_BK1_VGLS    0xB5  ///< VGL Voltage setting
#define ST7701_BK1_PWCTLR1 0xB7  ///< Power Control 1
#define ST7701_BK1_PWCTLR2 0xB8  ///< Power Control 2
#define ST7701_BK1_SPD1    0xC1  ///< Source pre_drive timing set1
#define ST7701_BK1_SPD2    0xC2  ///< Source EQ2 Setting

#define ST7701_CMD2BKxSEL_NONE 0x00  ///< Command2 BKx Selection None
#define ST7701_CMD2BK0SEL      0x10  ///< Command2 BK0 Selection
#define ST7701_CMD2BK1SEL      0x11  ///< Command2 BK1 Selection

// LCD_BL_EN, C7
#define LCD_BL_GPIO         (GPIO3)
#define LCD_BL_GPIO_PIN     (27)
// LCD_CS, B14
#define LCD_CS_GPIO         (GPIO2)
#define LCD_CS_GPIO_PIN     (31)
// LCD_SCK, M14
#define LCD_SCK_GPIO        (GPIO1)
#define LCD_SCK_GPIO_PIN    (0)
// LCD_MOSI, H10
#define LCD_MOSI_GPIO       (GPIO1)
#define LCD_MOSI_GPIO_PIN   (1)
// LCD_RST, G11
#define LCD_RST_GPIO        (GPIO1)
#define LCD_RST_GPIO_PIN    (3)

#define LCD_BL_PWM          (PWM1)
#define LCD_BL_PWM_CH       (kPWM_PwmB)
#define LCD_BL_PWM_MOD      (kPWM_Module_3)
#define LCD_BL_PWM_CTRL_MOD (kPWM_Control_Module_3)
#define LCD_BL_PWM_MODE     (kPWM_CenterAligned)

void lcd_init_io(void);
void lcd_reset(void);
void lcd_init(void);
void lcd_set_bl(uint8_t brightness);
