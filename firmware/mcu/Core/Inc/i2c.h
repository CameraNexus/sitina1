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
// File : i2c.h
// Brief: I2C board level driver
//
#pragma once

#define I2C_TYPE I2C_HandleTypeDef

void i2c_init(void);
int i2c_write_byte(I2C_TYPE *i2c, uint8_t addr, uint8_t val);
int i2c_write_reg(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t val);
int i2c_write_reg2(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t val1, uint8_t val2);
int i2c_write_payload(I2C_TYPE *i2c, uint8_t addr, uint8_t *payload, size_t len);
int i2c_read_reg(I2C_TYPE *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int i2c_read_payload(I2C_TYPE *i2c, uint8_t addr, uint8_t *tx_payload, size_t tx_len, uint8_t *rx_payload, size_t rx_len);
int i2c_read_byte(I2C_TYPE *i2c, uint8_t addr, uint8_t *val);
bool i2c_ping(I2C_TYPE *i2c, uint8_t addr);
void i2c_probe(I2C_TYPE *i2c);
