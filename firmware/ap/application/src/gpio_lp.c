// Copyright (c) 2023 Embedded_Projects
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
#include <unistd.h>
#include <stdlib.h>
#include "xil_printf.h"
#include "xil_cache.h"

#include "xparameters.h"
#include "mu_platform.h"

#define GPIO_ENABLE_MASK    GPIO_PIN_5
#define GPIO_LP_L0P_MASK    GPIO_PIN_8
#define GPIO_LP_L1P_MASK    GPIO_PIN_9
#define GPIO_LP_L0N_MASK    GPIO_PIN_10
#define GPIO_LP_L1N_MASK    GPIO_PIN_11
#define GPIO_LP_CKP_MASK    GPIO_PIN_6
#define GPIO_LP_CKN_MASK    GPIO_PIN_7

uint32_t gpio_odr_val;

#define GPIO_WRITE(val)     *GPIO0_ODR = val
#define GPIO_READ()         *GPIO0_IDR
// CK+- 11 D1+- 11 D0+- 11
#define GPIO_LP_11() GPIO_WRITE(gpio_odr_val | GPIO_ENABLE_MASK | \
			   	   	   	   	   	GPIO_LP_CKP_MASK | GPIO_LP_CKN_MASK | \
								GPIO_LP_L0P_MASK | GPIO_LP_L0N_MASK | \
								GPIO_LP_L1P_MASK | GPIO_LP_L1N_MASK)

// CK+- 11 D1+- 11 D0+- 10
#define GPIO_LP_10() GPIO_WRITE(gpio_odr_val | GPIO_ENABLE_MASK | \
			   	   	   	   	   	GPIO_LP_CKP_MASK | GPIO_LP_CKN_MASK | \
								GPIO_LP_L0P_MASK | \
								GPIO_LP_L1P_MASK | GPIO_LP_L1N_MASK)

// CK+- 11 D1+- 11 D0+- 01
#define GPIO_LP_01() GPIO_WRITE(gpio_odr_val | GPIO_ENABLE_MASK | \
			   	   	   	   	   	GPIO_LP_CKP_MASK | GPIO_LP_CKN_MASK | \
								GPIO_LP_L0N_MASK | \
								GPIO_LP_L1P_MASK | GPIO_LP_L1N_MASK)

// CK+- 11 D1+- 11 D0+- 00
#define GPIO_LP_00() GPIO_WRITE(gpio_odr_val | GPIO_ENABLE_MASK | \
			   	   	   	   	   	GPIO_LP_CKP_MASK | GPIO_LP_CKN_MASK | \
								GPIO_LP_L1P_MASK | GPIO_LP_L1N_MASK)

#define DELAY_TXS()	usleep(2)

// Escape mode command list
#define MIPI_LP_ESC_LPDT 0x87

#define MIPI_CRC_POLY

static uint32_t MIPI_ECC_TABLE[] = {
		0b111100010010110010110111,
		0b111100100101010101011011,
		0b011101001001101001101101,
		0b101110001110001110001110,
		0b110111110000001111110000,
		0b111011111111110000000000
};

int gpio_lp_init(void) {
	gpio_odr_val = *GPIO0_ODR;
	GPIO_LP_11();
    *GPIO0_OER |= GPIO_ENABLE_MASK |
        GPIO_LP_CKP_MASK | GPIO_LP_CKN_MASK |
        GPIO_LP_L0P_MASK | GPIO_LP_L0N_MASK |
		GPIO_LP_L1P_MASK | GPIO_LP_L1N_MASK;
}

int gpio_lp_release(void) {
	*GPIO0_BCR = GPIO_ENABLE_MASK;

	return 0;
}

void gpio_lp_setup(void) {
	GPIO_LP_11();
	DELAY_TXS();
}

// Send EME sequence
static inline void gpio_lp_enter_escape(void) {
	GPIO_LP_10();
	DELAY_TXS();
	GPIO_LP_00();
	DELAY_TXS();
	GPIO_LP_01();
	DELAY_TXS();
	GPIO_LP_00();
	DELAY_TXS();
}

