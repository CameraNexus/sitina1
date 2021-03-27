/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_sdmmc_host.h"
#include "fsl_sdmmc_common.h"
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
#include "fsl_cache.h"
#endif
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT (~0U)
#define SDMMCHOST_TRANSFER_CMD_EVENT                                                                                   \
    SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL | SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | \
        SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL
#define SDMMCHOST_TRANSFER_DATA_EVENT SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS | SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SDMMCHOST detect card insert status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardInsertByHost(USDHC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST detect card remove status by host controller.
 * @param base host base address.
 * @param userData user can register a application card insert callback through userData.
 */
static void SDMMCHOST_DetectCardRemoveByHost(USDHC_Type *base, void *userData);

/*!
 * @brief SDMMCHOST transfer complete callback.
 * @param base host base address.
 * @param handle host handle.
 * @param status interrupt status.
 * @param userData user data.
 */
static void SDMMCHOST_TransferCompleteCallback(USDHC_Type *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData);

/*!
 * @brief SDMMCHOST error recovery.
 * @param base host base address.
 */
static void SDMMCHOST_ErrorRecovery(USDHC_Type *base);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SDMMCHOST_DetectCardInsertByHost(USDHC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
    (void)SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(true, cd->userData);
            }
            if (cd->type == kSD_DetectCardByHostDATA3)
            {
                USDHC_DisableInterruptSignal(base, kUSDHC_CardInsertionFlag);
                if (cd->dat3PullFunc != NULL)
                {
                    cd->dat3PullFunc(kSD_DAT3PullUp);
                }
            }
        }
    }
}

static void SDMMCHOST_DetectCardRemoveByHost(USDHC_Type *base, void *userData)
{
    sd_detect_card_t *cd = NULL;

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
    (void)SDMMC_OSAEventClear(&(((sdmmchost_t *)userData)->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);

    if (userData != NULL)
    {
        cd = (sd_detect_card_t *)(((sdmmchost_t *)userData)->cd);
        if (cd != NULL)
        {
            if (cd->callback != NULL)
            {
                cd->callback(false, cd->userData);
            }

            if (cd->type == kSD_DetectCardByHostDATA3)
            {
                USDHC_DisableInterruptSignal(base, kUSDHC_CardRemovalFlag);
            }
        }
    }
}

static void SDMMCHOST_CardInterrupt(USDHC_Type *base, void *userData)
{
    sdio_card_int_t *cardInt = NULL;

    /* application callback */
    if (userData != NULL)
    {
        cardInt = ((sdmmchost_t *)userData)->cardInt;
        if ((cardInt != NULL) && (cardInt->cardInterrupt != NULL))
        {
            cardInt->cardInterrupt(cardInt->userData);
        }
    }
}

status_t SDMMCHOST_CardIntInit(sdmmchost_t *host, void *sdioInt)
{
    host->cardInt                       = sdioInt;
    host->handle.callback.SdioInterrupt = SDMMCHOST_CardInterrupt;
    SDMMCHOST_EnableCardInt(host, true);

    return kStatus_Success;
}

status_t SDMMCHOST_CardDetectInit(sdmmchost_t *host, void *cd)
{
    USDHC_Type *base       = host->hostController.base;
    sd_detect_card_t *sdCD = (sd_detect_card_t *)cd;
    if (cd == NULL)
    {
        return kStatus_Fail;
    }

    host->cd                           = cd;
    host->handle.callback.CardInserted = SDMMCHOST_DetectCardInsertByHost;
    host->handle.callback.CardRemoved  = SDMMCHOST_DetectCardRemoveByHost;
    if (sdCD->type == kSD_DetectCardByHostDATA3)
    {
        USDHC_CardDetectByData3(base, true);
    }
    else if (sdCD->type == kSD_DetectCardByHostCD)
    {
        USDHC_CardDetectByData3(base, false);
    }
    else
    {
        assert(false);
    }

    /* enable card detect interrupt */
    USDHC_ClearInterruptStatusFlags(base, (uint32_t)kUSDHC_CardInsertionFlag | (uint32_t)kUSDHC_CardRemovalFlag);
    USDHC_EnableInterruptStatus(base, (uint32_t)kUSDHC_CardInsertionFlag | (uint32_t)kUSDHC_CardRemovalFlag);

    if (SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Inserted)
    {
        (void)SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED);
        /* notify application about the card insertion status */
        if (sdCD->callback != NULL)
        {
            sdCD->callback(true, sdCD->userData);
        }
    }
    else
    {
        (void)SDMMC_OSAEventSet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_REMOVED);
        USDHC_EnableInterruptSignal(base, kUSDHC_CardInsertionFlag);
    }

    return kStatus_Success;
}

