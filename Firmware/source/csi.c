//
// SitinaNe
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
// File : sdram.c
// Brief: External SDRAM control
#include "fsl_csi.h"
#include "fsl_iomuxc.h"
#include "afe.h"
#include "ccd_timing.h"

static csi_handle_t csi_handle;
volatile bool framedone;

static void csi_callback(CSI_Type *base, csi_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_CSI_FrameDone == status)
    {
        // Frame done, do sth
        framedone = true;
    }
    else
    {
        // Fail to receive a frame?
    }
}

void csi_init_io() {
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_06_VIDEO_MUX_CSI_DATA15,  /* GPIO_AD_06 is configured as VIDEO_MUX_CSI_DATA15 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_07_VIDEO_MUX_CSI_DATA14,  /* GPIO_AD_07 is configured as VIDEO_MUX_CSI_DATA14 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_08_VIDEO_MUX_CSI_DATA13,  /* GPIO_AD_08 is configured as VIDEO_MUX_CSI_DATA13 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_09_VIDEO_MUX_CSI_DATA12,  /* GPIO_AD_09 is configured as VIDEO_MUX_CSI_DATA12 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_10_VIDEO_MUX_CSI_DATA11,  /* GPIO_AD_10 is configured as VIDEO_MUX_CSI_DATA11 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_11_VIDEO_MUX_CSI_DATA10,  /* GPIO_AD_11 is configured as VIDEO_MUX_CSI_DATA10 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_12_VIDEO_MUX_CSI_PIXCLK,  /* GPIO_AD_12 is configured as VIDEO_MUX_CSI_PIXCLK */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_14_VIDEO_MUX_CSI_VSYNC,  /* GPIO_AD_14 is configured as VIDEO_MUX_CSI_VSYNC */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_15_VIDEO_MUX_CSI_HSYNC,  /* GPIO_AD_15 is configured as VIDEO_MUX_CSI_HSYNC */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_16_VIDEO_MUX_CSI_DATA09,  /* GPIO_AD_16 is configured as VIDEO_MUX_CSI_DATA09 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_17_VIDEO_MUX_CSI_DATA08,  /* GPIO_AD_17 is configured as VIDEO_MUX_CSI_DATA08 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_18_VIDEO_MUX_CSI_DATA07,  /* GPIO_AD_18 is configured as VIDEO_MUX_CSI_DATA07 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_19_VIDEO_MUX_CSI_DATA06,  /* GPIO_AD_19 is configured as VIDEO_MUX_CSI_DATA06 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_20_VIDEO_MUX_CSI_DATA05,  /* GPIO_AD_20 is configured as VIDEO_MUX_CSI_DATA05 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_21_VIDEO_MUX_CSI_DATA04,  /* GPIO_AD_21 is configured as VIDEO_MUX_CSI_DATA04 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_22_VIDEO_MUX_CSI_DATA03,  /* GPIO_AD_22 is configured as VIDEO_MUX_CSI_DATA03 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinMux(
        IOMUXC_GPIO_AD_23_VIDEO_MUX_CSI_DATA02,  /* GPIO_AD_23 is configured as VIDEO_MUX_CSI_DATA02 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
}


void csi_init() {
    csi_init_io();

    // DVP connect to CSI
    CLOCK_EnableClock(kCLOCK_Video_Mux);
    VIDEO_MUX->VID_MUX_CTRL.CLR = VIDEO_MUX_VID_MUX_CTRL_CSI_SEL_MASK;

    // Set CSI clock
    const clock_root_config_t csiClockConfig = {
        .clockOff = false,
        .mux      = kCLOCK_CSI_ClockRoot_MuxSysPll3Out,
        .div      = 6,
    };
    CLOCK_SetRootClock(kCLOCK_Root_Csi, &csiClockConfig);

    csi_config_t config;

    CSI_GetDefaultConfig(&config);

    // Probably exact size not important... IMXRT105x's CSI doesn't even care HSYNC
    config.width           = CCD_PRV_LINE_LENGTH * 2;
    config.height          = CCD_PRV_FIELD_LINES;
    config.polarityFlags   = (uint32_t)kCSI_VsyncActiveLow | (uint32_t)kCSI_DataLatchOnFallingEdge;
    config.bytesPerPixel   = 2U;
    config.linePitch_Bytes = CCD_PRV_LINE_LENGTH * 2 * 2U;
    config.workMode        = kCSI_NonGatedClockMode;
    config.dataBus         = kCSI_DataBus16Bit;
    config.useExtVsync     = true;

    CSI_Init(CSI, &config);

    CSI_TransferCreateHandle(CSI, &csi_handle, csi_callback, NULL);
}

void csi_switch_resultion(uint32_t width, uint32_t height) {
    csi_config_t config;

    CSI_GetDefaultConfig(&config);

    // Probably exact size not important... IMXRT105x's CSI doesn't even care HSYNC
    config.width           = width;
    config.height          = height;
    config.polarityFlags   = (uint32_t)kCSI_VsyncActiveLow | (uint32_t)kCSI_DataLatchOnFallingEdge;
    config.bytesPerPixel   = 2U;
    config.linePitch_Bytes = width * 2U;
    config.workMode        = kCSI_NonGatedClockMode;
    config.dataBus         = kCSI_DataBus16Bit;
    config.useExtVsync     = true;

    CSI_Init(CSI, &config);
}

void csi_start() {
    framedone = false;
    CSI_TransferStart(CSI, &csi_handle);
}

void csi_stop() {
    CSI_TransferStop(CSI, &csi_handle);
}

void csi_submit_empty_buffer(uint8_t *buffer) {
    CSI_TransferSubmitEmptyBuffer(CSI, &csi_handle, (uint32_t)buffer);
}

uint8_t *csi_get_full_buffer() {
    uint32_t addr;
    status_t result;
    result = CSI_TransferGetFullBuffer(CSI, &csi_handle, &addr);
    if (result == kStatus_Success)
        return (uint8_t *)addr;
    return NULL;
}

void csi_wait_framedone() {
    while (!framedone);
}
