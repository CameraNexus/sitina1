//
// Project Fobu
// Copyright 2020 Wenting Zhang
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
// File : i2c.c
// Brief: I2C board level driver
//
#include "fsl_iomuxc.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "i2c.h"

/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY (CLOCK_GetFreq(kCLOCK_OscRc48MDiv2))

void i2c_init(void) {
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_12_LPI2C4_SCL,      /* GPIO_DISP_B2_12 is configured as LPI2C4_SCL */
        1U);                                    /* Software Input On Field: Force input path of pad GPIO_DISP_B2_12 */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_13_LPI2C4_SDA,      /* GPIO_DISP_B2_13 is configured as LPI2C4_SDA */
        1U);                                    /* Software Input On Field: Force input path of pad GPIO_DISP_B2_13 */
    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_DISP_B2_12_LPI2C4_SCL,      /* GPIO_DISP_B2_12 PAD functional properties : */
        0x10U);                                 /* Slew Rate Field: Slow Slew Rate
                                                   Drive Strength Field: normal drive strength
                                                   Pull / Keep Select Field: Pull Disable, Highz
                                                   Pull Up / Down Config. Field: Weak pull down
                                                   Open Drain Field: Enabled
                                                   Domain write protection: Both cores are allowed
                                                   Domain write protection lock: Neither of DWP bits is locked */
    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_DISP_B2_13_LPI2C4_SDA,      /* GPIO_DISP_B2_13 PAD functional properties : */
        0x10U);                                 /* Slew Rate Field: Slow Slew Rate
                                                   Drive Strength Field: normal drive strength
                                                   Pull / Keep Select Field: Pull Disable, Highz
                                                   Pull Up / Down Config. Field: Weak pull down
                                                   Open Drain Field: Enabled
                                                   Domain write protection: Both cores are allowed
                                                   Domain write protection lock: Neither of DWP bits is locked */

    lpi2c_master_config_t masterConfig = {0};

    /* Setup lpi2c master */
    /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * masterConfig.baudRate_Hz = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);

    LPI2C_MasterInit(LPI2C4, &masterConfig, LPI2C_CLOCK_FREQUENCY);
}

int i2c_write_reg(LPI2C_Type *i2c, uint8_t addr, uint8_t reg, uint8_t val) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &reg, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &val, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) goto error;

    return 0;

error:
    printf("I2C write failed\n");
    return -1;
}

int i2c_write_reg2(LPI2C_Type *i2c, uint8_t addr, uint8_t reg, uint8_t val1, uint8_t val2) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &reg, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &val1, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &val2, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) goto error;

    return 0;

error:
    printf("I2C write failed\n");
    return -1;
}

int i2c_write_payload(LPI2C_Type *i2c, uint8_t addr, uint8_t *payload, size_t len) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, payload, len)) goto error;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) goto error;

    return 0;

error:
    printf("I2C write failed\n");
    return -1;
}

int i2c_read_reg(LPI2C_Type *i2c, uint8_t addr, uint8_t reg, uint8_t *val) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, &reg, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterRepeatedStart(i2c, addr, kLPI2C_Read)) goto error;
    if (kStatus_Success != LPI2C_MasterReceive(i2c, val, 1)) goto error;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) goto error;

    return 0;

error:
    printf("I2C read failed\n");
    return -1;
}

int i2c_read_payload(LPI2C_Type *i2c, uint8_t addr, uint8_t *tx_payload, size_t tx_len, uint8_t *rx_payload, size_t rx_len) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) goto error;
    if (kStatus_Success != LPI2C_MasterSend(i2c, tx_payload, tx_len)) goto error;
    if (kStatus_Success != LPI2C_MasterRepeatedStart(i2c, addr, kLPI2C_Read)) goto error;
    if (kStatus_Success != LPI2C_MasterReceive(i2c, rx_payload, rx_len)) goto error;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) goto error;

    return 0;

error:
    printf("I2C read failed\n");
    return -1;
}

bool i2c_ping(LPI2C_Type *i2c, uint8_t addr) {
    if (kStatus_Success != LPI2C_MasterStart(i2c, addr, kLPI2C_Write)) return false;
    if (kStatus_Success != LPI2C_MasterStop(i2c)) return false;
    return true;
}