uint32_t SDMMCHOST_CardDetectStatus(sdmmchost_t *host)
{
    sd_detect_card_t *sdCD = (sd_detect_card_t *)(host->cd);
    uint32_t insertStatus  = kSD_Removed;

    if (sdCD->dat3PullFunc != NULL)
    {
        sdCD->dat3PullFunc(kSD_DAT3PullDown);
        SDMMC_OSADelay(1U);
    }

    if ((USDHC_GetPresentStatusFlags(host->hostController.base) & (uint32_t)kUSDHC_CardInsertedFlag) != 0U)
    {
        insertStatus = kSD_Inserted;
        if (sdCD->dat3PullFunc != NULL)
        {
            sdCD->dat3PullFunc(kSD_DAT3PullUp);
        }
    }

    return insertStatus;
}

status_t SDMMCHOST_PollingCardDetectStatus(sdmmchost_t *host, uint32_t waitCardStatus, uint32_t timeout)
{
    assert(host != NULL);
    assert(host->cd != NULL);

    sd_detect_card_t *cd = host->cd;
    uint32_t event       = 0U;

    (void)SDMMC_OSAEventGet(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED, &event);
    if ((((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED) &&
         (waitCardStatus == (uint32_t)kSD_Inserted)) ||
        (((event & SDMMC_OSA_EVENT_CARD_REMOVED) == SDMMC_OSA_EVENT_CARD_REMOVED) &&
         (waitCardStatus == (uint32_t)kSD_Removed)))
    {
        return kStatus_Success;
    }

    /* Wait card inserted. */
    do
    {
        if (SDMMC_OSAEventWait(&(host->hostEvent), SDMMC_OSA_EVENT_CARD_INSERTED | SDMMC_OSA_EVENT_CARD_REMOVED,
                               timeout, &event) != kStatus_Success)
        {
            return kStatus_Fail;
        }
        else
        {
            if ((waitCardStatus == (uint32_t)kSD_Inserted) &&
                ((event & SDMMC_OSA_EVENT_CARD_INSERTED) == SDMMC_OSA_EVENT_CARD_INSERTED))
            {
                SDMMC_OSADelay(cd->cdDebounce_ms);
                if (SDMMCHOST_CardDetectStatus(host) == (uint32_t)kSD_Inserted)
                {
                    break;
                }
            }

            if (((event & SDMMC_OSA_EVENT_CARD_REMOVED) == SDMMC_OSA_EVENT_CARD_REMOVED) &&
                (waitCardStatus == (uint32_t)kSD_Removed))
            {
                break;
            }
        }
    } while (true);

    return kStatus_Success;
}

status_t SDMMCHOST_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase,
                                        const sdmmchost_detect_card_t *cd,
                                        bool waitCardStatus)
{
    assert(cd != NULL);

    while ((USDHC_GetInterruptStatusFlags(hostBase) & (uint32_t)kUSDHC_CardInsertionFlag) != (uint32_t)waitCardStatus)
    {
    }

    return kStatus_Success;
}

static void SDMMCHOST_TransferCompleteCallback(USDHC_Type *base,
                                               usdhc_handle_t *handle,
                                               status_t status,
                                               void *userData)
{
    uint32_t eventStatus = 0U;

    if (status == kStatus_USDHC_TransferDataFailed)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL;
    }
    else if (status == kStatus_USDHC_TransferDataComplete)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS;
    }
    else if (status == kStatus_USDHC_SendCommandFailed)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL;
    }
    else if (status == kStatus_USDHC_TransferDMAComplete)
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_DMA_COMPLETE;
    }
    else
    {
        eventStatus = SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS;
    }

    (void)SDMMC_OSAEventSet(&(((sdmmchost_t *)userData)->hostEvent), eventStatus);
}

