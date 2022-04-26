/*
 * csi.c
 *
 *  Created on: Apr 8, 2022
 *      Author: wenting
 */

#include "fsl_csi.h"
#include "afe.h"

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


void csi_init() {
    csi_config_t config;

    CSI_GetDefaultConfig(&config);

    // Probably exact size not important... IMXRT105x's CSI doesn't even care HSYNC
    config.width           = CCD_LINE_LENGTH * 2;
    config.height          = CCD_FIELD_LINES + 1;
    config.polarityFlags   = (uint32_t)kCSI_VsyncActiveLow | (uint32_t)kCSI_DataLatchOnFallingEdge;
    config.bytesPerPixel   = 2U;
    config.linePitch_Bytes = CCD_LINE_LENGTH * 2 * 2U;
    config.workMode        = kCSI_NonGatedClockMode;
    config.dataBus         = kCSI_DataBus16Bit;
    config.useExtVsync     = true;

    CSI_Init(CSI, &config);

    CSI_TransferCreateHandle(CSI, &csi_handle, csi_callback, NULL);
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
