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
#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xv_frmbufrd.h"
#include "xscugic.h"
#include "xdsitxss.h"

#include "gpio_lp.h"


#define IMAGE_WIDTH  	 480
#define IMAGE_HEIGHT 	 480
#define IMAGE_PIXEL_SIZE 4

#define BITBANG_COMMAND 1
#define ENABLE_VIDEO 1

extern const unsigned char gImage_image480480[921600];

static uint8_t __aligned(64) s_image_buffer[IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE];

static XV_frmbufrd s_frmbufrd;
static XDsiTxSs s_dsi_txss;

static uint8_t s_lcd_init_seq[] = {
		0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x13,
		0xEF,  1, 0x08,
		0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x10,
		0xC0,  2, 0x3B, 0x00,
		0xC1,  2, 0x10, 0x0C,
		0xC2,  2, 0x31, 0x0A, // Inversion selection 31-2dot, 37-column
		0xC3,  1, 0x02,
		0xCC,  1, 0x10,
		0xCD,  1, 0x08,
	    0xB0, 16, 0x40, 0x0E, 0x58, 0x0E, 0x12, 0x08, 0x0C, 0x09, 0x09, 0x27, 0x07, 0x18, 0x15, 0x78, 0x26, 0xC7,
	    0xB1, 16, 0x40, 0x13, 0x5B, 0x0D, 0x11, 0x06, 0x0A, 0x08, 0x08, 0x26, 0x03, 0x13, 0x12, 0x79, 0x28, 0xC9,
	    0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x11,
	    0xB0,  1, 0x6D, //30
	    0xB1,  1, 0x38, //VCOM
	    0xB2,  1, 0x81, //84
	    0xB3,  1, 0x80,
	    0xB5,  1, 0x4E, //4E
	    0xB7,  1, 0x85,
	    0xB8,  1, 0x20,
	    0xC1,  1, 0x78,
	    0xC2,  1, 0x78,
	    0xD0,  1, 0x88,
	    0xE0,  3, 0x00, 0x00, 0x02,
	    0xE1, 11, 0x06, 0x30, 0x08, 0x30, 0x05, 0x30, 0x07, 0x30, 0x00, 0x33, 0x33,
	    0xE2, 12, 0x11, 0x11, 0x33, 0x33, 0xF4, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00, 0x00,
	    0xE3,  4, 0x00, 0x00, 0x11, 0x11,
	    0xE4,  2, 0x44, 0x44,
	    0xE5, 16, 0x0D, 0xF5, 0x30, 0xF0, 0x0F, 0xF7, 0x30, 0xF0, 0x09, 0xF1, 0x30, 0xF0, 0x0B, 0xF3, 0x30, 0xF0,
	    0xE6,  4, 0x00, 0x00, 0x11, 0x11,
	    0xE7,  2, 0x44, 0x44,
	    0xE8, 16, 0x0C, 0xF4, 0x30, 0xF0, 0x0E, 0xF6, 0x30, 0xF0, 0x08, 0xF0, 0x30, 0xF0, 0x0A, 0xF2, 0x30, 0xF0,
	    0xE9,  2, 0x36, 0x01,
	    0xEB,  7, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40,
	    0xED, 16, 0xFF, 0x45, 0x67, 0xFA, 0x01, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x10, 0xAF, 0x76, 0x54, 0xFF,
	    0xEF,  6, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F,
	    0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x00,
	    0x11,  0
};

