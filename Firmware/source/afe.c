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
#include "afe.h"

#include "nano_shell.h"
#include "command/command.h"
#include "shell_io/shell_io.h"

// Horizontal Signals:
// H1 - AFE H1A
// H2 - AFE H2A
// H1BR - AFE H1B
// H2BR - AFE H2B
// When using Dual output mode, H1BR = H1, H2BR = H2
// When using Single output mode, H1BR = H2, H2BR = H1 (not possible?)

// GPO1 - STROBE
// GPO2 - PWR_EN
// XV1 - V1
// XV2 - V2
// XV3 - V2_3rd / VSG
// XV4 - FDG

// These settings only affect the memory layout.
// Actual used count could be less.
#define AFE_FIELD_COUNT (1)
#define AFE_VPAT_COUNT (3)
#define AFE_VSEQ_COUNT (4)

typedef struct {
    uint32_t value;
    bool dirty;
} REG_COPY;

typedef enum {
    R_FIELD,
    R_VSEQ,
    R_VPAT
} REG_TYPE;

// Only AFE base registers are saved in the copy
#define REG_COPY_COUNT 0xd8
static REG_COPY reg_copy[REG_COPY_COUNT];

#define CCD_DUMMY_PIX 4
#define CCD_DARK_PIX 19
#define CCD_BUFFER_PIX 13
#define CCD_ACTIVE_PIX 2004
#define CCD_LINE_WIDTH (CCD_DUMMY_PIX + CCD_DARK_PIX + CCD_BUFFER_PIX + CCD_ACTIVE_PIX)

#define CCD_SWEEP_LINES (-1) // TO BE FILLED IN
#define CCD_DUMMY_READ_LINES (2)
#define CCD_VSG_LINES (1)

#define CCD_DARK_LINES_U (16)
#define CCD_BUF_LINES_U (8)
#define CCD_ACTIVE_LINES (2672)
#define CCD_BUF_LINES_D (8)
#define CCD_DARK_LINES_D (17)
#define CCD_LINES (CCD_DARK_LINES_U + CCD_BUF_LINES_U + CCD_ACTIVE_LINES + CCD_BUF_LINES_D + CCD_DARK_LINES_D)
#define CCD_FIELD_LINES (CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES + CCD_VSG_LINES + CCD_LINES)

void afe_init_io(void) {
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        1,
        kGPIO_NoIntmode,
    };

    // IOs default to 1
    GPIO_PinInit(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, &config);
    GPIO_PinInit(AFE_MOSI_GPIO, AFE_MOSI_GPIO_PIN, &config);
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

void afe_set_reg(uint32_t reg, uint32_t offset, uint32_t mask, uint32_t val,
        bool update_immediately) {
    reg_copy[reg].value &= ~(mask << offset);
    reg_copy[reg].value |= (val & mask) << offset;
    if (update_immediately) {
        afe_write_reg(reg, reg_copy[reg].value);
        reg_copy[reg].dirty = false;
    }
    else {
        reg_copy[reg].dirty = true;
    }
}

void afe_init_reg_copy(void) {
    for (int i = 0; i < REG_COPY_COUNT; i++)
        reg_copy[i].dirty = false;
    // Initial values
    reg_copy[0x30] = 0x100 | (20 << 8);
    reg_copy[0x31] = 0x100 | (20 << 8);
    reg_copy[0x32] = 0x100 | (20 << 8);
    reg_copy[0x33] = 0x100 | (20 << 8);
    reg_copy[0x34] = 0x100 | (10 << 8);
    reg_copy[0x7a] = 0xff00;
}

void afe_set_conf_reg(REG_TYPE t, int group, uint32_t offset, uint32_t val) {
    uint32_t addr = 0;
    if (t == R_VPAT) // V-PATTERN
        addr = 0x400 + group * 48;
    else if (t == R_VSEQ) // V-SEQ
        addr = 0x400 + AFE_VPAT_COUNT * 48 + group * 40;
    else if (t == R_FIELD)
        addr = 0x400 + AFE_VPAT_COUNT * 48 + AFE_VSEQ_COUNT * 40 + group * 16;
    afe_write_reg(addr, val);
}

