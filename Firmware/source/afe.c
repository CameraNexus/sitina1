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
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "afe.h"
#include "ccd_timing.h"

#include "fsl_debug_console.h"

// Horizontal Signals:
// H1 - AFE H2A (Swapped!)
// H2 - AFE H1A
// H1BR - AFE H2B
// H2BR - AFE H1B
// When using Dual output mode, H1BR = H1, H2BR = H2
// When using Single output mode, H1BR = H2, H2BR = H1 (not possible?)

// GPO1 - STROBE
// GPO2 - PWR_EN
// XV1 - V1
// XV2 - V2
// XV3 - V2_3rd / VSG
// XV4 - FDG

// All the timing calculation are based on 24 MHz clock! (48 MP/s)
// Rated maximum data rate of KAI-11000/11002: 28 MHz (56 MP/s)
// At 24MHz, 1 pixel = 41.67ns

// These settings only affect the memory layout.
// Actual used count could be less.
#define AFE_FIELD_COUNT (1)
#define AFE_VPAT_COUNT (3)
#define AFE_VSEQ_COUNT (3)

// Base settings
#define AFE_SYNC_CONFIG (0x61) // External sync enable should be set even if using software sync

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
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        1,
        kGPIO_NoIntmode,
    };

    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_00_GPIO8_IO31,           /* GPIO_AD_00 is configured as GPIO8_IO31 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_01_GPIO9_IO00,           /* GPIO_AD_01 is configured as GPIO9_IO00 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_02_GPIO9_IO01,           /* GPIO_AD_02 is configured as GPIO9_IO01 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_31_GPIO9_IO30,           /* GPIO_AD_31 is configured as GPIO9_IO30 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_33_GPIO10_IO00,          /* GPIO_AD_33 is configured as GPIO10_IO00 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_35_GPIO10_IO02,          /* GPIO_AD_35 is configured as GPIO10_IO02 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */

    IOMUXC_SetPinConfig(
        IOMUXC_GPIO_AD_02_GPIO9_IO01,           /* GPIO_AD_02 PAD functional properties : */
        0x07U);                                 /* Slew Rate Field: Fast Slew Rate
                                                   Drive Strength Field: high drive strength
                                                   Pull / Keep Select Field: Pull Enable
                                                   Pull Up / Down Config. Field: Weak pull down
                                                   Open Drain Field: Disabled
                                                   Domain write protection: Both cores are allowed
                                                   Domain write protection lock: Neither of DWP bits is locked */

    // IOs default to 1
    GPIO_PinInit(AFE_MOSI_GPIO, AFE_MOSI_GPIO_PIN, &config);
    GPIO_PinInit(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, &config);
    GPIO_PinInit(AFE_CS_GPIO, AFE_CS_GPIO_PIN, &config);
    GPIO_PinInit(AFE_RST_GPIO, AFE_RST_GPIO_PIN, &config);
    GPIO_PinInit(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, &config);

    //config.direction = kGPIO_DigitalInput; // TODO
    config.outputLogic = 1;
    GPIO_PinInit(AFE_STROBE_GPIO, AFE_STROBE_GPIO_PIN, &config);
}

static void afe_delay() {
    volatile int x = 5;
    while (x--);
}

// Clock high on idle, latch on rising edge, LSB first
AT_QUICKACCESS_SECTION_CODE(void afe_write_byte(uint8_t byte)) {
    for (int i = 0; i < 8; i++) {
        GPIO_PinWrite(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, 0);
        GPIO_PinWrite(AFE_MOSI_GPIO, AFE_MOSI_GPIO_PIN, byte & 0x01);
        afe_delay();
        GPIO_PinWrite(AFE_SCK_GPIO, AFE_SCK_GPIO_PIN, 1);
        byte >>= 1;
        afe_delay();
    }
}