#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
void SDMMCHOST_InstallCacheAlignBuffer(sdmmchost_t *host, void *cacheAlignBuffer, uint32_t cacheAlignBufferSize)
{
    assert(((uint32_t)cacheAlignBuffer & (SDMMC_DATA_BUFFER_ALIGN_CACHE - 1)) == 0U);
    assert(cacheAlignBufferSize >= SDMMC_DATA_BUFFER_ALIGN_CACHE * 2U);

    host->cacheAlignBuffer     = cacheAlignBuffer;
    host->cacheAlignBufferSize = cacheAlignBufferSize;
}
#endif

status_t SDMMCHOST_TransferFunction(sdmmchost_t *host, sdmmchost_transfer_t *content)
{
    status_t error = kStatus_Success;
    uint32_t event = 0U;
    usdhc_adma_config_t dmaConfig;

#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
    usdhc_scatter_gather_data_list_t sgDataList0;
    usdhc_scatter_gather_data_list_t sgDataList1;
    usdhc_scatter_gather_data_t scatterGatherData;
    uint32_t unAlignSize                     = 0U;
    usdhc_scatter_gather_transfer_t transfer = {.data = NULL, .command = content->command};
#endif

    if (content->data != NULL)
    {
        (void)memset(&dmaConfig, 0, sizeof(usdhc_adma_config_t));
        /* config adma */
        dmaConfig.dmaMode = kUSDHC_DmaModeAdma2;
#if !(defined(FSL_FEATURE_USDHC_HAS_NO_RW_BURST_LEN) && FSL_FEATURE_USDHC_HAS_NO_RW_BURST_LEN)
        dmaConfig.burstLen = kUSDHC_EnBurstLenForINCR;
#endif
        dmaConfig.admaTable      = host->dmaDesBuffer;
        dmaConfig.admaTableWords = host->dmaDesBufferWordsNum;

#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER

        if ((host->cacheAlignBuffer == NULL) || ((host->cacheAlignBufferSize == 0U)))
        {
            /* application should register cache line size align buffer for host driver maintain the unalign data
             * transfer */
            assert(false);
            return kStatus_InvalidArgument;
        }

        scatterGatherData.enableAutoCommand12 = content->data->enableAutoCommand12;
        scatterGatherData.enableAutoCommand23 = content->data->enableAutoCommand23;
        scatterGatherData.enableIgnoreError   = content->data->enableIgnoreError;
        scatterGatherData.dataType            = content->data->dataType;
        scatterGatherData.blockSize           = content->data->blockSize;

        transfer.data = &scatterGatherData;

        if (content->data->rxData != NULL)
        {
            scatterGatherData.sgData.dataAddr = content->data->rxData;
            scatterGatherData.dataDirection   = kUSDHC_TransferDirectionReceive;
        }
        else
        {
            scatterGatherData.sgData.dataAddr = (uint32_t *)content->data->txData;
            scatterGatherData.dataDirection   = kUSDHC_TransferDirectionSend;
        }
        scatterGatherData.sgData.dataSize = content->data->blockSize * content->data->blockCount;
        scatterGatherData.sgData.dataList = NULL;

        /*
         * If the receive transfer buffer address is not cache line size align, such as
         *---------------------------------------------------------------------
         *|  unalign head region|      align region     |   unaling tail region|
         *---------------------------------------------------------------------
         *
         * Then host driver will splict it into three scatter gather transfers,
         * 1. host->cacheAlignBuffer will be used as first scatter gather data address, the data size is the unalign
         *head region size.
         * 2. align region start address will be used as second scatter gather data address, the data size is the align
         *region size.
         * 3. (uint32_t *)((uint32_t)host->cacheAlignBuffer + SDMMC_DATA_BUFFER_ALIGN_CACHE)
         *    will be used as third scatter gather data address, the data size is the unaling tail region size.
         *
         * Once scatter gather transfer done,
         * 1. host driver will invalidate the cache for the data buffer in the scatter gather transfer list
         * 2. host driver will copy data from host->cacheAlignBuffer to unalign head region
         * 3. host driver will copy data from (uint32_t *)((uint32_t)host->cacheAlignBuffer +
         *SDMMC_DATA_BUFFER_ALIGN_CACHE) to unalign tail region
         *
         * At last, cache line unalign transfer done
         */
        if ((content->data->rxData != NULL) &&
            (((uint32_t)content->data->rxData % SDMMC_DATA_BUFFER_ALIGN_CACHE) != 0U))
        {
            unAlignSize          = ((uint32_t)content->data->rxData -
                           (((uint32_t)content->data->rxData) & (~(SDMMC_DATA_BUFFER_ALIGN_CACHE - 1))));
            sgDataList1.dataSize = unAlignSize;
            unAlignSize          = SDMMC_DATA_BUFFER_ALIGN_CACHE - unAlignSize;

            scatterGatherData.sgData.dataAddr = host->cacheAlignBuffer;
            scatterGatherData.sgData.dataSize = unAlignSize;
            scatterGatherData.sgData.dataList = &sgDataList0;

            sgDataList0.dataAddr =
                (void *)((((uint32_t)content->data->rxData) & (~(SDMMC_DATA_BUFFER_ALIGN_CACHE - 1))) +
                         SDMMC_DATA_BUFFER_ALIGN_CACHE);
            sgDataList0.dataSize = content->data->blockCount * content->data->blockSize - SDMMC_DATA_BUFFER_ALIGN_CACHE;
            sgDataList0.dataList = &sgDataList1;
            sgDataList1.dataAddr = (uint32_t *)((uint32_t)host->cacheAlignBuffer + SDMMC_DATA_BUFFER_ALIGN_CACHE);
            sgDataList1.dataList = NULL;
        }
#endif

#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
        if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
        {
            /* no matter read or write transfer, clean the cache line anyway to avoid data miss */
            DCACHE_CleanByRange(
                (uint32_t)(content->data->txData == NULL ? content->data->rxData : content->data->txData),
                (content->data->blockSize) * (content->data->blockCount));
        }
#endif
#endif
    }

    /* clear redundant transfer event flag */
    (void)SDMMC_OSAEventClear(&(host->hostEvent), SDMMCHOST_TRANSFER_CMD_EVENT);

#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
    error = USDHC_TransferScatterGatherADMANonBlocking(host->hostController.base, &host->handle, &dmaConfig, &transfer);
#else
    error = USDHC_TransferNonBlocking(host->hostController.base, &host->handle, &dmaConfig, content);
#endif

    if (error == kStatus_Success)
    {
        /* wait command event */
        if ((kStatus_Fail == SDMMC_OSAEventWait(&(host->hostEvent), SDMMCHOST_TRANSFER_CMD_EVENT,
                                                SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event)) ||
            ((event & SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL) != 0U))
        {
            error = kStatus_Fail;
        }
        else
        {
            if (content->data != NULL)
            {
                if ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS) == 0U)
                {
                    if (((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U) ||
                        (kStatus_Fail == SDMMC_OSAEventWait(&(host->hostEvent), SDMMCHOST_TRANSFER_DATA_EVENT,
                                                            SDMMCHOST_TRANSFER_COMPLETE_TIMEOUT, &event) ||
                         ((event & SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL) != 0U)))
                    {
                        error = kStatus_Fail;
                    }
                }
            }
        }
    }

    if (error != kStatus_Success)
    {
        /* host error recovery */
        SDMMCHOST_ErrorRecovery(host->hostController.base);
    }
    else
    {
        if ((content->data != NULL) && (content->data->rxData != NULL))
        {
#if SDMMCHOST_ENABLE_CACHE_LINE_ALIGN_TRANSFER
            if (((uint32_t)content->data->rxData % SDMMC_DATA_BUFFER_ALIGN_CACHE) != 0U)
            {
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
                if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
                {
                    DCACHE_InvalidateByRange((uint32_t)scatterGatherData.sgData.dataAddr,
                                             scatterGatherData.sgData.dataSize);

                    DCACHE_InvalidateByRange((uint32_t)sgDataList0.dataAddr, sgDataList0.dataSize);

                    DCACHE_InvalidateByRange((uint32_t)sgDataList1.dataAddr, sgDataList1.dataSize);
                }
#endif
#endif
                memcpy(content->data->rxData, scatterGatherData.sgData.dataAddr, scatterGatherData.sgData.dataSize);
                memcpy((void *)((uint32_t)content->data->rxData + content->data->blockCount * content->data->blockSize -
                                sgDataList1.dataSize),
                       sgDataList1.dataAddr, sgDataList1.dataSize);
            }
            else
#endif
            {
#if ((defined __DCACHE_PRESENT) && __DCACHE_PRESENT) || (defined FSL_FEATURE_HAS_L1CACHE && FSL_FEATURE_HAS_L1CACHE)
#if !(defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL)
                /* invalidate the cache for read */
                if (host->enableCacheControl == kSDMMCHOST_CacheControlRWBuffer)
                {
                    DCACHE_InvalidateByRange((uint32_t)content->data->rxData,
                                             (content->data->blockSize) * (content->data->blockCount));
                }
#endif
#endif
            }
        }
    }

    return error;
}

