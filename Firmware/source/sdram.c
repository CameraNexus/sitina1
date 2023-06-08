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
#include "fsl_clock.h"
#include "fsl_semc.h"
#include "fsl_iomuxc.h"
#include "sdram.h"

static void sdram_init_pins(void);

void sdram_init(void) {
    semc_config_t config;
    semc_sdram_config_t sdramconfig;
    uint32_t clockFrq = CLOCK_GetRootClockFreq(kCLOCK_Root_Semc);

    sdram_init_pins();

    // Initializes the configure structure to zero.
    memset(&config, 0, sizeof(semc_config_t));
    memset(&sdramconfig, 0, sizeof(semc_sdram_config_t));

    // Initialize SEMC
    SEMC_GetDefaultConfig(&config);
    config.dqsMode = kSEMC_Loopbackdqspad;
    SEMC_Init(SEMC, &config);

    // Configure SDRAM.
    sdramconfig.csxPinMux           = kSEMC_MUXCSX0;
    sdramconfig.address             = 0x80000000;
    sdramconfig.memsize_kbytes      = 64 * 1024; // 32MB
    sdramconfig.portSize            = kSEMC_PortSize32Bit;
    sdramconfig.burstLen            = kSEMC_Sdram_BurstLen1;
    sdramconfig.columnAddrBitNum    = kSEMC_SdramColunm_9bit;
    sdramconfig.casLatency          = kSEMC_LatencyThree;
    sdramconfig.tPrecharge2Act_Ns   = 18; // Trp
    sdramconfig.tAct2ReadWrite_Ns   = 18; // Trcd
    sdramconfig.tRefreshRecovery_Ns = 67; // Use the maximum of the (Trfc , Txsr)
    sdramconfig.tWriteRecovery_Ns   = 12;
    // The minimum cycle of SDRAM CLK off state.
    // CKE is off in self refresh at a minimum period tRAS.
    sdramconfig.tCkeOff_Ns             = 42;
    sdramconfig.tAct2Prechage_Ns       = 42; // Tras
    sdramconfig.tSelfRefRecovery_Ns    = 67;
    sdramconfig.tRefresh2Refresh_Ns    = 60;
    sdramconfig.tAct2Act_Ns            = 60;
    sdramconfig.tPrescalePeriod_Ns     = 160 * (1000000000 / clockFrq);
    sdramconfig.refreshPeriod_nsPerRow = 64 * 1000000 / 8192;
    sdramconfig.refreshUrgThreshold    = sdramconfig.refreshPeriod_nsPerRow;
    sdramconfig.refreshBurstLen        = 1;
    sdramconfig.delayChain             = 2;

    SEMC_ConfigureSDRAM(SEMC, kSEMC_SDRAM_CS0, &sdramconfig, clockFrq);

    return;
}

