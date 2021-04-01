//
// afe.c: AD9990 AFE driver
//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
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
#include "afe.h"

void afe_init_io(void) {
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        0,
        kGPIO_NoIntmode,
    };

    // IOs default to 0
    GPIO_PinInit(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, &config);
    GPIO_PinInit(AFE_MOSI_GPIO, AFE_MOSI_GPIO_PIN, &config);

    config.outputLogic = 1;

    // IOs default to 1
    GPIO_PinInit(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, &config);
    GPIO_PinInit(AFE_CS_GPIO, AFE_CS_GPIO_PIN, &config);
}

// Clock high on idle, latch on rising edge, LSB first
void afe_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        GPIO_PinWrite(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, 0);
        GPIO_PinWrite(AFE_MOSI_GPIO, AFE_MOSI_GPIO_PIN, byte & 0x01);
        GPIO_PinWrite(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, 1);
        byte >>= 1;
    }
}

// 12 bit address, 28 bit data
void afe_write_reg(uint32_t reg, uint32_t val) {
    GPIO_PinWrite(AFE_CS_GPIO, AFE_CS_GPIO_PIN, 0);
    afe_write_byte(reg & 0xff);
    afe_write_byte(((reg >> 8) & 0xf) | ((val << 4) & 0xf));
    afe_write_byte((val >> 4) & 0xff);
    afe_write_byte((val >> 12) & 0xff);
    afe_write_byte((val >> 20) & 0xff);
    GPIO_PinWrite(AFE_CS_GPIO, AFE_CS_GPIO_PIN, 1);
}

void afe_init(void) {
    afe_init_io();

    afe_write_reg(0x10, 0x1); // Software reset

    // Define the standby status of the AD9990 vertical outputs
    afe_write_reg(0x25, 0x1ff8000); // Standby2
    afe_write_reg(0x26, 0x1ff8000); // Standby3

}