static void SDMMCHOST_ErrorRecovery(USDHC_Type *base)
{
    uint32_t status = 0U;
    /* get host present status */
    status = USDHC_GetPresentStatusFlags(base);
    /* check command inhibit status flag */
    if ((status & (uint32_t)kUSDHC_CommandInhibitFlag) != 0U)
    {
        /* reset command line */
        (void)USDHC_Reset(base, kUSDHC_ResetCommand, 100U);
    }
    /* check data inhibit status flag */
    if ((status & (uint32_t)kUSDHC_DataInhibitFlag) != 0U)
    {
        /* reset data line */
        (void)USDHC_Reset(base, kUSDHC_ResetData, 100U);
    }
}

void SDMMCHOST_SetCardPower(sdmmchost_t *host, bool enable)
{
    /* host not support */
}

void SDMMCHOST_SetCardBusWidth(sdmmchost_t *host, uint32_t dataBusWidth)
{
    assert((dataBusWidth != (uint32_t)kUSDHC_DataBusWidth8Bit) ||
           ((dataBusWidth == (uint32_t)kUSDHC_DataBusWidth8Bit) &&
            ((uint32_t)SDMMCHOST_INSTANCE_SUPPORT_8_BIT_WIDTH(host) != 0U)));

    USDHC_SetDataBusWidth(host->hostController.base, dataBusWidth == (uint32_t)kSDMMC_BusWdith1Bit ?
                                                         kUSDHC_DataBusWidth1Bit :
                                                         dataBusWidth == (uint32_t)kSDMMC_BusWdith4Bit ?
                                                         kUSDHC_DataBusWidth4Bit :
                                                         kUSDHC_DataBusWidth8Bit);
}

