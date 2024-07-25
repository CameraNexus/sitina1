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
// File : app_main.c
// Brief: Application main
//
#include "stm32g0xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "app_main.h"
#include "power.h"
#include "lcd.h"
#include "key.h"

static bool power_on = false;
static volatile bool lcd_vcom_update_req = false;

static volatile bool tick_60hz = false;
static volatile bool tick_120hz = false;
static volatile bool tick_1hz = false;

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    UNUSED(hrtc);
    lcd_vcom_update_req = true;
}

void app_systick(void) {
    static uint32_t tick = 0;
    tick ++;
    if (tick % 8 == 0) {
        tick_120hz = true; // actually 125Hz
    }
    if (tick % 16 == 0) {
        tick_60hz = true;
    }
    if (tick % 1000 == 0) {
        tick_1hz = true;
    }
}

void app_init(void) {
    // First booting up
    // Initialize always ON sections
    lcd_init();
    key_init();
    lcd_disp_string(0, 0, "Sitina 1");
    lcd_disp_string(0, 12, "EC VER 1.0");
    lcd_stby_update();
}

void app_tick(void) {
    if (power_on) {
        // Do things during power on

        // Blink LED
        if (tick_1hz) {
            tick_1hz = false;
            // Nothing?
        }

        if (tick_60hz) {
            tick_60hz = false;
            lcd_schedule_com_inversion();

            static uint16_t keys_last = 0xffff;
            uint16_t keys = key_get_buttons();
            if (keys_last != keys) {
                keys_last = keys;
                if (keys & KEY_FN1_MASK)
                    lcd_disp_string(0, 0, "FN1    ");
                else if (keys & KEY_FN2_MASK)
                    lcd_disp_string(0, 0, "FN2    ");
                else if (keys & KEY_FN3_MASK)
                    lcd_disp_string(0, 0, "FN3    ");
                else if (keys & KEY_FN4_MASK)
                    lcd_disp_string(0, 0, "FN4    ");
                else if (keys & KEY_SHUTTER_MASK)
                    lcd_disp_string(0, 0, "SHUTTER");
                else if (keys & KEY_UP_MASK)
                    lcd_disp_string(0, 0, "UP     ");
                else if (keys & KEY_LEFT_MASK)
                    lcd_disp_string(0, 0, "LEFT   ");
                else if (keys & KEY_RIGHT_MASK)
                    lcd_disp_string(0, 0, "RIGHT  ");
                else if (keys & KEY_DOWN_MASK)
                    lcd_disp_string(0, 0, "DOWN   ");
                else if (keys & KEY_FN5_MASK)
                    lcd_disp_string(0, 0, "FN5    ");
                else if (keys & KEY_FN6_MASK)
                    lcd_disp_string(0, 0, "FN6    ");
                else
                    lcd_disp_string(0, 0, "       ");
                lcd_schedule_update();
            }
            static int8_t rotenc_last = 0x7f;
            int8_t rotenc = key_get_rotenc();
            if (rotenc_last != rotenc) {
                rotenc_last = rotenc;
                uint8_t rotenc_abs;
                if (rotenc < 0) {
                    rotenc_abs = 0 - rotenc;
                    lcd_disp_char(0, 12, '-');
                }
                else {
                    rotenc_abs = rotenc;
                    lcd_disp_char(0, 12, ' ');
                }
                lcd_disp_char(8, 12, '0' + (rotenc_abs / 10));
                lcd_disp_char(16, 12, '0' + (rotenc_abs % 10));
                lcd_schedule_update();
            }
        }

        if (tick_120hz) {
            tick_120hz = false;
            lcd_update(); // This also scans the key
        }

        // Detect power down
        if (!power_is_power_on()) {
            // Reset all GPIOs

            // Disable WDT

            // Power down
            power_lcd_off();
            power_fpga_reset();
            power_power_off_ccd();
            power_power_off_lens();

            power_on = false;
        }
        // Feed WDT

    }
    else {
        // Currently not ON, check if it's ON due to being powered ON or RTC
        // tick for screen update
        if (lcd_vcom_update_req) {
            lcd_stby_com_inversion();
        }

        if (power_is_power_on()) {
            // POWER ON
            power_on = true;
            // Initialize PMIC
            power_pmic_init();
            // Clear LCD
            lcd_clear(0);
            // Wait
            HAL_Delay(100);
            // Power ON CCD (test for now)
            power_power_on_ccd();
            // Wait
            HAL_Delay(100);
            // Power ON LCD
            power_lcd_on();
            // Release FPGA
            power_release_fpga_reset();
        }
        else {
            // Go back to sleep
            // TODO
        }
    }
}
