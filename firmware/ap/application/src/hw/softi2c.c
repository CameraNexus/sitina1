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
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "mu_platform.h"
#include "softi2c.h"

static const uint32_t SDA_PIN[2] = {13, 15};
static const uint32_t SCL_PIN[2] = {12, 14};

#define SDA_L(x)    *GPIO0_OESR = (1 << SDA_PIN[x])
#define SDA_H(x)    *GPIO0_OECR = (1 << SDA_PIN[x])
#define SCL_L(x)    *GPIO0_OESR = (1 << SCL_PIN[x])
#define SCL_H(x)    *GPIO0_OECR = (1 << SCL_PIN[x])
#define READ_SDA(x) ((*GPIO0_IDR >> SDA_PIN[x]) & 0x1)

void si2c_init(void) {
    *GPIO0_BCR =
        (1 << SDA_PIN[0]) | (1 << SDA_PIN[1]) |
        (1 << SCL_PIN[0]) | (1 << SCL_PIN[1]);
}

static void si2c_start(SI2C_PORT port) {
    SDA_H(port);
	usleep(1);
	SCL_H(port);
	usleep(5);
 	SDA_L(port);//START:when CLK is high,DATA change form high to low 
	usleep(5);
	SCL_L(port);//钳住I2C总线，准备发送或接收数据 
	usleep(2);
}	  

static void si2c_stop(SI2C_PORT port) {
	SCL_L(port);
	SDA_L(port);//STOP:when CLK is high DATA change form low to high
 	usleep(4);
	SCL_H(port); 
	usleep(5);
	SDA_H(port);//发送I2C总线结束信号
	usleep(4);							   	
}

static int si2c_wait_ack(SI2C_PORT port) {
	uint timeout=0;
	SDA_H(port);
    usleep(1);	   
	SCL_H(port);
    usleep(1);	 
	while (READ_SDA(port)) {
		timeout++;
		if (timeout > 100) {
			si2c_stop(port);
			return 1; // fail
		}
        usleep(1);
	}
	SCL_L(port);
	return 0; // success
}

static void si2c_sack(SI2C_PORT port) {
	SCL_L(port);
	SDA_L(port);
	usleep(2);
	SCL_H(port);
	usleep(2);
	SCL_L(port);
}

static void si2c_nack(SI2C_PORT port) {
	SCL_L(port);
	SDA_H(port);
	usleep(2);
	SCL_H(port);
	usleep(2);
	SCL_L(port);
}

// Send byte
static int si2c_write_byte(SI2C_PORT port, uint8_t txd) {
    uint8_t t;

    SCL_L(port);
    for (t=0; t<8; t++) {
		if((txd&0x80)>>7)
			SDA_H(port);
		else
			SDA_L(port);
		txd<<=1;
		usleep(2);
		SCL_H(port);
		usleep(3);
		SCL_L(port);
		usleep(2);
    }	 

    t = si2c_wait_ack(port);

    usleep(5);

    return t;
}

static uint8_t si2c_read_byte(SI2C_PORT port) {
	unsigned char i,receive=0;
    for (i=0; i<8; i++)	{
        SCL_L(port); 
        usleep(2);
		SCL_H(port);
        receive<<=1;
        if (READ_SDA(port)) receive++;   
		usleep(2); 
    }					 
    return receive;
}

static void si2c_ack(SI2C_PORT port, bool ack) {
    if (!ack)
        si2c_nack(port);
    else
        si2c_sack(port);
}

int si2c_write_reg(SI2C_PORT i2c, uint8_t addr, uint8_t reg, uint8_t val) {
	int retval = -1;
    si2c_start(i2c);
    usleep(5);
	if (!(si2c_write_byte(i2c, (addr << 1) | 0x0))) {
		si2c_write_byte(i2c, reg);
		si2c_write_byte(i2c, val);
        retval = 0;
	}
	si2c_stop(i2c);
    return retval;
}

int si2c_read_reg(SI2C_PORT i2c, uint8_t addr, uint8_t reg, uint8_t *val) {
    int retval = -1;
	uint8_t rd = 0;

	si2c_start(i2c);
	if (!(si2c_write_byte(i2c, (addr << 1) | 0x0))) {
		si2c_write_byte(i2c, reg);
		si2c_start(i2c);
		si2c_write_byte(i2c, (addr << 1) | 0x1);
		rd = si2c_read_byte(i2c);
		si2c_ack(i2c, true);
	}
	si2c_stop(i2c);

    *val = rd;
    return retval;
}