void SDMMCHOST_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    if (pwr != NULL)
    {
        pwr->powerOff();
        SDMMC_OSADelay(pwr->powerOffDelay_ms);
    }
}

void SDMMCHOST_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    /* use user define the power on function  */
    if (pwr != NULL)
    {
        pwr->powerOn();
        SDMMC_OSADelay(pwr->powerOnDelay_ms);
    }
    else
    {
        /* Delay several milliseconds to make card stable. */
        SDMMC_OSADelay(1000U);
    }
}

status_t SDMMCHOST_Init(sdmmchost_t *host)
{
    assert(host != NULL);

    usdhc_transfer_callback_t usdhcCallback = {0};
    usdhc_host_t *usdhcHost                 = &(host->hostController);

    /* sdmmc osa init */
    SDMMC_OSAInit();

    /* Initializes USDHC. */
    usdhcHost->config.endianMode          = kUSDHC_EndianModeLittle;
    usdhcHost->config.dataTimeout         = 0xFU;
    usdhcHost->config.readWatermarkLevel  = 0x80U;
    usdhcHost->config.writeWatermarkLevel = 0x80U;
    USDHC_Init(usdhcHost->base, &(usdhcHost->config));

    /* Create handle for SDHC driver */
    usdhcCallback.TransferComplete = SDMMCHOST_TransferCompleteCallback;
    USDHC_TransferCreateHandle(usdhcHost->base, &host->handle, &usdhcCallback, host);

    /* Create transfer event. */
    if (kStatus_Success != SDMMC_OSAEventCreate(&(host->hostEvent)))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

void SDMMCHOST_Reset(sdmmchost_t *host)
{
    USDHC_Type *base = host->hostController.base;

    /* voltage switch to normal but not 1.8V */
    UDSHC_SelectVoltage(base, false);
    /* Disable DDR mode */
    USDHC_EnableDDRMode(base, false, 0U);
    /* disable tuning */
#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE)
    USDHC_EnableStandardTuning(base, 0, 0, false);
    USDHC_EnableAutoTuning(host->hostController.base, false);
#endif

#if FSL_FEATURE_USDHC_HAS_HS400_MODE
    /* Disable HS400 mode */
    USDHC_EnableHS400Mode(base, false);
    /* Disable DLL */
    USDHC_EnableStrobeDLL(base, false);
#endif
    /* reset data/command/tuning circuit */
    (void)USDHC_Reset(base, kUSDHC_ResetAll, 100U);

    USDHC_DisableInterruptSignal(base, kUSDHC_AllInterruptFlags);
}

void SDMMCHOST_Deinit(sdmmchost_t *host)
{
    usdhc_host_t *sdhcHost = &host->hostController;
    SDMMCHOST_Reset(host);
    USDHC_Deinit(sdhcHost->base);
    (void)SDMMC_OSAEventDestroy(&(host->hostEvent));
}

void SDMMCHOST_SwitchToVoltage(sdmmchost_t *host, uint32_t voltage)
{
    if (voltage == (uint32_t)kSDMMC_OperationVoltage180V)
    {
        UDSHC_SelectVoltage(host->hostController.base, true);
    }
    else
    {
        UDSHC_SelectVoltage(host->hostController.base, false);
    }
}

#if SDMMCHOST_SUPPORT_SDR104 || SDMMCHOST_SUPPORT_SDR50 || SDMMCHOST_SUPPORT_HS200 || SDMMCHOST_SUPPORT_HS400
static status_t SDMMCHOST_ExecuteStdTuning(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t blockSize)
{
    sdmmchost_transfer_t content = {0U};
    sdmmchost_cmd_t command      = {0U};
    sdmmchost_data_t data        = {0U};
    bool tuningError             = true;
    status_t error               = kStatus_Success;

    command.index        = tuningCmd;
    command.argument     = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize  = blockSize;
    data.blockCount = 1U;
    data.rxData     = revBuf;
    data.dataType   = kUSDHC_TransferDataTuning;

    content.command = &command;
    content.data    = &data;

    (void)USDHC_Reset(host->hostController.base, kUSDHC_ResetTuning, 100U);
    /* disable standard tuning */
    USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START, SDMMCHOST_TUINIG_STEP,
                               false);
    /*
     * Tuning fail found on some SOCS caused by the difference of delay cell, so we need to i
     * ncrease the tuning counter to cover the adjustable tuninig window
     */
    USDHC_SetStandardTuningCounter(host->hostController.base, SDMMCHOST_STANDARD_TUNING_COUNTER);
    /* enable the standard tuning */
    USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START, SDMMCHOST_TUINIG_STEP, true);

    while (true)
    {
        error = SDMMCHOST_TransferFunction(host, &content);
        /* send tuning block */
        if (kStatus_Success != error)
        {
            return kStatus_SDMMC_TransferFailed;
        }
        SDMMC_OSADelay(1U);

        /*wait excute tuning bit clear*/
        if ((USDHC_GetExecuteStdTuningStatus(host->hostController.base) != 0U))
        {
            continue;
        }

        /* if tuning error , re-tuning again */
        if ((USDHC_CheckTuningError(host->hostController.base) != 0U) && tuningError)
        {
            tuningError = false;
            /* enable the standard tuning */
            USDHC_EnableStandardTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START,
                                       SDMMCHOST_TUINIG_STEP, true);
            (void)USDHC_AdjustDelayForManualTuning(host->hostController.base, SDMMCHOST_STANDARD_TUNING_START);
        }
        else
        {
            break;
        }
    }

    /* check tuning result*/
    if (USDHC_CheckStdTuningResult(host->hostController.base) == 0U)
    {
        return kStatus_SDMMC_TuningFail;
    }

    USDHC_EnableAutoTuning(host->hostController.base, true);

    return kStatus_Success;
}

