//
// st7701s.c: Control interface driver for ST7701S
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
#include "fsl_gpio.h"
#include "fsl_pwm.h"
#include "lcd.h"

#include "nano_shell.h"
#include "command/command.h"
#include "shell_io/shell_io.h"

#define ST7701S_CMD(cmd, params...)                         \
    {                                                       \
        const uint8_t d[] = {params};                       \
        lcd_write_cmd(cmd, sizeof(d) / sizeof(uint8_t), d); \
    }

void lcd_init_io(void) {
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        0,
        kGPIO_NoIntmode,
    };

    // IOs default to 0
    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
    GPIO_PinInit(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, &config);
    GPIO_PinInit(LCD_MOSI_GPIO, LCD_MOSI_GPIO_PIN, &config);

    config.outputLogic = 1;

    // IOs default to 1
    GPIO_PinInit(LCD_CS_GPIO, LCD_CS_GPIO_PIN, &config);
    GPIO_PinInit(LCD_RST_GPIO, LCD_RST_GPIO_PIN, &config);
}

void lcd_init_pwm(void) {
    pwm_config_t pwm_config;
    PWM_GetDefaultConfig(&pwm_config);

    pwm_config.reloadLogic = kPWM_ReloadPwmFullCycle;

    if (PWM_Init(LCD_BL_PWM, LCD_BL_PWM_MOD, &pwm_config) == kStatus_Fail) {
        while(1);
        // Todo: proper fatal
    }

    uint32_t pwm_source_freq = CLOCK_GetFreq(kCLOCK_IpgClk);
    uint32_t pwm_target_freq = 1000ul;
    pwm_signal_param_t pwm_signal;

    pwm_signal.pwmChannel = LCD_BL_PWM_CH;
    pwm_signal.level = kPWM_HighTrue;
    pwm_signal.dutyCyclePercent = 50;
    pwm_signal.deadtimeValue = 0;
    pwm_signal.faultState = kPWM_PwmFaultState0;

    PWM_SetupPwm(LCD_BL_PWM, LCD_BL_PWM_MOD, &pwm_signal, 1, LCD_BL_PWM_MODE,
            pwm_target_freq, pwm_source_freq);

    PWM_SetPwmLdok(LCD_BL_PWM, LCD_BL_PWM_CTRL_MOD, true);
    PWM_StartTimer(LCD_BL_PWM, LCD_BL_PWM_CTRL_MOD);
}

#define LCD_SPI_9B

// This function is only used during initialization, doesn't need to be fast
#if defined(LCD_SPI_9B)
void lcd_write(uint8_t byte, int dc) {
    //SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(LCD_MOSI_GPIO, LCD_MOSI_GPIO_PIN, dc);
    GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 1);
    //SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    for (int i = 0; i < 8; i++) {
        GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 0);
        GPIO_PinWrite(LCD_MOSI_GPIO, LCD_MOSI_GPIO_PIN, byte & 0x80);
        //SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 1);
        //SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        byte <<= 1;
    }
    GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 0);
    //SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}
#elif defined(LCD_SPI_16B)
void lcd_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 0);
        GPIO_PinWrite(LCD_MOSI_GPIO, LCD_MOSI_GPIO_PIN, byte & 0x80);
        SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 1);
        SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
        byte <<= 1;
    }
    GPIO_PinWrite(LCD_SCK_GPIO, LCD_SCK_GPIO_PIN, 0);
}