// 12 bit address, 28 bit data
AT_QUICKACCESS_SECTION_CODE(void afe_write_reg(uint32_t reg, uint32_t val)) {
    GPIO_PinWrite(AFE_CS_GPIO, AFE_CS_GPIO_PIN, 0);
    afe_write_byte(reg & 0xff);
    afe_write_byte(((reg >> 8) & 0xf) | ((val & 0xf) << 4));
    afe_write_byte((val >> 4) & 0xff);
    afe_write_byte((val >> 12) & 0xff);
    afe_write_byte((val >> 20) & 0xff);
    GPIO_PinWrite(AFE_CS_GPIO, AFE_CS_GPIO_PIN, 1);
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

    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(AFE_RST_GPIO, AFE_RST_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(AFE_RST_GPIO, AFE_RST_GPIO_PIN, 1);
    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    afe_write_reg(0x10, 0x1); // Software reset
    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    // Define the standby status of the AD9990 vertical outputs
    afe_write_reg(0x25, 0x1ff8000); // VT_STBY12
    afe_write_reg(0x26, 0x1ff8000); // VT_STBY3
    afe_write_reg(0x1c, 0xff8000); // VSGSELECT

    // Place the AFE into normal operation
    afe_write_reg(0x00, 0x04); // Standby

    // Divide clock by 2
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

    afe_write_reg(0x23, 0x0e); // 3.3V IO

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
    //afe_write_reg(0x35, 0x00); // Disable retime for H1, H2, HLA, HLB
    afe_write_reg(0x35, 0x0f); // Enable retime for H1, H2, HLA, HLB

    afe_write_reg(0x38,
            (31 << 0) | // SHDLOC
            (51 << 8) | // SHPLOC
            (10 << 16)); // SHPWIDTH

    // Data output
    afe_write_reg(0x39,
            (0 << 0) | // DOUTPHASEP = 0
            (16 << 8) | // DOUTPHASEN = 16, in SDR mode, must be P+16
            (3 << 20)); // SDR output

    //afe_write_reg(0x1c,
    //        (1 << 2)); // Select XV3 as VSG output

    afe_write_reg(0x28,
            (AFE_VPAT_COUNT << 0) | // VPATNUM
            (AFE_VSEQ_COUNT << 5)); // SEQNUM
    afe_write_reg(0x2a,
            (AFE_FIELD_COUNT << 0)); // MODE
    afe_write_reg(0x2b,
            (0 << 0)); // FIELD0 = 0
    afe_write_reg(0x2c, 0);

    // Configure timing

    // Configure field
//    afe_set_conf_reg(R_FIELD, 0, 0x00,
//            (0 << 0) |  // Use seq 0 for region 0 (Fast Dump)
//            (1 << 5) |  // Use seq 1 for region 1 (Dummy readout, 2 lines)
//            (2 << 10) | // Use seq 2 for region 2 (VSG pulse)
//            (1 << 15)); // Use seq 1 for region 3 (Image readout)
//    afe_set_conf_reg(R_FIELD, 0, 0x01,
//            (1 << 20)); // Sweep mode on for Region 0*/
//    afe_set_conf_reg(R_FIELD, 0, 0x02,
//            (CCD_LINE_LENGTH)); // Line length of the last line
//    afe_set_conf_reg(R_FIELD, 0, 0x03,
//            (0 << 0) |  // Enter Fast Dump mode immediately
//            (CCD_SWEEP_LINES << 13)); // Enter Dummy readout mode
//    afe_set_conf_reg(R_FIELD, 0, 0x04,
//            (CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES) |
//            ((CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES + CCD_VSG_LINES) << 13));
//    afe_set_conf_reg(R_FIELD, 0, 0x07,
//            (CCD_FIELD_LINES << 13)); // VD field length
//    afe_set_conf_reg(R_FIELD, 0, 0x08,
//            ((CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES)) | // SG active line
//            (8191 << 13));

    afe_set_conf_reg(R_FIELD, 0, 0x00,
            (1 << 0) |  // Use seq 1 for region 0 (Dummy readout, 2 lines)
            (2 << 5) |  // Use seq 2 for region 1 (VSG pulse)
            (1 << 10)); // Use seq 1 for region 2 (Image readout)
    afe_set_conf_reg(R_FIELD, 0, 0x01, 0);
    afe_set_conf_reg(R_FIELD, 0, 0x02,
            (CCD_LINE_LENGTH)); // Line length of the last line
    afe_set_conf_reg(R_FIELD, 0, 0x03,
            (0 << 0) |  // Enter Dummy readout mode immediately
            (CCD_DUMMY_READ_LINES << 13)); // Enter VSG pulse mode
    afe_set_conf_reg(R_FIELD, 0, 0x04,
            (CCD_DUMMY_READ_LINES + CCD_VSG_LINES));
    afe_set_conf_reg(R_FIELD, 0, 0x07,
            (CCD_FIELD_LINES << 13)); // VD field length
    afe_set_conf_reg(R_FIELD, 0, 0x08,
            ((CCD_DUMMY_READ_LINES)) | // SG active line
            (8191 << 13));


//    afe_set_conf_reg(R_FIELD, 0, 0x00,
//            (1 << 0));  // Use seq 1 for region 0 (Image readout)
//    afe_set_conf_reg(R_FIELD, 0, 0x01, 0);
//    afe_set_conf_reg(R_FIELD, 0, 0x02,
//            (CCD_LINE_LENGTH)); // Line length of the last line
//    afe_set_conf_reg(R_FIELD, 0, 0x03,
//            (0 << 0) |  // Enter Fast Dump mode immediately
//            (8191 << 13)); // Enter Dummy readout mode
//    afe_set_conf_reg(R_FIELD, 0, 0x04,
//            (8191 << 0) |
//            (8191 << 13));
//    afe_set_conf_reg(R_FIELD, 0, 0x05,
//            (8191 << 0) |
//            (8191 << 13));
//    afe_set_conf_reg(R_FIELD, 0, 0x06,
//            (8191 << 0) |
//            (8191 << 13));
//    afe_set_conf_reg(R_FIELD, 0, 0x07,
//            (8191 << 0) |
//            (CCD_LINES << 13)); // VD field length
//    afe_set_conf_reg(R_FIELD, 0, 0x08,
//            (CCD_DUMMY_READ_LINES) | // SG active line
//            (8191 << 13));

    afe_set_conf_reg(R_FIELD, 0, 0x0a, 8191); // Disable CLPOB region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0b, 8191); // Disable CLPOB region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0c, 8191); // Disable CLPOB region 3
    afe_set_conf_reg(R_FIELD, 0, 0x0d, 8191); // Disable PBLK region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0e, 8191); // Disable PBLK region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0f, 8191); // Disable PBLK region 3

    // Configure vertical sequence 0: fast dump
    afe_set_conf_reg(R_VSEQ, 0, 0x00,
            (1 << 0) | // CLPOB starts as high (not clamping)
            (0 << 1)); // PBLK starts as low (blanking)
    afe_set_conf_reg(R_VSEQ, 0, 0x01, CCD_LINE_LENGTH); // HD even line length
    afe_set_conf_reg(R_VSEQ, 0, 0x02, CCD_LINE_LENGTH); // HD odd line length
    afe_set_conf_reg(R_VSEQ, 0, 0x03, 0); // VSG pulse use toggle 1, toggle 2
    afe_set_conf_reg(R_VSEQ, 0, 0x04, CCD_LINE_LENGTH); // LASTREPLEN_A
    afe_set_conf_reg(R_VSEQ, 0, 0x05, 0); // LASTREPLEN C/D
    afe_set_conf_reg(R_VSEQ, 0, 0x06,
            (1 << 0) | // XV1 (V1) starts with high
            (0 << 1) | // XV2 (V2) starts with low
            (0 << 2) | // XV3 (VSG) starts with low
            (1 << 3)); // XV4 (FDG) starts with high
    afe_set_conf_reg(R_VSEQ, 0, 0x07, 0); // Assign V1 to V12 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x08, 0); // Assign V13 to V24 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x09, 0x0); // Assign VPAT0 to group A
    afe_set_conf_reg(R_VSEQ, 0, 0x0a,
            (0 << 0) | // VSTARTA = 0
            (CCD_DUMP_LENGTH << 13)); // VLENA = the total length
    afe_set_conf_reg(R_VSEQ, 0, 0x0b,
            (CCD_DUMP_LINES << 0) | // VREPA_1 = 1
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
            (CCD_LINE_LENGTH << 13)); // HBLKEND, always in blank
    afe_set_conf_reg(R_VSEQ, 0, 0x18,
            (CCD_LINE_LENGTH << 0) | // HBLKLEN
            (1 << 13) | // HBLKREP = 1
            (0 << 21) | // Masking polarity of H1 during HBLK is 0
            (1 << 22) | // Masking polarity of H2 during HBLK is 1
            (1 << 23) | // Masking polarity of HLA during HBLK is 1
            (1 << 24)); // Masking polarity of HLB during HBLK is 1
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
            (8191 << 0) | // CLPOB toggle position 1
            (8191 << 13)); // CLPOB toggle position 2
    afe_set_conf_reg(R_VSEQ, 0, 0x23,
            (8191 << 0) | // PBLK toggle position 1
            (8191 << 13)); // PBLK toggle position 2
    afe_set_conf_reg(R_VSEQ, 0, 0x24, 0); // HBLK offset A for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x25, 0); // HBLK offset B for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x26, 0); // HBLK offset C for mode 2
    afe_set_conf_reg(R_VSEQ, 0, 0x27, 0); // HBLK counter start position

    // Configure vertical pattern 0 for fast line dump
    for (int i = 0; i <= 0x2f; i++)
        afe_set_conf_reg(R_VPAT, 0, i, 0); // Set all positions to zero
    afe_set_conf_reg(R_VPAT, 0, 0x00,
            (CCD_TFD_PIX << 0) | // V1 toggle position 1
            (CCD_DUMP_VTOG << 13)); // V1 toggle position 2
    afe_set_conf_reg(R_VPAT, 0, 0x02,
            (CCD_TFD_PIX << 0) | // V2 toggle position 1
            (CCD_DUMP_VTOG << 13)); // V2 toggle position 2

    // Configure vertical sequence 1: image readout
    afe_set_conf_reg(R_VSEQ, 1, 0x00,
            (1 << 0) | // CLPOB starts as invalid
            (0 << 1)); // PBLK starts as valid
    afe_set_conf_reg(R_VSEQ, 1, 0x01, CCD_LINE_LENGTH); // HD even line length
    afe_set_conf_reg(R_VSEQ, 1, 0x02, CCD_LINE_LENGTH); // HD odd line length
    afe_set_conf_reg(R_VSEQ, 1, 0x03, 0); // VSG pulse use toggle 1, toggle 2
    afe_set_conf_reg(R_VSEQ, 1, 0x04, CCD_LINE_LENGTH); // LASTREPLEN_A
    afe_set_conf_reg(R_VSEQ, 1, 0x05, 0); // LASTREPLEN C/D
    afe_set_conf_reg(R_VSEQ, 1, 0x06,
            (1 << 0) | // XV1 (V1) starts with high
            (0 << 1) | // XV2 (V2) starts with low
            (0 << 2) | // XV3 (VSG) starts with low
            (0 << 3)); // XV4 (FDG) starts with low
    afe_set_conf_reg(R_VSEQ, 1, 0x07, 0); // Assign V1 to V12 to group A
    afe_set_conf_reg(R_VSEQ, 1, 0x08, 0); // Assign V13 to V24 to group A
    afe_set_conf_reg(R_VSEQ, 1, 0x09, 0x1); // Assign VPAT1 to group A
    afe_set_conf_reg(R_VSEQ, 1, 0x0a,
            (0 << 0) | // VSTARTA = 0
            (CCD_LINE_LENGTH << 13)); // VLENA = the total length
    afe_set_conf_reg(R_VSEQ, 1, 0x0b,
            (1 << 0) | // VREPA_1 = 1
            (0 << 13)); // VREPA_2 = 0
    afe_set_conf_reg(R_VSEQ, 1, 0x0c,
            (0 << 0) | // VREPA_3 = 0
            (0 << 13)); // VREPA_4 = 0
    afe_set_conf_reg(R_VSEQ, 1, 0x0d, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x0e, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x0f, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x10, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x11, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x12, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x13, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x14, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x15, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x16, 0);
    afe_set_conf_reg(R_VSEQ, 1, 0x17,
            (0 << 0) | // HBLKSTART = 0
            ((CCD_HBLK_LENGTH - 1) << 13)); // HBLKEND = CCD_HBLK_LENGTH
    afe_set_conf_reg(R_VSEQ, 1, 0x18,
            (CCD_HBLK_LENGTH << 0) | // HBLKLEN
            (1 << 13) | // HBLKREP = 1
            (0 << 21) | // Masking polarity of H1 during HBLK is 0
            (1 << 22) | // Masking polarity of H2 during HBLK is 1
            (1 << 23) | // Masking polarity of HLA during HBLK is 1
            (1 << 24)); // Masking polarity of HLB during HBLK is 1
    afe_set_conf_reg(R_VSEQ, 1, 0x19,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1a,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1b,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1c,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1d,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1e,
            (8191 << 0) |
            (8191 << 13));
    afe_set_conf_reg(R_VSEQ, 1, 0x1f,
            (8191 << 0) | // Disable HBLK repeat area start position A
            (8191 << 13));// Disable HBLK repeat area start position B
    afe_set_conf_reg(R_VSEQ, 1, 0x20,
            (8191 << 0) | // Disable HBLK repeat area start position C
            0); // FREEZE/RESUME not enabled
    afe_set_conf_reg(R_VSEQ, 1, 0x21, 0); // HBLK Odd field repeat area pattern
    afe_set_conf_reg(R_VSEQ, 1, 0x22,
            (CCD_CLPOB_BEGIN << 0) | // CLPOB toggle position 1
            (CCD_CLPOB_END << 13)); // CLPOB toggle position 2
//    afe_set_conf_reg(R_VSEQ, 1, 0x22,
//            (CCD_CLPOB_BEGIN << 0) | // CLPOB toggle position 1
//            (CCD_CLPOB_END << 13)); // CLPOB toggle position 2
    afe_set_conf_reg(R_VSEQ, 1, 0x23,
            (CCD_HBLK_LENGTH << 0) | // PBLK toggle position 1
            (8191 << 13)); // PBLK toggle position 2
    afe_set_conf_reg(R_VSEQ, 1, 0x24, 0); // HBLK offset A for mode 2
    afe_set_conf_reg(R_VSEQ, 1, 0x25, 0); // HBLK offset B for mode 2
    afe_set_conf_reg(R_VSEQ, 1, 0x26, 0); // HBLK offset C for mode 2
    afe_set_conf_reg(R_VSEQ, 1, 0x27, 0); // HBLK counter start position

    // Configure vertical pattern 1 for image readout
    for (int i = 0; i <= 0x2f; i++)
        afe_set_conf_reg(R_VPAT, 1, i, 0); // Set all positions to zero
    afe_set_conf_reg(R_VPAT, 1, 0x00,
            (1 << 0) | // V1 toggle position 1
            (CCD_TVCCD_PIX << 13)); // V1 toggle position 2
    afe_set_conf_reg(R_VPAT, 1, 0x02,
            (1 << 0) | // V2 toggle position 1
            (CCD_TVCCD_PIX << 13)); // V2 toggle position 2

    // Configure vertical sequence 2: vsg pulse
    afe_set_conf_reg(R_VSEQ, 2, 0x00,
            (1 << 0) | // CLPOB starts as high (not clamping)
            (0 << 1)); // PBLK starts as low (blanking)
    afe_set_conf_reg(R_VSEQ, 2, 0x01, CCD_VSG_LENGTH); // HD even line length
    afe_set_conf_reg(R_VSEQ, 2, 0x02, CCD_VSG_LENGTH); // HD odd line length
    afe_set_conf_reg(R_VSEQ, 2, 0x03, 0); // VSG pulse use toggle 1, toggle 2
    afe_set_conf_reg(R_VSEQ, 2, 0x04, CCD_VSG_LENGTH); // LASTREPLEN_A
    afe_set_conf_reg(R_VSEQ, 2, 0x05, 0); // LASTREPLEN C/D
    afe_set_conf_reg(R_VSEQ, 2, 0x06,
            (1 << 0) | // XV1 (V1) starts with high
            (0 << 1) | // XV2 (V2) starts with low
            (0 << 2) | // XV3 (VSG) starts with low
            (0 << 3)); // XV4 (FDG) starts with low
    afe_set_conf_reg(R_VSEQ, 2, 0x07, 0); // Assign V1 to V12 to group A
    afe_set_conf_reg(R_VSEQ, 2, 0x08, 0); // Assign V13 to V24 to group A
    afe_set_conf_reg(R_VSEQ, 2, 0x09, 0x2); // Assign VPAT2 to group A
    afe_set_conf_reg(R_VSEQ, 2, 0x0a,
            (0 << 0) | // VSTARTA = 0
            (CCD_VSG_LENGTH << 13)); // VLENA = the total length
    afe_set_conf_reg(R_VSEQ, 2, 0x0b,
            (1 << 0) | // VREPA_1 = 1
            (0 << 13)); // VREPA_2 = 0
    afe_set_conf_reg(R_VSEQ, 2, 0x0c,
            (0 << 0) | // VREPA_3 = 0
            (0 << 13)); // VREPA_4 = 0
    afe_set_conf_reg(R_VSEQ, 2, 0x0d, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x0e, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x0f, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x10, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x11, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x12, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x13, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x14, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x15, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x16, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x17,
            (0 << 0) | // HBLKSTART = 0
            ((CCD_VSG_LENGTH - 1) << 13)); // HBLKEND = CCD_VSG_LENGTH, always in blank
    afe_set_conf_reg(R_VSEQ, 2, 0x18,
            (CCD_VSG_LENGTH << 0) | // HBLKLEN
            (1 << 13) | // HBLKREP = 1
            (0 << 21) | // Masking polarity of H1 during HBLK is 0
            (1 << 22) | // Masking polarity of H2 during HBLK is 1
            (1 << 23) | // Masking polarity of HLA during HBLK is 1
            (1 << 24)); // Masking polarity of HLB during HBLK is 1
    afe_set_conf_reg(R_VSEQ, 2, 0x19, 0); // No toggle for HBLK
    afe_set_conf_reg(R_VSEQ, 2, 0x1a, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x1b, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x1c, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x1d, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x1e, 0);
    afe_set_conf_reg(R_VSEQ, 2, 0x1f,
            (8191 << 0) | // Disable HBLK repeat area start position A
            (8191 << 13));// Disable HBLK repeat area start position B
    afe_set_conf_reg(R_VSEQ, 2, 0x20,
            (8191 << 0) | // Disable HBLK repeat area start position C
            0); // FREEZE/RESUME not enabled
    afe_set_conf_reg(R_VSEQ, 2, 0x21, 0); // HBLK Odd field repeat area pattern
    afe_set_conf_reg(R_VSEQ, 2, 0x22,
            (8191 << 0) | // CLPOB toggle position 1
            (8191 << 13)); // CLPOB toggle position 2
    afe_set_conf_reg(R_VSEQ, 2, 0x23,
            (8191 << 0) | // PBLK toggle position 1
            (8191 << 13)); // PBLK toggle position 2
    afe_set_conf_reg(R_VSEQ, 2, 0x24, 0); // HBLK offset A for mode 2
    afe_set_conf_reg(R_VSEQ, 2, 0x25, 0); // HBLK offset B for mode 2
    afe_set_conf_reg(R_VSEQ, 2, 0x26, 0); // HBLK offset C for mode 2
    afe_set_conf_reg(R_VSEQ, 2, 0x27, 0); // HBLK counter start position

    // Configure vertical pattern 2 for vsg pulse
    for (int i = 0; i <= 0x2f; i++)
        afe_set_conf_reg(R_VPAT, 2, i, 0); // Set all positions to zero
    afe_set_conf_reg(R_VPAT, 2, 0x00,
            (CCD_T3P_PIX << 0) | // V1 toggle position 1
            ((CCD_T3P_PIX + CCD_TV3RD_PIX) << 13)); // V1 toggle position 2
    afe_set_conf_reg(R_VPAT, 2, 0x02,
            (1 << 0) | // V2 toggle position 1
            ((CCD_T3P_PIX + CCD_TV3RD_PIX + CCD_T3D_PIX) << 13)); // V2 toggle position 2
    afe_set_conf_reg(R_VPAT, 2, 0x04,
            ((CCD_T3P_PIX + 2 - CCD_TV3RD_COMP) << 0) | // V3 toggle position 1
            ((CCD_T3P_PIX + CCD_TV3RD_PIX) << 13)); // V3 toggle position 2

    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    // Release internal timing core reset
    afe_write_reg(0x14, 0x01);

    // Enable External power supply (not enabled until out control = 1)
    //afe_write_reg(0x73,
    //        (7 << 3)); // GP2_PROTOCOL = KEEP ON
//    afe_write_reg(0x73,
//            (7 << 0)); // GP1_PROTOCOL = KEEP ON
//    afe_write_reg(0x7a,
//            (0xff << 8) | // SEL_GPO = 0xff
//            (0xff << 16)); // GPO_OUTPUT_EN = 0xff

    //afe_write_reg(0xc3, 0x1);
    gpo_status = 0;

    // Enable outputs
    afe_write_reg(0x11, 0x1);

    // Enable master mode operation
    afe_write_reg(0x20, 0x1);
}