static status_t SDMMCHOST_ReceiveTuningBlock(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t size)
{
    assert(revBuf != NULL);

    usdhc_command_t command   = {0U};
    uint32_t interruptStatus  = 0U;
    uint32_t transferredWords = 0U;
    uint32_t wordSize         = size / sizeof(uint32_t);
    USDHC_Type *base          = host->hostController.base;

    command.index        = tuningCmd;
    command.argument     = 0U;
    command.responseType = kCARD_ResponseTypeR1;
    command.flags        = kUSDHC_DataPresentFlag;

    /* disable DMA first */
    USDHC_EnableInternalDMA(base, false);
    /* set data configurations */
    USDHC_SetDataConfig(base, kUSDHC_TransferDirectionReceive, 1U, size);
    /* enable status */
    USDHC_EnableInterruptStatus(base, (uint32_t)kUSDHC_CommandCompleteFlag | (uint32_t)kUSDHC_CommandErrorFlag |
                                          (uint32_t)kUSDHC_BufferReadReadyFlag);
    /* polling cmd done */
    USDHC_SendCommand(base, &command);
    while (0U == (interruptStatus & ((uint32_t)kUSDHC_CommandCompleteFlag | (uint32_t)kUSDHC_CommandErrorFlag)))
    {
        interruptStatus = USDHC_GetInterruptStatusFlags(base);
    }
    /* clear interrupt status */
    USDHC_ClearInterruptStatusFlags(base, interruptStatus);
    /* check command inhibit status flag */
    if ((USDHC_GetPresentStatusFlags(base) & (uint32_t)kUSDHC_CommandInhibitFlag) != 0U)
    {
        /* reset command line */
        (void)USDHC_Reset(base, kUSDHC_ResetCommand, 100U);
    }

    while (0U == (interruptStatus & (uint32_t)kUSDHC_BufferReadReadyFlag))
    {
        interruptStatus = USDHC_GetInterruptStatusFlags(base);
    }

    while (transferredWords < wordSize)
    {
        revBuf[transferredWords++] = USDHC_ReadData(base);
    }

    USDHC_ClearInterruptStatusFlags(
        base, interruptStatus | (uint32_t)kUSDHC_DataCompleteFlag | (uint32_t)kUSDHC_DataErrorFlag);

    return kStatus_Success;
}

