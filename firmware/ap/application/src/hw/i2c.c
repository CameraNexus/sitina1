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
// File : i2c.c
// Brief: I2C board level driver
//
#include "xparameters.h"
#include "xiicps.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <xstatus.h>
#include "i2c.h"

XIicPs i2c0;

static void i2c_init_instance(XIicPs *i2c, uint32_t baseaddr, uint32_t fsclk) {
    XIicPs_Config *config;
    int status;

    config = XIicPs_LookupConfig(baseaddr);
    assert(config);
    status = XIicPs_CfgInitialize(i2c, config, config->BaseAddress);
    assert(status == XST_SUCCESS);
    status = XIicPs_SelfTest(i2c);
    assert(status == XST_SUCCESS);
    XIicPs_SetSClk(i2c, fsclk);
}

void i2c_init(void) {
    i2c_init_instance(&i2c0, XPAR_XIICPS_0_BASEADDR, 100*1000);
}

int i2c_write_byte(I2C_TYPE *i2c, uint8_t addr, uint8_t val) {
    uint8_t buf[1] = {val};
    int result;
    result = XIicPs_MasterSendPolled(i2c, buf, 1, addr);
    return (result == XST_SUCCESS) ? 0 : -1;
}

int i2c_write_reg(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    int result;
    result = XIicPs_MasterSendPolled(i2c, buf, 2, addr);
    return (result == XST_SUCCESS) ? 0 : -1;
}

int i2c_write_reg2(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t val1, uint8_t val2) {
    uint8_t buf[3] = {reg, val1, val2};
    int result;
    result = XIicPs_MasterSendPolled(i2c, buf, 3, addr);
    return (result == XST_SUCCESS) ? 0 : -1;
}

int i2c_write_payload(I2C_TYPE *i2c, uint8_t addr, uint8_t *payload, size_t len) {
    int result;
    result = XIicPs_MasterSendPolled(i2c, payload, len, addr);
    return (result == XST_SUCCESS) ? 0 : -1;
}

int i2c_read_reg(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t *val) {
    // TODO: Unsupported for now
    return -1;
}

int i2c_read_payload(I2C_TYPE *i2c, uint8_t addr, uint8_t *tx_payload, size_t tx_len, uint8_t *rx_payload, size_t rx_len) {
    // TODO: Unsupported for now
    int result;
    result = XIicPs_MasterSendPolled(i2c, tx_payload, tx_len, addr);
    if (result != XST_SUCCESS)
        return -1;
    result = XIicPs_MasterRecvPolled(i2c, rx_payload, rx_len, addr);
    return (result == XST_SUCCESS) ? 0 : -1;
}

int i2c_read_byte(I2C_TYPE *i2c, uint8_t addr, uint8_t *val) {
    uint8_t buf[1];
    int result;
    result = XIicPs_MasterRecvPolled(i2c, buf, 1, addr);
    *val = buf[0];
    return (result == XST_SUCCESS) ? 0 : -1;
}

bool i2c_ping(I2C_TYPE *i2c, uint8_t addr) {
    // TODO: Unsupported for now
    return false;
}

void i2c_probe(I2C_TYPE *i2c) {
    // TODO: Unsupported for now
}