void afe_start(void) {
    GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(10, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 1);
    //afe_write_reg(0x13, AFE_SYNC_CONFIG | (1 << 24));
}

void afe_enable_power(void) {
    gpo_status |= 0x2;
    afe_write_reg(0xc3, gpo_status);
}

void afe_disable_power(void) {
    gpo_status &= ~0x2;
    afe_write_reg(0xc3, gpo_status);
}

AT_QUICKACCESS_SECTION_CODE(void afe_strobe(void)) {
//    gpo_status |= 0x1;
//    // TODO: Use AFE timing core to generate strobe?
//    // Strobe should be min 3us, typ 4us, max 10us
//    afe_write_reg(0xc3, gpo_status);
//    gpo_status &= ~0x1;
//    afe_write_reg(0xc3, gpo_status);
    GPIO_PinWrite(AFE_STROBE_GPIO, AFE_STROBE_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(4, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(AFE_STROBE_GPIO, AFE_STROBE_GPIO_PIN, 1);
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

//int afe_cmd(const shell_cmd_t *pcmd, int argc, char *const argv[]) {
//    if (argc == 1) {
//        shell_printf("afe command expects an operation\r\n");
//        return 1;
//    }
//    if (strcmp(argv[1], "init") == 0) {
//        afe_init();
//        return 0;
//    }
//    if (strcmp(argv[1], "start") == 0) {
//        afe_start();
//        return 0;
//    }
//    shell_printf("invalid operation\r\n");
//    return 1;
//}
