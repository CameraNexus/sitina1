//
// Sitina1
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
#include <stdint.h>
#include <stdbool.h>
#include "mcusvc.h"
#include "pal_power.h"
#include "pwm.h"

static uint16_t batchrg;
static bool ischarging;

void pal_pwr_init(void) {
    pwm_init();
}

void pal_pwr_refresh(void) {
    uint32_t batchrg = mcusvc_get_bat_charge();
    batchrg = batchrg / (65535 / 100);
    ischarging = mcusvc_get_is_bat_charging();
}

uint8_t pal_pwr_get_battery_percent(void) {
    return batchrg;
}

bool pal_pwr_is_battery_charging(void) {
    return ischarging;
}

void pal_pwr_set_vab(uint8_t setpoint) {
    // 5-16V
    pwm_set(setpoint);
}
