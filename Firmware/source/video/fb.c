//
// fb.c: Framebuffer device middle layer
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
#include "fsl_elcdif.h"
#include "fb.h"
// Clock should already be enabled in clock_init()
// Pixel clock = 22 MHz
// Frame rate = 22M / (640 + 41 + 4 + 18) / (480 + 10 + 4 + 10) = 62 Hz

// Framebuffer in the NCACHE_REGION, last 1MB of the SDRAM
// Occupies 640*480*2 = 614KB
uint8_t *framebuffer = (uint8_t *)0x81f00000;

void fb_init(void) {
    // Reset LCDIF
    CLOCK_EnableClock(kCLOCK_Lcd);
    CLOCK_EnableClock(kCLOCK_LcdPixel);
    ELCDIF_Reset(LCDIF);
    NVIC_ClearPendingIRQ(LCDIF_IRQn);
    CLOCK_DisableClock(kCLOCK_LcdPixel);
    CLOCK_DisableClock(kCLOCK_Lcd);

    NVIC_SetPriority(LCDIF_IRQn, 3);
    EnableIRQ(LCDIF_IRQn);

    // Initialize LCDIF
    elcdif_rgb_mode_config_t elcdif_config = {0};

    elcdif_config.panelWidth = FB_WIDTH;
    elcdif_config.panelHeight = FB_HEIGHT;
    elcdif_config.hsw = FB_HSW;
    elcdif_config.hfp = FB_HFP;
    elcdif_config.hbp = FB_HBP;
    elcdif_config.vsw = FB_VSW;
    elcdif_config.vfp = FB_VFP;
    elcdif_config.vbp = FB_VBP;
    elcdif_config.bufferAddr = 0;
    elcdif_config.dataBus = FB_DB;
    elcdif_config.polarityFlags = FB_POL;

    ELCDIF_RgbModeInit(LCDIF, &elcdif_config);

    ELCDIF_RgbModeSetPixelFormat(LCDIF, FB_PIXFMT);

    ELCDIF_SetNextBufferAddr(LCDIF, (uint32_t)framebuffer);

    ELCDIF_RgbModeStart(LCDIF);

    // ELCDIF_EnableInterrupts(LCDIF, (uint32_t)kELCDIF_CurFrameDoneInterruptEnable);
}

void LCDIF_IRQHandler(void) {
    ELCDIF_ClearInterruptStatus(LCDIF, (uint32_t)kELCDIF_CurFrameDone);

}