static void sdram_init_pins(void) {
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00,      /* GPIO_EMC_B1_00 is configured as SEMC_DATA00 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01,      /* GPIO_EMC_B1_01 is configured as SEMC_DATA01 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02,      /* GPIO_EMC_B1_02 is configured as SEMC_DATA02 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03,      /* GPIO_EMC_B1_03 is configured as SEMC_DATA03 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04,      /* GPIO_EMC_B1_04 is configured as SEMC_DATA04 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05,      /* GPIO_EMC_B1_05 is configured as SEMC_DATA05 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06,      /* GPIO_EMC_B1_06 is configured as SEMC_DATA06 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07,      /* GPIO_EMC_B1_07 is configured as SEMC_DATA07 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_08_SEMC_DM00,        /* GPIO_EMC_B1_08 is configured as SEMC_DM00 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00,      /* GPIO_EMC_B1_09 is configured as SEMC_ADDR00 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01,      /* GPIO_EMC_B1_10 is configured as SEMC_ADDR01 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02,      /* GPIO_EMC_B1_11 is configured as SEMC_ADDR02 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03,      /* GPIO_EMC_B1_12 is configured as SEMC_ADDR03 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04,      /* GPIO_EMC_B1_13 is configured as SEMC_ADDR04 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05,      /* GPIO_EMC_B1_14 is configured as SEMC_ADDR05 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06,      /* GPIO_EMC_B1_15 is configured as SEMC_ADDR06 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07,      /* GPIO_EMC_B1_16 is configured as SEMC_ADDR07 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08,      /* GPIO_EMC_B1_17 is configured as SEMC_ADDR08 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09,      /* GPIO_EMC_B1_18 is configured as SEMC_ADDR09 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11,      /* GPIO_EMC_B1_19 is configured as SEMC_ADDR11 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12,      /* GPIO_EMC_B1_20 is configured as SEMC_ADDR12 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_21_SEMC_BA0,         /* GPIO_EMC_B1_21 is configured as SEMC_BA0 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_22_SEMC_BA1,         /* GPIO_EMC_B1_22 is configured as SEMC_BA1 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10,      /* GPIO_EMC_B1_23 is configured as SEMC_ADDR10 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_24_SEMC_CAS,         /* GPIO_EMC_B1_24 is configured as SEMC_CAS */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_25_SEMC_RAS,         /* GPIO_EMC_B1_25 is configured as SEMC_RAS */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_26_SEMC_CLK,         /* GPIO_EMC_B1_26 is configured as SEMC_CLK */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_27_SEMC_CKE,         /* GPIO_EMC_B1_27 is configured as SEMC_CKE */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_28_SEMC_WE,          /* GPIO_EMC_B1_28 is configured as SEMC_WE */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_29_SEMC_CS0,         /* GPIO_EMC_B1_29 is configured as SEMC_CS0 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08,      /* GPIO_EMC_B1_30 is configured as SEMC_DATA08 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09,      /* GPIO_EMC_B1_31 is configured as SEMC_DATA09 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10,      /* GPIO_EMC_B1_32 is configured as SEMC_DATA10 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11,      /* GPIO_EMC_B1_33 is configured as SEMC_DATA11 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12,      /* GPIO_EMC_B1_34 is configured as SEMC_DATA12 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13,      /* GPIO_EMC_B1_35 is configured as SEMC_DATA13 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14,      /* GPIO_EMC_B1_36 is configured as SEMC_DATA14 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15,      /* GPIO_EMC_B1_37 is configured as SEMC_DATA15 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_38_SEMC_DM01,        /* GPIO_EMC_B1_38 is configured as SEMC_DM01 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_39_SEMC_DQS,         /* GPIO_EMC_B1_39 is configured as SEMC_DQS */
	      1U);                                    /* Software Input On Field: Force input path of pad GPIO_EMC_B1_39 */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_40_SEMC_RDY,         /* GPIO_EMC_B1_40 is configured as SEMC_RDY */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00,       /* GPIO_EMC_B1_41 is configured as SEMC_CSX00 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16,      /* GPIO_EMC_B2_00 is configured as SEMC_DATA16 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17,      /* GPIO_EMC_B2_01 is configured as SEMC_DATA17 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18,      /* GPIO_EMC_B2_02 is configured as SEMC_DATA18 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19,      /* GPIO_EMC_B2_03 is configured as SEMC_DATA19 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20,      /* GPIO_EMC_B2_04 is configured as SEMC_DATA20 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21,      /* GPIO_EMC_B2_05 is configured as SEMC_DATA21 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22,      /* GPIO_EMC_B2_06 is configured as SEMC_DATA22 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23,      /* GPIO_EMC_B2_07 is configured as SEMC_DATA23 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_08_SEMC_DM02,        /* GPIO_EMC_B2_08 is configured as SEMC_DM02 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24,      /* GPIO_EMC_B2_09 is configured as SEMC_DATA24 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25,      /* GPIO_EMC_B2_10 is configured as SEMC_DATA25 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26,      /* GPIO_EMC_B2_11 is configured as SEMC_DATA26 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27,      /* GPIO_EMC_B2_12 is configured as SEMC_DATA27 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28,      /* GPIO_EMC_B2_13 is configured as SEMC_DATA28 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29,      /* GPIO_EMC_B2_14 is configured as SEMC_DATA29 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30,      /* GPIO_EMC_B2_15 is configured as SEMC_DATA30 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31,      /* GPIO_EMC_B2_16 is configured as SEMC_DATA31 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_17_SEMC_DM03,        /* GPIO_EMC_B2_17 is configured as SEMC_DM03 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4,        /* GPIO_EMC_B2_18 is configured as SEMC_DQS4 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_19_SEMC_CLKX00,      /* GPIO_EMC_B2_19 is configured as SEMC_CLKX00 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	  IOMUXC_SetPinMux(
	      IOMUXC_GPIO_EMC_B2_20_SEMC_CLKX01,      /* GPIO_EMC_B2_20 is configured as SEMC_CLKX01 */
	      0U);                                    /* Software Input On Field: Input Path is determined by functionality */

	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_00_SEMC_DATA00,      /* GPIO_EMC_B1_00 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_01_SEMC_DATA01,      /* GPIO_EMC_B1_01 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_02_SEMC_DATA02,      /* GPIO_EMC_B1_02 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_03_SEMC_DATA03,      /* GPIO_EMC_B1_03 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_04_SEMC_DATA04,      /* GPIO_EMC_B1_04 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_05_SEMC_DATA05,      /* GPIO_EMC_B1_05 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_06_SEMC_DATA06,      /* GPIO_EMC_B1_06 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_07_SEMC_DATA07,      /* GPIO_EMC_B1_07 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_08_SEMC_DM00,        /* GPIO_EMC_B1_08 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_09_SEMC_ADDR00,      /* GPIO_EMC_B1_09 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_10_SEMC_ADDR01,      /* GPIO_EMC_B1_10 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_11_SEMC_ADDR02,      /* GPIO_EMC_B1_11 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_12_SEMC_ADDR03,      /* GPIO_EMC_B1_12 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_13_SEMC_ADDR04,      /* GPIO_EMC_B1_13 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_14_SEMC_ADDR05,      /* GPIO_EMC_B1_14 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_15_SEMC_ADDR06,      /* GPIO_EMC_B1_15 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_16_SEMC_ADDR07,      /* GPIO_EMC_B1_16 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_17_SEMC_ADDR08,      /* GPIO_EMC_B1_17 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_18_SEMC_ADDR09,      /* GPIO_EMC_B1_18 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_19_SEMC_ADDR11,      /* GPIO_EMC_B1_19 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_20_SEMC_ADDR12,      /* GPIO_EMC_B1_20 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_21_SEMC_BA0,         /* GPIO_EMC_B1_21 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_22_SEMC_BA1,         /* GPIO_EMC_B1_22 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_23_SEMC_ADDR10,      /* GPIO_EMC_B1_23 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_24_SEMC_CAS,         /* GPIO_EMC_B1_24 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_25_SEMC_RAS,         /* GPIO_EMC_B1_25 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_26_SEMC_CLK,         /* GPIO_EMC_B1_26 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_27_SEMC_CKE,         /* GPIO_EMC_B1_27 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_28_SEMC_WE,          /* GPIO_EMC_B1_28 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_29_SEMC_CS0,         /* GPIO_EMC_B1_29 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_30_SEMC_DATA08,      /* GPIO_EMC_B1_30 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_31_SEMC_DATA09,      /* GPIO_EMC_B1_31 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_32_SEMC_DATA10,      /* GPIO_EMC_B1_32 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_33_SEMC_DATA11,      /* GPIO_EMC_B1_33 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_34_SEMC_DATA12,      /* GPIO_EMC_B1_34 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_35_SEMC_DATA13,      /* GPIO_EMC_B1_35 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_36_SEMC_DATA14,      /* GPIO_EMC_B1_36 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_37_SEMC_DATA15,      /* GPIO_EMC_B1_37 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_38_SEMC_DM01,        /* GPIO_EMC_B1_38 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_39_SEMC_DQS,         /* GPIO_EMC_B1_39 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_40_SEMC_RDY,         /* GPIO_EMC_B1_40 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B1_41_SEMC_CSX00,       /* GPIO_EMC_B1_41 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_00_SEMC_DATA16,      /* GPIO_EMC_B2_00 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_01_SEMC_DATA17,      /* GPIO_EMC_B2_01 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_02_SEMC_DATA18,      /* GPIO_EMC_B2_02 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_03_SEMC_DATA19,      /* GPIO_EMC_B2_03 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_04_SEMC_DATA20,      /* GPIO_EMC_B2_04 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_05_SEMC_DATA21,      /* GPIO_EMC_B2_05 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_06_SEMC_DATA22,      /* GPIO_EMC_B2_06 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_07_SEMC_DATA23,      /* GPIO_EMC_B2_07 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_08_SEMC_DM02,        /* GPIO_EMC_B2_08 PAD functional properties : */
	      0x04U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pullup resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_09_SEMC_DATA24,      /* GPIO_EMC_B2_09 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_10_SEMC_DATA25,      /* GPIO_EMC_B2_10 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_11_SEMC_DATA26,      /* GPIO_EMC_B2_11 PAD functional properties : */
	      0x04U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pullup resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_12_SEMC_DATA27,      /* GPIO_EMC_B2_12 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_13_SEMC_DATA28,      /* GPIO_EMC_B2_13 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_14_SEMC_DATA29,      /* GPIO_EMC_B2_14 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_15_SEMC_DATA30,      /* GPIO_EMC_B2_15 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_16_SEMC_DATA31,      /* GPIO_EMC_B2_16 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_17_SEMC_DM03,        /* GPIO_EMC_B2_17 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_18_SEMC_DQS4,        /* GPIO_EMC_B2_18 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_19_SEMC_CLKX00,      /* GPIO_EMC_B2_19 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
	  IOMUXC_SetPinConfig(
	      IOMUXC_GPIO_EMC_B2_20_SEMC_CLKX01,      /* GPIO_EMC_B2_20 PAD functional properties : */
	      0x08U);                                 /* PDRV Field: high drive strength
	                                                 Pull Down Pull Up Field: Internal pulldown resistor enabled
	                                                 Open Drain Field: Disabled
	                                                 Domain write protection: Both cores are allowed
	                                                 Domain write protection lock: Neither of DWP bits is locked */
}