void lcd_write(uint8_t byte, int dc) {
    GPIO_PinWrite(LCD_CS_GPIO, LCD_CS_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    lcd_write_byte(dc ? 0x40 : 0x00);
    lcd_write_byte(byte);
    GPIO_PinWrite(LCD_CS_GPIO, LCD_CS_GPIO_PIN, 1);
    SDK_DelayAtLeastUs(1, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}
#endif

void lcd_write_cmd(const uint8_t cmd, const int count, const uint8_t *params) {
#if (defined(LCD_SPI_9B) || defined(LCD_SPI_8B))
    GPIO_PinWrite(LCD_CS_GPIO, LCD_CS_GPIO_PIN, 0);
    lcd_write(cmd, 0);
    for (int i = 0; i < count; i++) {
        lcd_write(params[i], 1);
    }
    GPIO_PinWrite(LCD_CS_GPIO, LCD_CS_GPIO_PIN, 1);
#elif defined(LCD_SPI_16B)
    for (int i = 0; i < count; i++) {
        lcd_write(cmd, 0);
        lcd_write(i, 0);
        lcd_write(params[i], 1);
    }
#endif
}

// Probably shouldn't be called, as the 7701 shares the reset line with 9990
void lcd_reset(void) {
    GPIO_PinWrite(LCD_RST_GPIO, LCD_RST_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(LCD_RST_GPIO, LCD_RST_GPIO_PIN, 1);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}

void st7701s_init(void) {
    // Sleep out
    ST7701S_CMD(ST7701_SLPOUT);
    SDK_DelayAtLeastUs(5000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    ST7701S_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, 0x13);
    ST7701S_CMD(0xEF, 0x08);

    // Command2, BK0
    ST7701S_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK0SEL);
    ST7701S_CMD(ST7701_BK0_LNESET, 0x4F, 0x00); // 640 Line
    ST7701S_CMD(ST7701_BK0_PORCTRL, 0x10, 0x0C);  // vbp, vfp
    ST7701S_CMD(ST7701_BK0_INVSEL, 0x01, 0x14);
    ST7701S_CMD(ST7701_BK0_SDIR, 0x04);

    ST7701S_CMD(ST7701_BK0_RGBCTRL, 0x00, 0x08, 0x02); // DE mode, VS/HS low active, data inp on posedge, EN high active
    //ST7701S_CMD(ST7701_BK0_RGBCTRL, 0x80, 0x08, 0x02); // HV mode, VS/HS low active, data inp on posedge, EN high active

    ST7701S_CMD(0xCC, 0x10);

    ST7701S_CMD(ST7701_BK0_PVGAMCTRL, 0x0A, 0x18, 0x1E, 0x12, 0x16, 0x0C, 0x0E, 0x0D, 0x0C, 0x29, 0x06, 0x14, 0x13, 0x29, 0x33, 0x1C);
    ST7701S_CMD(ST7701_BK0_NVGAMCTRL, 0x0A, 0x19, 0x21, 0x0A, 0x0C, 0x00, 0x0C, 0x03, 0x03, 0x23, 0x01, 0x0E, 0x0C, 0x27, 0x2B, 0x1C);
    // Command2, BK1
    ST7701S_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BK1SEL);
    ST7701S_CMD(ST7701_BK1_VRHS, 0x5D);
    ST7701S_CMD(ST7701_BK1_VCOM, 0x48);
    ST7701S_CMD(ST7701_BK1_VGHSS, 0x84);
    ST7701S_CMD(ST7701_BK1_TESTCMD, 0x80);
    ST7701S_CMD(ST7701_BK1_VGLS, 0x4D);
    ST7701S_CMD(ST7701_BK1_PWCTLR1, 0x85);
    ST7701S_CMD(ST7701_BK1_PWCTLR2, 0x20);
    ST7701S_CMD(ST7701_BK1_SPD1, 0x78);
    ST7701S_CMD(ST7701_BK1_SPD2, 0x78);

    ST7701S_CMD(0xD0, 0x88);

    ST7701S_CMD(0xE0, 0x00, 0x00, 0x02);
    ST7701S_CMD(0xE1, 0x06, 0xA0, 0x08, 0xA0, 0x05, 0xA0, 0x07, 0xA0, 0x00, 0x44, 0x44);
    ST7701S_CMD(0xE2, 0x20, 0x20, 0x44, 0x44, 0x96, 0xA0, 0x00, 0x00, 0x96, 0xA0, 0x00, 0x00);
    ST7701S_CMD(0xE3, 0x00, 0x00, 0x22, 0x22);
    ST7701S_CMD(0xE4, 0x44, 0x44);
    ST7701S_CMD(0xE5, 0x0D, 0x91, 0x0A, 0xA0, 0x0F, 0x93, 0x0A, 0xA0, 0x09, 0x8D, 0x0A, 0xA0, 0x0B, 0x8F, 0x0A, 0xA0);
    ST7701S_CMD(0xE6, 0x00, 0x00, 0x22, 0x22);
    ST7701S_CMD(0xE7, 0x44, 0x44);
    ST7701S_CMD(0xE8, 0x0C, 0x90, 0x0A, 0xA0, 0x0E, 0x92, 0x0A, 0xA0, 0x08, 0x8C, 0x0A, 0xA0, 0x0A, 0x8E, 0x0A, 0xA0);
    ST7701S_CMD(0xE9, 0x36, 0x00);
    ST7701S_CMD(0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40);
    ST7701S_CMD(0xED, 0xFF, 0x45, 0x67, 0xFA, 0x01, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x10, 0xAF, 0x76, 0x54, 0xFF);
    ST7701S_CMD(0xEF, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F);

    ST7701S_CMD(ST7701_CND2BKxSEL, 0x77, 0x01, 0x00, 0x00, ST7701_CMD2BKxSEL_NONE);

    ST7701S_CMD(ST7701_MADCTL, 0x00);
    // set pixel format
    ST7701S_CMD(ST7701_COLMOD, 0x50); // 0x50: RGB565

    // display on
    ST7701S_CMD(ST7701_DISPON);

    //ST7701S_CMD(ST7701_ALLPON);
}

void lcd_init(void) {
    lcd_init_io();
    lcd_init_pwm();
    lcd_reset();
    st7701s_init();
}

void lcd_set_bl(uint8_t brightness) {
    PWM_UpdatePwmDutycycle(LCD_BL_PWM, LCD_BL_PWM_MOD, LCD_BL_PWM_CH,
            LCD_BL_PWM_MODE, brightness);
    PWM_SetPwmLdok(LCD_BL_PWM, LCD_BL_PWM_CTRL_MOD, true);

    GPIO_PinWrite(LCD_BL_GPIO, LCD_BL_GPIO_PIN, (brightness) ? 1 : 0);
}

int lcd_cmd(const shell_cmd_t *pcmd, int argc, char *const argv[]) {
    if (argc == 1) {
        shell_printf("lcd command expects an operation\r\n");
        return 1;
    }
    if (strcmp(argv[1], "reset") == 0) {
        lcd_reset();
        st7701s_init();
        return 0;
    }
    if (strcmp(argv[1], "brightness") == 0) {
        if (argc != 3) {
            shell_printf("lcd brightness expects a parameter\r\n");
            return 1;
        }
        int brightness = atoi(argv[2]);
        if ((brightness > 100) || (brightness < 0)) {
            shell_printf("invalid brightness value\r\n");
            return 1;
        }
        lcd_set_bl(brightness);
        return 0;
    }
    shell_printf("invalid operation\r\n");
    return 1;
}

NANO_SHELL_ADD_CMD(lcd, lcd_cmd, "control lcd", "set lcd parameters");
