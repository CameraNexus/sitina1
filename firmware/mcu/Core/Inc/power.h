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
// File : power.h
// Brief: Board power control
//
#pragma once

bool power_is_power_on(void);
void power_pmic_init(void);
void power_set_sd_iovcc(bool en1v8);
void power_power_on_ccd(void);
void power_power_off_ccd(void);
void power_power_on_lens(void);
void power_power_off_lens(void);
void power_power_off_pmic(void);
uint8_t power_get_pek(void);
void power_release_fpga_reset(void);
void power_fpga_reset(void);
void power_lcd_on(void);
void power_lcd_set_brightness(uint8_t val);
void power_lcd_off(void);
void power_off(void);