void afe_init(void) {
    afe_init_io();

    afe_write_reg(0x10, 0x1); // Software reset

    // Define the standby status of the AD9990 vertical outputs
    afe_set_reg(AFE_REG_VT_STBY12, 0x1ff8000, true);
    afe_set_reg(AFE_REG_VT_STBY3, 0x1ff8000, true);
    afe_set_reg(AFE_REG_VSGSELECT, 0xff8000, true);

    // Place the AFE into normal operation
    afe_set_reg(AFE_REG_VSGSELECT, 0x0, true);

    // Configure timing

    // Setting H driving strength to 4.3mA
    afe_set_reg(AFE_REG_H1ADRV, 0x1, false);
    afe_set_reg(AFE_REG_H2ADRV, 0x1, false);
    afe_set_reg(AFE_REG_HLADRV, 0x1, false);
    afe_set_reg(AFE_REG_RGADRV, 0x1, true);
    afe_set_reg(AFE_REG_H1BDRV, 0x1, false);
    afe_set_reg(AFE_REG_H2BDRV, 0x1, false);
    afe_set_reg(AFE_REG_HLBDRV, 0x1, false);
    afe_set_reg(AFE_REG_RGBDRV, 0x1, true);

    // HCLK mode 1: H1A = H1B, H2A = H2B = inverse of H1
    afe_set_reg(AFE_REG_HCLKMODE, 0x1, true);

    // Data output
    afe_set_reg(AFE_REG_DOUTPHASEP, 0x0, false);
    afe_set_reg(AFE_REG_DOUTPHASEN, 0x10, false); // In SDR mode, must be P+16
    afe_set_reg(AFE_REG_DCLK_EDGEMODE, 0x3, true); // SDR output

    afe_set_reg(AFE_REG_VSGSELECT, 0x4, true); // Select XV3 as VSG output

    afe_set_reg(AFE_REG_VPATNUM, AFE_VPAT_COUNT, false);
    afe_set_reg(AFE_REG_SEQNUM, AFE_VSEQ_COUNT, true);
    afe_set_reg(AFE_REG_MODE, AFE_FIELD_COUNT, true);
    afe_set_reg(AFE_REG_FIELD0, 0x0, true); // Use field 0

    SDK_DelayAtLeastUs(500, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    // Configure field
    afe_set_conf_reg(R_FIELD, 0, 0x00,
            (0 << 0) |  // Use seq 0 for region 0 (Fast Dump)
            (1 << 5) |  // Use seq 1 for region 1 (Dummy readout, 2 lines)
            (2 << 10) | // Use seq 2 for region 2 (VSG pulse)
            (1 << 15)); // Use seq 1 for region 3 (Image readout)
    afe_set_conf_reg(R_FIELD, 0, 0x01,
            (1 << 20)); // Sweep mode on for Region 0
    afe_set_conf_reg(R_FIELD, 0, 0x02,
            (CCD_LINE_WIDTH)); // Line length of the last line
    afe_set_conf_reg(R_FIELD, 0, 0x03,
            (0 << 0) |  // Enter Fast Dump mode immediately
            (CCD_SWEEP_LINES << 13)); // Enter Dummy readout mode
    afe_set_conf_reg(R_FIELD, 0, 0x04,
            (CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES) |
            ((CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES + CCD_VSG_LINES) << 13));
    afe_set_conf_reg(R_FIELD, 0, 0x07,
            (CCD_FIELD_LINES << 13)); // VD field length
    afe_set_conf_reg(R_FIELD, 0, 0x08,
            ((CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES)) | // SG active line
            (8191 << 13));
    afe_set_conf_reg(R_FIELD, 0, 0x0a, 8191); // Disable CLPOB region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0b, 8191); // Disable CLPOB region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0c, 8191); // Disable CLPOB region 3
    afe_set_conf_reg(R_FIELD, 0, 0x0d, 8191); // Disable PBLK region 1
    afe_set_conf_reg(R_FIELD, 0, 0x0e, 8191); // Disable PBLK region 2
    afe_set_conf_reg(R_FIELD, 0, 0x0f, 8191); // Disable PBLK region 3

    // Reset the internal timing core
    afe_set_reg(AFE_REG_TGCORE_RSTB, 0x1, true);

    // Enable External power supply (not enabled until out control = 1)
    afe_set_reg(AFE_REG_GP2_PROTOCOL, 0x7, true); // Keep on
    afe_set_reg(AFE_REG_GPO_OUTPUT_EN, 0xff, true);

    // Enable outputs
    afe_set_reg(AFE_REG_OUTCONTROL, 0x1, true);

    // Enable master mode operation
    afe_set_reg(AFE_REG_MASTER, 0x1, true);
}

void afe_start(void) {
    GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 0);
    SDK_DelayAtLeastUs(10, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    GPIO_PinWrite(AFE_SYNC_GPIO, AFE_SYNC_GPIO_PIN, 1);
}

void afe_power_down(void) {
    // Enter standby 3
    afe_set_reg(AFE_REG_STANDBY, 0x3, true);
}

int afe_cmd(const shell_cmd_t *pcmd, int argc, char *const argv[]) {
    if (argc == 1) {
        shell_printf("afe command expects an operation\n");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0) {
        afe_init();
        return 0;
    }
    shell_printf("invalid operation\n");
    return 1;
}

NANO_SHELL_ADD_CMD(afe, afe_cmd, "control afe", "afe debugging command");