static status_t SDMMC_CheckTuningResult(uint32_t *buffer, uint32_t size)
{
    uint32_t i              = 0U;
    const uint32_t *pattern = SDMMC_TuningBlockPattern4Bit;

    if (size == 128U)
    {
        pattern = SDMMC_TuningBlockPattern8Bit;
    }

    for (i = 0U; i < size / sizeof(uint32_t); i++)
    {
        if (pattern[i] != SWAP_WORD_BYTE_SEQUENCE(buffer[i]))
        {
#if defined SDMMC_ENABLE_LOG_PRINT
            SDMMC_LOG("tuning unmatch target: %x, read :%x\r\n", pattern[i], SWAP_WORD_BYTE_SEQUENCE(buffer[i]));
#endif
            return kStatus_SDMMC_TuningFail;
        }
    }

    return kStatus_Success;
}

static status_t SDMMCHOST_ExecuteManualTuning(sdmmchost_t *host,
                                              uint32_t tuningCmd,
                                              uint32_t *revBuf,
                                              uint32_t blockSize)
{
    uint32_t *buffer             = revBuf;
    uint32_t tuningDelayCell     = 0U;
    uint32_t validDelayCellStart = 0U;
    bool validWindowFound        = false;
    uint32_t validWindowCounter  = 0U;
    status_t ret                 = kStatus_Success;

    (void)USDHC_Reset(host->hostController.base, kUSDHC_ResetTuning, 100U);
    USDHC_EnableManualTuning(host->hostController.base, true);

    while (true)
    {
        (void)USDHC_AdjustDelayForManualTuning(host->hostController.base, tuningDelayCell);

        (void)SDMMCHOST_ReceiveTuningBlock(host, tuningCmd, buffer, blockSize);

        if (kStatus_Success == SDMMC_CheckTuningResult(buffer, blockSize))
        {
            if (validWindowFound == false)
            {
                validDelayCellStart = tuningDelayCell;
                validWindowFound    = true;
            }

            if ((validWindowCounter + validDelayCellStart) != tuningDelayCell)
            {
                validWindowFound   = false;
                validWindowCounter = 0U;
            }

            validWindowCounter++;

#if defined SDMMC_ENABLE_LOG_PRINT
            SDMMC_LOG("tuning pass point: %d\r\n", tuningDelayCell);
#endif
        }
        else
        {
            if ((validWindowFound) && (validWindowCounter > 2U))
            {
                break;
            }
        }

        if (++tuningDelayCell >= SDMMCHOST_MAX_TUNING_DELAY_CELL)
        {
            break;
        }

        (void)memset(buffer, 0, blockSize);

        SDMMC_OSADelay(2U);
    }
    (void)memset(buffer, 0, blockSize);

    SDMMC_OSADelay(2U);

    /* select middle position of the window */
    (void)USDHC_AdjustDelayForManualTuning(host->hostController.base, validDelayCellStart + validWindowCounter / 2U);
    /* send tuning block with the average delay cell */
    (void)SDMMCHOST_ReceiveTuningBlock(host, tuningCmd, buffer, blockSize);
    ret = SDMMC_CheckTuningResult(buffer, blockSize);
    /* abort tuning */
    USDHC_EnableManualTuning(host->hostController.base, false);

    /* enable auto tuning */
    USDHC_EnableAutoTuning(host->hostController.base, true);

    return ret;
}

