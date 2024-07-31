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
#include <stdint.h>
#include <stdbool.h>
#include "mu_platform.h"
#include "afe.h"
#include "ccd_timing.h"

#define AFE_MOSI_GPIO_PIN   GPIO_PIN_2
#define AFE_SCK_GPIO_PIN    GPIO_PIN_4
#define AFE_CS_GPIO_PIN     GPIO_PIN_3
#define AFE_RST_GPIO_PIN    GPIO_PIN_0
#define AFE_SYNC_GPIO_PIN   GPIO_PIN_1

// This is the active field count. Programmed numbers can be higher
#define AFE_FIELD_COUNT (1)
// These settings only affect the memory layout.
// Actual used count could be less.
#define AFE_VPAT_COUNT (1)
#define AFE_VSEQ_COUNT (1)

// Up to 3072 registers
// V-PAT registers are 48 each
// V-SEQ registers are 40 each
// FIELD registers are 16 each

// Base settings
#define AFE_SYNC_CONFIG (0x62) // External sync disabled

typedef struct {
    uint32_t value;
    bool dirty;
} REG_COPY;

typedef enum {
    R_FIELD,
    R_VSEQ,
    R_VPAT
} REG_TYPE;

static uint32_t gpo_status;

void afe_init_io(void) {
	*GPIO0_BSR = AFE_MOSI_GPIO_PIN | AFE_SCK_GPIO_PIN | AFE_CS_GPIO_PIN |
			AFE_RST_GPIO_PIN | AFE_SYNC_GPIO_PIN;
	*GPIO0_OER |= AFE_MOSI_GPIO_PIN | AFE_SCK_GPIO_PIN | AFE_CS_GPIO_PIN |
			AFE_RST_GPIO_PIN | AFE_SYNC_GPIO_PIN;
}

static void afe_delay() {
    volatile int x = 20;
    while (x--);
}

// Clock high on idle, latch on rising edge, LSB first
void afe_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
    	*GPIO0_BCR = AFE_SCK_GPIO_PIN;
    	if (byte & 0x01)
    		*GPIO0_BSR = AFE_MOSI_GPIO_PIN;
    	else
    		*GPIO0_BCR = AFE_MOSI_GPIO_PIN;
        afe_delay();
        *GPIO0_BSR = AFE_SCK_GPIO_PIN;
        byte >>= 1;
        afe_delay();
    }
}

// 12 bit address, 28 bit data
void afe_write_reg(uint32_t reg, uint32_t val) {
	*GPIO0_BCR = AFE_CS_GPIO_PIN;
    afe_write_byte(reg & 0xff);
    afe_write_byte(((reg >> 8) & 0xf) | ((val & 0xf) << 4));
    afe_write_byte((val >> 4) & 0xff);
    afe_write_byte((val >> 12) & 0xff);
    afe_write_byte((val >> 20) & 0xff);
    *GPIO0_BSR = AFE_CS_GPIO_PIN;
}

void afe_set_conf_reg(REG_TYPE t, int group, uint32_t offset, uint32_t val) {
    uint32_t addr = 0;
    if (t == R_VPAT) // V-PATTERN
        addr = 0x400 + group * 48;
    else if (t == R_VSEQ) // V-SEQ
        addr = 0x400 + AFE_VPAT_COUNT * 48 + group * 40;
    else if (t == R_FIELD)
        addr = 0x400 + AFE_VPAT_COUNT * 48 + AFE_VSEQ_COUNT * 40 + group * 16;
    addr += offset;
    afe_write_reg(addr, val);
}

