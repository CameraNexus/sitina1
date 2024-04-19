//
// Sitina1
// Copyright 2023 Wenting Zhang
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
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_mipi_dsi.h"
#include "vout.h"
#include "utils.h"
#include "fsl_debug_console.h"

#define BOARD_LCD_BL_GPIO GPIO11
#define BOARD_LCD_BL_PIN  7
#define BOARD_LCD_RST_GPIO GPIO11
#define BOARD_LCD_RST_PIN  16

#define LCD_CMD(params...) \
	{ \
		const uint8_t d[] = {params}; \
		vout_dsi_write(d, sizeof(d) / sizeof(uint8_t)); \
	}

void lcd_init(void) {
    const gpio_pin_config_t pinConfig = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_06_GPIO11_IO07,     /* GPIO_DISP_B2_06 is configured as GPIO11_IO07 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_DISP_B2_15_GPIO11_IO16,     /* GPIO_DISP_B2_15 is configured as GPIO11_IO16 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */

    GPIO_PinInit(BOARD_LCD_BL_GPIO, BOARD_LCD_BL_PIN, &pinConfig);
    GPIO_PinInit(BOARD_LCD_RST_GPIO, BOARD_LCD_RST_PIN, &pinConfig);

    GPIO_PinWrite(BOARD_LCD_RST_GPIO, BOARD_LCD_RST_PIN, 0);
    util_sleep_ms(100);
    GPIO_PinWrite(BOARD_LCD_RST_GPIO, BOARD_LCD_RST_PIN, 1);

    // Sleep out
    LCD_CMD(0x11);
    util_sleep_ms(200);

    // Initialization sequence
    LCD_CMD(0xB9, 0xF1, 0x12, 0x83); // Enable user command
    LCD_CMD(0xBA, // Set MIPI
    		0x31, // VC_main = 0, 2 lanes
			0x81, // DSI_LDO_SEL = 1.7V, RTERM = 90 ohm
			0x05, // IHSRX = x6 (low high speed driving ability)
			0xF9, // TX_CLK_SEL = fDSICLK/16
			0x0E, // HFP_OSC (min. HFP number in DSI)
			0x0E, // HBP_OSC (min. HBP number in DSI)
			// The rest are undocumented
			0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x44, 0x25, 0x00, 0x91, 0x0A, 0x00, 0x00, 0x02,
			0x4F, 0xD1, 0x00, 0x00, 0x37);
    LCD_CMD(0xB8, // Set power ext
    		0x25, // PCCS = 2, ECP_DC_DIV = 1/4 HSYNC
			0x22, // DT = 15ms XDK_ECP = x2
			0x20, // PFM_DC_DIV = /1
			0x03); // ECP_SYNC_EN = 1
    LCD_CMD(0xBF, 0x02, 0x11, 0x00); // Undocumented
    LCD_CMD(0xB3, // Set RGB IF
    		0x10, // VBP_RGB_GEN
			0x10, // VFP_RGB_GEN
			0x0A, // DE_BP_RGB_GEN
			0x50, // DE_FP_RGB_GEN
			// The rest are documented
			0x03, 0xFF, 0x00, 0x00, 0x00, 0x00);
    LCD_CMD(0xC0, // Set SCR
    		0x73, 0x73, 0x50, 0x50, 0x00, 0x00, 0x08, 0x70, 0x00);
    LCD_CMD(0xBC, 0x46); // Set VDC
    //LCD_CMD(0xCC, 0x0B); // Set panel (direction, polarity, BGR)
    LCD_CMD(0xCC, 0x07); // Set panel (direction, polarity, BGR)
    LCD_CMD(0xB4, 0x80); // Set CYC
    LCD_CMD(0xB2, // Set DISP
    		0x3C, // NL = 60
			0x12, // RESVLSB = 0, BLK_CON = VSSD, RESO_SEL = 720RGB
			0x30); // WHITE_FRAME = 3 frames, ISC = 0 frame
    LCD_CMD(0xE3, // Set EQ
    		0x07, 0x07, 0x0B, 0x0B, 0x03, 0x0B, 0x00, 0x00,
			0x00, 0x00, 0xFF, 0x00, 0x40, 0x10);
    LCD_CMD(0xC1, // Set POWER
    		0x65, 0x00, 0x32, 0x32, 0x77, 0xF1, 0xCC, 0xCC,
			0x77, 0x77, 0x33, 0x33);
    LCD_CMD(0xB5, 0x0A, 0x0A); // Set BGP
    LCD_CMD(0xB6, 0xB7, 0xB7); // Set VCOM
    LCD_CMD(0xE9, // Set forward GIP
    		0x88, 0x10, 0x0A, 0x10, 0x0F, 0xA1, 0x80, 0x12,
			0x31, 0x23, 0x47, 0x86, 0xA1, 0x80, 0x47, 0x08,
			0x04, 0x44, 0x00, 0x00, 0x00, 0x00, 0x04, 0x44,
			0x00, 0x00, 0x00, 0x00, 0x75, 0x31, 0x88, 0x88,
			0x88, 0x1F, 0x88, 0x38, 0xFF, 0x58, 0x88, 0x64,
			0x20, 0x88, 0x88, 0x88, 0x0F, 0x88, 0x28, 0xFF,
			0x48, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    LCD_CMD(0xEA, // Set backward GIP
    		0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x02, 0x46, 0x88, 0x88,
			0x88, 0x28, 0x8F, 0x08, 0xFF, 0x48, 0x88, 0x13,
			0x57, 0x88, 0x88, 0x88, 0x38, 0x8F, 0x18, 0xFF,
			0x58, 0x88, 0x23, 0x00, 0x00, 0x01, 0x02, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xA1,
			0x80, 0x00, 0x00, 0x00, 0x00);
    LCD_CMD(0xE0, // Set GAMMA
    		0x00, 0x03, 0x06, 0x2D, 0x3E, 0x3F, 0x34, 0x32,
			0x08, 0x0C, 0x0D, 0x10, 0x12, 0x11, 0x12, 0x10,
			0x15, 0x00, 0x03, 0x06, 0x2D, 0x3E, 0x3F, 0x34,
			0x32, 0x08, 0x0C, 0x0D, 0x10, 0x12, 0x11, 0x12,
			0x10, 0x15);
    LCD_CMD(0x29); // Display On


    // Read ID
    uint8_t rdid[1] = {0x04};
    uint8_t rdid_result[4] = {0x00, 0x00, 0x00, 0x00};
    uint32_t rdid_rxsize = 3;
    vout_dsi_read(rdid, 1, rdid_result, &rdid_rxsize);
    PRINTF("ID bytes: %d\r\n", rdid_rxsize);
    PRINTF("ID: %02x %02x %02x\r\n", rdid_result[0], rdid_result[1], rdid_result[2]);

    GPIO_PinWrite(BOARD_LCD_BL_GPIO, BOARD_LCD_BL_PIN, 1);
}

void lcd_bl_on(void) {
	GPIO_PinWrite(BOARD_LCD_BL_GPIO, BOARD_LCD_BL_PIN, 1);
}

void lcd_bl_off(void) {
	GPIO_PinWrite(BOARD_LCD_BL_GPIO, BOARD_LCD_BL_PIN, 0);
}