// Send Mark-1 sequence
static inline void gpio_lp_exit_escape(void) {
	GPIO_LP_00();
	DELAY_TXS();
	GPIO_LP_10();
	DELAY_TXS();
	GPIO_LP_11();
	DELAY_TXS();
}

// Shift 1 byte from LSB
static inline void gpio_lp_shift_byte(uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		if (byte & (1U << i)) {
			// Shift '1'
			GPIO_LP_10();
			DELAY_TXS();
			GPIO_LP_00();
			DELAY_TXS();
		}
		else {
			// Shift '0'
			GPIO_LP_01();
			DELAY_TXS();
			GPIO_LP_00();
			DELAY_TXS();
		}
	}
}

static void gpio_lpdt(uint8_t *payload, uint16_t length) {
	gpio_lp_enter_escape();
	gpio_lp_shift_byte(MIPI_LP_ESC_LPDT);
	for (uint16_t i = 0; i < length; i++) {
		gpio_lp_shift_byte(payload[i]);
	}
	gpio_lp_exit_escape();
}

static void gpio_lpdt_compute_ecc(uint8_t *ph) {
	uint32_t ph_val = ph[0] | ph[1] << 8U | ph[2] << 16U;
	uint8_t ecc_byte = 0;
	for (uint8_t i = 0; i < 6; i++) { // Each bit in table
		uint32_t field = ph_val & MIPI_ECC_TABLE[i];
		uint8_t parity = 0;

		// Calculate parity
		for(uint8_t j = 0; j < 24; j++) {
			parity ^= field;
			field >>= 1;
		}

		if(parity & 1) {
			ecc_byte |= 1 << i;
		}
	}

	ph[3] = ecc_byte;
}

static uint16_t gpio_lpdt_compute_crc(uint8_t *pkt_pd, uint16_t len) {
    uint16_t poly = 0x8408;

    int byte_counter;
    int bit_counter;
    uint8_t  current_data;
    uint16_t result = 0xffff;

    for (byte_counter = 0; byte_counter < len; byte_counter++) {
        current_data = pkt_pd[byte_counter];

        for (bit_counter = 0; bit_counter < 8; bit_counter++)
        {
            if (((result & 0x0001) ^ ((current_data) & 0x0001)))
                result = ((result >> 1) & 0x7fff) ^ poly;
            else
                result = ((result >> 1) & 0x7fff);
            current_data = (current_data >> 1); // & 0x7F;
        }
    }
    return result;
}

int gpio_lp_write_dcs(uint8_t command, uint8_t *param, uint16_t param_length) {
	if (param_length > 1) {
		// Compose a long write
		uint16_t packet_length = param_length + 7; // DI + WC0 + WC1 + ECC + CMD + CRC0 + CRC1
		uint8_t *packet_buffer = malloc(packet_length);
		if(!packet_buffer) return -1;

		packet_buffer[0] = 0x39; // DCS long write command
		packet_buffer[1] = (param_length + 1) & 0xFFU; // WC0
		packet_buffer[2] = (param_length + 1) >> 8U;   // WC1

		// Fill in ECC byte.
		gpio_lpdt_compute_ecc(packet_buffer);

		packet_buffer[4] = command;
		memcpy(&packet_buffer[5], param, param_length);

		// Compute CRC for LPa
		uint16_t crc = gpio_lpdt_compute_crc(&packet_buffer[4], param_length + 1);
		packet_buffer[packet_length - 2] = crc & 0xFF;
		packet_buffer[packet_length - 1] = crc >> 8U;

		gpio_lpdt(packet_buffer, packet_length);

		free(packet_buffer);
	}
	else if(param_length > 0) {
		// Compose a short write with 1 parameter
		uint8_t packet_buffer[4] = { 0x15, command, param[0], 0x00 };
		gpio_lpdt_compute_ecc(packet_buffer);

		// Shift the packet out.
		gpio_lpdt(packet_buffer, 0x04);
	} else {
		// Compose a short write without parameters
		uint8_t packet_buffer[4] = { 0x05, command, 0x00, 0x00 };
		gpio_lpdt_compute_ecc(packet_buffer); // Fill in ECC byte

		// Shift the packet out.
		gpio_lpdt(packet_buffer, 0x04);
	}

	return 0;
}