static int init_drivers(void) {
	XV_frmbufrd_Config *frmbufrd_config = XV_frmbufrd_LookupConfig(XPAR_V_FRMBUF_RD_0_DEVICE_ID);
	int status = XV_frmbufrd_Initialize(&s_frmbufrd, frmbufrd_config->BaseAddress);
	if(status != XST_SUCCESS) {
		printf("FramebufRd initialization failed.\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

static void config_frmbuf(void) {
	XV_frmbufrd_Set_HwReg_width(&s_frmbufrd, IMAGE_WIDTH);
	XV_frmbufrd_Set_HwReg_height(&s_frmbufrd, IMAGE_HEIGHT);
	XV_frmbufrd_Set_HwReg_stride(&s_frmbufrd, IMAGE_WIDTH * IMAGE_PIXEL_SIZE);
	XV_frmbufrd_Set_HwReg_video_format(&s_frmbufrd, 27); // BGRX8
	XV_frmbufrd_Set_HwReg_frm_buffer_V(&s_frmbufrd, (u32)s_image_buffer);
	XV_frmbufrd_EnableAutoRestart(&s_frmbufrd);

	XV_frmbufrd_Start(&s_frmbufrd);

	printf("Frame buffer read configured.\r\n");
}


static int dsi_initseq(XDsiTxSs *txss) {
	int status = gpio_lp_write_dcs(0x01, NULL, 0x00);
	if(status != XST_SUCCESS) {
		printf("Reset command send failed.\r\n");
		return status;
	}

	usleep(200 * 1000);

	uint32_t i = 0;
	while(i < sizeof(s_lcd_init_seq)) {
		status = gpio_lp_write_dcs(s_lcd_init_seq[i], &s_lcd_init_seq[i + 2], s_lcd_init_seq[i + 1]);
		if(status != XST_SUCCESS) {
			printf("Send initialization command failed at %ld", i);
			return status;
		}
		i += s_lcd_init_seq[i + 1] + 2; // Next seq
	}

	usleep(200 * 1000);

	status = gpio_lp_write_dcs(0x29, NULL, 0x00);

	usleep(25 * 1000);

	gpio_lp_write_dcs(0x35, NULL, 0x00);

	return status;
}

static int config_dsi(void) {
	XDsiTxSs_Config *config = XDsiTxSs_LookupConfig(XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DEVICE_ID);

	if(!config) return XST_FAILURE;

	int status = XDsiTxSs_CfgInitialize(&s_dsi_txss, config, config->BaseAddr);
	if(status != XST_SUCCESS) {
		printf("DSI TX subsystem initialization failed.\r\n");
		return status;
	}

	status = XDsiTxSs_SelfTest(&s_dsi_txss);
	if(status != XST_SUCCESS) {
		printf("DSI TX subsystem self test failed.\r\n");
		return status;
	}
	else {
		printf("DSI TX subsystem self test succeeded.\r\n");
	}

	XDsiTxSs_Reset(&s_dsi_txss);

	gpio_lp_init();
	gpio_lp_setup();

	// Return all lanes to LP-11 state
	usleep(200 * 1000);

	dsi_initseq(&s_dsi_txss);

	gpio_lp_release();

	XDsiTxSs_Reset(&s_dsi_txss);

	XDsi_VideoTiming timing = {
			.HActive = 480 * 3,
			.HFrontPorch = 20,
			.HBackPorch = 56,
			.HSyncWidth = 8,
			.VActive = 480,
			.VFrontPorch = 40,
			.VBackPorch = 60,
			.VSyncWidth = 10,
			.BLLPBurst = 0,
	};

	XDsiTxSs_SetCustomVideoInterfaceTiming(&s_dsi_txss, XDSI_VM_NON_BURST_SYNC_EVENT, &timing);

	status = XDsiTxSs_Activate(&s_dsi_txss, XDSITXSS_PHY, XDSITXSS_ENABLE);
	if(status != XST_SUCCESS) {
		printf("D-PHY enable failed.\r\n");
	}

	status = XDsiTxSs_Activate(&s_dsi_txss, XDSITXSS_DSI, XDSITXSS_ENABLE);
	if(status != XST_SUCCESS) {
		printf("DSI enable failed.\r\n");
	}

	s_dsi_txss.SpktData.Data0 = 0x00;
	s_dsi_txss.SpktData.Data1 = 0x00;
	s_dsi_txss.SpktData.DataType = 0x32;
	s_dsi_txss.SpktData.VcId = 0;

	XDsiTxSs_SendShortPacket(&s_dsi_txss);

	return status;
}

int main()
{

    if(init_drivers() != XST_SUCCESS) {
    	printf("Driver initialization failed.\r\n");
    	for(;;) {
    		//
    	}
    }

    config_frmbuf();

    if(config_dsi() != XST_SUCCESS) {
    	printf("DSI TXSS initialization failed.\r\n");
    	for(;;) {
    		//
    	}
    }

    memset(s_image_buffer, 0xFF, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);

    //memcpy(s_image_buffer, gImage_image480480, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);
    Xil_DCacheFlushRange((UINTPTR)s_image_buffer, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);

    print("Hello World\n\r");
    print("Successfully ran Hello World application");

    for(;;) {
    	sleep(1);
    }

    return 0;
}