status_t SDMMCHOST_ExecuteTuning(sdmmchost_t *host, uint32_t tuningCmd, uint32_t *revBuf, uint32_t blockSize)
{
    if (host->tuningType == (uint32_t)kSDMMCHOST_StandardTuning)
    {
        return SDMMCHOST_ExecuteStdTuning(host, tuningCmd, revBuf, blockSize);
    }

    return SDMMCHOST_ExecuteManualTuning(host, tuningCmd, revBuf, blockSize);
}
#endif

status_t SDMMCHOST_StartBoot(sdmmchost_t *host,
                             sdmmchost_boot_config_t *hostConfig,
                             sdmmchost_cmd_t *cmd,
                             uint8_t *buffer)
{
    sdmmchost_transfer_t content = {0};
    sdmmchost_data_t data        = {0};
    status_t error               = kStatus_Success;

    USDHC_SetMmcBootConfig(host->hostController.base, hostConfig);

    data.blockSize  = hostConfig->blockSize;
    data.blockCount = hostConfig->blockCount;
    data.rxData     = (uint32_t *)(uint32_t)buffer;
    data.dataType   = kUSDHC_TransferDataBoot;

    content.data    = &data;
    content.command = cmd;

    error = SDMMCHOST_TransferFunction(host, &content);
    /* should check tuning error during every transfer*/
    if (kStatus_Success != error)
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t SDMMCHOST_ReadBootData(sdmmchost_t *host, sdmmchost_boot_config_t *hostConfig, uint8_t *buffer)
{
    sdmmchost_cmd_t command      = {0};
    sdmmchost_transfer_t content = {0};
    sdmmchost_data_t data        = {0};
    status_t error               = kStatus_Success;

    USDHC_SetMmcBootConfig(host->hostController.base, hostConfig);
    USDHC_EnableMmcBoot(host->hostController.base, true);

    data.blockSize  = hostConfig->blockSize;
    data.blockCount = hostConfig->blockCount;
    data.rxData     = (uint32_t *)(uint32_t)buffer;
    data.dataType   = kUSDHC_TransferDataBootcontinous;
    /* no command should be send out  */
    command.type = kCARD_CommandTypeEmpty;

    content.data    = &data;
    content.command = &command;

    error = SDMMCHOST_TransferFunction(host, &content);
    if (kStatus_Success != error)
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}