void afe_init(void) {
    afe_init_io();

    usleep(500);
    *GPIO0_BCR = AFE_RST_GPIO_PIN;
    usleep(500);
    *GPIO0_BSR = AFE_RST_GPIO_PIN;
    usleep(500);

    afe_write_reg(0x10, 0x1); // Software reset
    usleep(500);

    // Define the standby status of the AD9990 vertical outputs
    afe_write_reg(0x25, 0x1ff8000); // VT_STBY12
    afe_write_reg(0x26, 0x1ff8000); // VT_STBY3
    afe_write_reg(0x1c, 0xff8000); // VSGSELECT

    // Place the AFE into normal operation
    afe_write_reg(0x00, 0x24); // Standby

    // Don't divide clock by 2
    afe_write_reg(0x0d, 0x00);

    // Mode setup
    afe_write_reg(0x13, AFE_SYNC_CONFIG);

    afe_write_reg(0x15, 0x01); // Enable CLO oscillator

    // Setting H driving strength to 4.3mA
    afe_write_reg(0x36,
            (1 << 0) | // H1ADRV = 4.3mA
            (1 << 4) | // H2ADRV = 4.3mA
            (1 << 16) | // HLADRV = 4.3mA
            (1 << 20)); // RGADRV = 4.3mA
    afe_write_reg(0x37,
            (1 << 0) | // H1BDRV = 4.3mA
            (1 << 4) | // H2BDRV = 4.3mA
            (1 << 16) | // HLBDRV = 4.3mA
            (1 << 20)); // RGBDRV = 4.3mA

    afe_write_reg(0x23, 0x00); // 1.8V IO

    // HCLK mode 1: H1A = H1B, H2A = H2B = inverse of H1
    // HCLK mode 2: H1A = H1B, H2A = H2B, individually programmable
    afe_write_reg(0x24, 0x2);

    afe_write_reg(0x30,
            (4 << 0) |  // H1 rising edge location
            (32 << 8) | // H1 falling edge location
            (1 << 16)); // Must be 1
    afe_write_reg(0x31,
            (36 << 0) |  // H2 rising edge location
            (0 << 8) | // H2 falling edge location
            (1 << 16)); // Must be 1
    afe_write_reg(0x32,
            (4 << 0) |  // HLA rising edge location
            (14 << 8) | // HLA falling edge location
            (1 << 16)); // Must be 1
    afe_write_reg(0x33,
            (0 << 0) |  // HLB rising edge location
            (32 << 8) | // HLB falling edge location
            (1 << 16)); // Must be 1
    afe_write_reg(0x34,
            (0 << 0) |  // RG rising edge location
            (16 << 8) | // RG falling edge location //TODO: Should be 8
            (1 << 16)); // Must be 1
    afe_write_reg(0x35, 0x00); // Disable retime for H1, H2, HLA, HLB

    afe_write_reg(0x38,
            (0 << 0) | // SHDLOC
            (32 << 8) | // SHPLOC
            (10 << 16)); // SHPWIDTH

    // Data output
    // afe_write_reg(0x39,
    //         (0 << 0) | // DOUTPHASEP = 0
    //         (16 << 8) | // DOUTPHASEN = 16, in SDR mode, must be P+16
    //         (3 << 20)); // SDR output
    afe_write_reg(0x39,
            (0 << 0) | // DOUTPHASEP = 0
            (32 << 8) | // DOUTPHASEN = 32
            (0 << 20)); // DDR output

    afe_write_reg(0x28,
            (AFE_VPAT_COUNT << 0) | // VPATNUM
            (AFE_VSEQ_COUNT << 5)); // SEQNUM
    afe_write_reg(0x2a,
            (AFE_FIELD_COUNT << 0)); // MODE
    afe_write_reg(0x2b,
            (0 << 0)); // FIELD0 = 0 // Boot up in field 0
    afe_write_reg(0x2c, 0);

    // Configure timing

    // Configure field
    // Field 0: Full resolution readout
    afe_set_conf_reg(R_FIELD, 0, 0x00,
            (0 << 0));  // Use seq 0 for region 0 (Image readout)
    afe_set_conf_reg(R_FIELD, 0, 0x01, 0);
    afe_set_conf_reg(R_FIELD, 0, 0x02,
            (CCD_LINE_LENGTH)); // Line length of the last line
    afe_set_conf_reg(R_FIELD, 0, 0x03,
            (0 << 0));  // Enter readout mode immediately
    afe_set_conf_reg(R_FIELD, 0, 0x07,
            (CCD_LINES << 13)); // VD field length
    afe_set_conf_reg(R_FIELD, 0, 0x08,
            (8191) | // SG active line
            (8191 << 13));

    afe_set_conf_reg(R_FIELD, 0, 0x0a, 8191); // Disable CLPOB region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0b, 8191); // Disable CLPOB region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0c, 8191); // Disable CLPOB region 3
    afe_set_conf_reg(R_FIELD, 0, 0x0d, 8191); // Disable PBLK region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0e, 8191); // Disable PBLK region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0f, 8191); // Disable PBLK region 3

    // Configure vertical sequence 0: image readout
    afe_set_conf_reg(R_VSEQ, 0, 0x00,
            (1 << 0) | // CLPOB starts as invalid
            (0 << 1)); // PBLK starts as valid
    afe_set_conf_reg(R_VSEQ, 0, 0x01, CCD_LINE_LENGTH); // HD even line length
    afe_set_conf_reg(R_VSEQ, 0, 0x02, CCD_LINE_LENGTH); // HD odd line length
    afe_set_conf_reg(R_VSEQ, 0, 0x03, 0); // VSG pulse use toggle 1, toggle 2
    afe_set_conf_reg(R_VSEQ, 0, 0x04, CCD_LINE_LENGTH); // LASTREPLEN_A
    afe_set_conf_reg(R_VSEQ, 0, 0x05, 0); // LASTREPLEN C/D
    afe_set_conf_reg(R_VSEQ, 0, 0x06, 0); // Polarity doesn't matter
    afe_set_conf_reg(R_VSEQ, 0, 0x07, 0); // Assign V1 to V12 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x08, 0); // Assign V13 to V24 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x09, 0x0); // Assign VPAT0 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x0a,
            (0 << 0) | // VSTARTA = 0
            (CCD_LINE_LENGTH << 13)); // VLENA = the total length
    afe_set_conf_reg(R_VSEQ, 0, 0x0b,
            (1 << 0) | // VREPA_1 = 1
            (0 << 13)); // VREPA_2 = 0
    afe_set_conf_reg(R_VSEQ, 0, 0x0c,
            (0 << 0) | // VREPA_3 = 0
            (0 << 13)); // VREPA_4 = 0
    afe_set_conf_reg(R_VSEQ, 0, 0x0d, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x0e, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x0f, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x10, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x11, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x12, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x13, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x14, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x15, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x16, 0);
    afe_set_conf_reg(R_VSEQ, 0, 0x17,
            (0 << 0) | // HBLKSTART = 0
            ((CCD_HBLK_LENGTH - 1) << 13)); // HBLKEND = CCD_HBLK_LENGTH
    afe_set_conf_reg(R_VSEQ, 0, 0x18,
            (CCD_HBLK_LENGTH << 0) | // HBLKLEN
            (1 << 13)); // HBLKREP = 1
    afe_set_conf_reg(R_VSEQ, 0, 0x19,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1a,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1b,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1c,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1d,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1e,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 0, 0x1f,
            (8191 << 0) | // Disable HBLK repeat area start position A
            (8191 << 13));// Disable HBLK repeat area start position B
    afe_set_conf_reg(R_VSEQ, 0, 0x20,
            (8191 << 0) | // Disable HBLK repeat area start position C
            0); // FREEZE/RESUME not enabled
    afe_set_conf_reg(R_VSEQ, 0, 0x21, 0); // HBLK Odd field repeat area pattern
    afe_set_conf_reg(R_VSEQ, 0, 0x22,
            (CCD_CLPOB_BEGIN << 0) | // CLPOB toggle position 1
            (CCD_CLPOB_END << 13)); // CLPOB toggle position 2
    afe_set_conf_reg(R_VSEQ, 0, 0x23,
            (CCD_HBLK_LENGTH << 0) | // PBLK toggle position 1
            (8191 << 13)); // PBLK toggle position 2
    afe_set_conf_reg(R_VSEQ, 0, 0x24, 0); // HBLK offset A for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x25, 0); // HBLK offset B for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x26, 0); // HBLK offset C for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x27, 0); // HBLK counter start position

    // Configure vertical pattern 0 for image readout
    for (int i = 0; i <= 0x2f; i++)
        afe_set_conf_reg(R_VPAT, 0, i, 0); // Set all positions to zero

    usleep(500);

    // Release internal timing core reset
    afe_write_reg(0x14, 0x01);

    afe_write_reg(0x71, 1);

    afe_write_reg(0x70, 1);

    //afe_write_reg(0xc3, 0x1);
    gpo_status = 0;

    // Enable outputs
    afe_write_reg(0x11, 0x1);

    // Enable master mode operation
    //afe_write_reg(0x20, 0x1);
    afe_write_reg(0x20, 0x0);

    // Enable slave mode operation
    afe_write_reg(0x0e, 0x100);
}

void afe_start(void) {
    // GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 0);
    // SDK_DelayAtLeastUs(10, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    // GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 1);
    afe_write_reg(0x13, AFE_SYNC_CONFIG | (1 << 24));
}

void afe_stop(void) {
    // Stop master mode operation
    afe_write_reg(0x20, 0x0);
    afe_write_reg(0x14, 0x0); // Reset timing core
}

void afe_power_down(void) {
    // Enter standby 3
    //afe_set_reg(AFE_REG_STANDBY, 0x3, true);
}

void afe_switch_to_draft(void) {

}

void afe_switch_to_still(void) {

}

void afe_capture_finish(void) {
    
}
