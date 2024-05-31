#ifndef XPARAMETERS_H   /* prevent circular inclusions */
#define XPARAMETERS_H   /* by using protection macros */

/* Definition for CPU ID */
#define XPAR_CPU_ID 0U

/* Definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ 666666687


/******************************************************************/

/* Canonical definitions for peripheral PS7_CORTEXA9_0 */
#define XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ 666666687


/******************************************************************/


/* Definitions for interface APB_M_0 */
#define XPAR_APB_M_0_BASEADDR 0x40000000
#define XPAR_APB_M_0_HIGHADDR 0x40FFFFFF

#include "xparameters_ps.h"

#define STDIN_BASEADDRESS 0xF8800000
#define STDOUT_BASEADDRESS 0xF8800000

/******************************************************************/

/* Platform specific definitions */
#define PLATFORM_ZYNQ
 
/* Definitions for sleep timer configuration */
#define XSLEEP_TIMER_IS_DEFAULT_TIMER
 
 
/******************************************************************/

/* Definitions for peripheral PS7_DDR_0 */
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR 0x00100000
#define XPAR_PS7_DDR_0_S_AXI_HIGHADDR 0x1FFFFFFF


/******************************************************************/

/* Definitions for driver DEVCFG */
#define XPAR_XDCFG_NUM_INSTANCES 1U

/* Definitions for peripheral PS7_DEV_CFG_0 */
#define XPAR_PS7_DEV_CFG_0_DEVICE_ID 0U
#define XPAR_PS7_DEV_CFG_0_BASEADDR 0xF8007000U
#define XPAR_PS7_DEV_CFG_0_HIGHADDR 0xF80070FFU


/******************************************************************/

/* Canonical definitions for peripheral PS7_DEV_CFG_0 */
#define XPAR_XDCFG_0_DEVICE_ID XPAR_PS7_DEV_CFG_0_DEVICE_ID
#define XPAR_XDCFG_0_BASEADDR 0xF8007000U
#define XPAR_XDCFG_0_HIGHADDR 0xF80070FFU


/******************************************************************/

/* Definitions for driver DMAPS */
#define XPAR_XDMAPS_NUM_INSTANCES 2

/* Definitions for peripheral PS7_DMA_NS */
#define XPAR_PS7_DMA_NS_DEVICE_ID 0
#define XPAR_PS7_DMA_NS_BASEADDR 0xF8004000
#define XPAR_PS7_DMA_NS_HIGHADDR 0xF8004FFF


/* Definitions for peripheral PS7_DMA_S */
#define XPAR_PS7_DMA_S_DEVICE_ID 1
#define XPAR_PS7_DMA_S_BASEADDR 0xF8003000
#define XPAR_PS7_DMA_S_HIGHADDR 0xF8003FFF


/******************************************************************/

/* Canonical definitions for peripheral PS7_DMA_NS */
#define XPAR_XDMAPS_0_DEVICE_ID XPAR_PS7_DMA_NS_DEVICE_ID
#define XPAR_XDMAPS_0_BASEADDR 0xF8004000
#define XPAR_XDMAPS_0_HIGHADDR 0xF8004FFF

/* Canonical definitions for peripheral PS7_DMA_S */
#define XPAR_XDMAPS_1_DEVICE_ID XPAR_PS7_DMA_S_DEVICE_ID
#define XPAR_XDMAPS_1_BASEADDR 0xF8003000
#define XPAR_XDMAPS_1_HIGHADDR 0xF8003FFF


/******************************************************************/

/* Definitions for driver DSI */
#define XPAR_XDSI_NUM_INSTANCES 1

/* Definitions for peripheral MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0 */
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DEVICE_ID 0
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_BASEADDR 0x00000000
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_HIGHADDR 0x0000FFFF
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DSI_LANES 2
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DSI_DATATYPE 0x3E
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DSI_BYTE_FIFO 2048
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DSI_CRC_GEN 1
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DSI_PIXELS 1


/******************************************************************/

/* Canonical definitions for peripheral MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0 */
#define XPAR_DSI_0_DEVICE_ID XPAR_MIPI_DSI_TX_SUBSYSTEM_0_MIPI_DSI_TX_CTRL_0_DEVICE_ID
#define XPAR_DSI_0_BASEADDR 0x00000000
#define XPAR_DSI_0_HIGHADDR 0x0000FFFF
#define XPAR_DSI_0_DSI_LANES 2
#define XPAR_DSI_0_DSI_DATATYPE 0x3E
#define XPAR_DSI_0_DSI_BYTE_FIFO 2048
#define XPAR_DSI_0_DSI_CRC_GEN 1
#define XPAR_DSI_0_DSI_PIXELS 1


/******************************************************************/

/* Definitions for driver DSITXSS */
#define XPAR_XDSITXSS_NUM_INSTANCES 1

/* Definitions for peripheral MIPI_DSI_TX_SUBSYSTEM_0 */
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DEVICE_ID 0
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_BASEADDR 0x43000000
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_HIGHADDR 0x43FFFFFF
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DSI_LANES 2
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DSI_DATATYPE 0x3E
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DSI_BYTE_FIFO 2048
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DSI_CRC_GEN 1
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DSI_PIXELS 1
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DPHY_LINERATE 250
#define XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DPHY_EN_REG_IF 0


/******************************************************************/

/* Canonical definitions for peripheral MIPI_DSI_TX_SUBSYSTEM_0 */
#define XPAR_DSITXSS_0_DEVICE_ID XPAR_MIPI_DSI_TX_SUBSYSTEM_0_DEVICE_ID
#define XPAR_DSITXSS_0_BASEADDR 0x43000000
#define XPAR_DSITXSS_0_HIGHADDR 0x43FFFFFF
#define XPAR_DSITXSS_0_DSI_LANES 2
#define XPAR_DSITXSS_0_DSI_DATATYPE 0x3E
#define XPAR_DSITXSS_0_DSI_BYTE_FIFO 2048
#define XPAR_DSITXSS_0_DSI_CRC_GEN 1
#define XPAR_DSITXSS_0_DSI_PIXELS 1
#define XPAR_DSITXSS_0_DPHY_LINERATE 250
#define XPAR_DSITXSS_0_DPHY_EN_REG_IF 0


/******************************************************************/


/* Peripheral Definitions for peripheral PS7_AFI_0 */
#define XPAR_PS7_AFI_0_S_AXI_BASEADDR 0xF8008000
#define XPAR_PS7_AFI_0_S_AXI_HIGHADDR 0xF8008FFF


/* Peripheral Definitions for peripheral PS7_AFI_1 */
#define XPAR_PS7_AFI_1_S_AXI_BASEADDR 0xF8009000
#define XPAR_PS7_AFI_1_S_AXI_HIGHADDR 0xF8009FFF


/* Peripheral Definitions for peripheral PS7_AFI_2 */
#define XPAR_PS7_AFI_2_S_AXI_BASEADDR 0xF800A000
#define XPAR_PS7_AFI_2_S_AXI_HIGHADDR 0xF800AFFF


/* Peripheral Definitions for peripheral PS7_AFI_3 */
#define XPAR_PS7_AFI_3_S_AXI_BASEADDR 0xF800B000
#define XPAR_PS7_AFI_3_S_AXI_HIGHADDR 0xF800BFFF


/* Peripheral Definitions for peripheral PS7_DDRC_0 */
#define XPAR_PS7_DDRC_0_S_AXI_BASEADDR 0xF8006000
#define XPAR_PS7_DDRC_0_S_AXI_HIGHADDR 0xF8006FFF


/* Peripheral Definitions for peripheral PS7_GLOBALTIMER_0 */
#define XPAR_PS7_GLOBALTIMER_0_S_AXI_BASEADDR 0xF8F00200
#define XPAR_PS7_GLOBALTIMER_0_S_AXI_HIGHADDR 0xF8F002FF


/* Peripheral Definitions for peripheral PS7_GPV_0 */
#define XPAR_PS7_GPV_0_S_AXI_BASEADDR 0xF8900000
#define XPAR_PS7_GPV_0_S_AXI_HIGHADDR 0xF89FFFFF


/* Peripheral Definitions for peripheral PS7_INTC_DIST_0 */
#define XPAR_PS7_INTC_DIST_0_S_AXI_BASEADDR 0xF8F01000
#define XPAR_PS7_INTC_DIST_0_S_AXI_HIGHADDR 0xF8F01FFF


/* Peripheral Definitions for peripheral PS7_IOP_BUS_CONFIG_0 */
#define XPAR_PS7_IOP_BUS_CONFIG_0_S_AXI_BASEADDR 0xE0200000
#define XPAR_PS7_IOP_BUS_CONFIG_0_S_AXI_HIGHADDR 0xE0200FFF


/* Peripheral Definitions for peripheral PS7_L2CACHEC_0 */
#define XPAR_PS7_L2CACHEC_0_S_AXI_BASEADDR 0xF8F02000
#define XPAR_PS7_L2CACHEC_0_S_AXI_HIGHADDR 0xF8F02FFF


/* Peripheral Definitions for peripheral PS7_OCMC_0 */
#define XPAR_PS7_OCMC_0_S_AXI_BASEADDR 0xF800C000
#define XPAR_PS7_OCMC_0_S_AXI_HIGHADDR 0xF800CFFF


/* Peripheral Definitions for peripheral PS7_PL310_0 */
#define XPAR_PS7_PL310_0_S_AXI_BASEADDR 0xF8F02000
#define XPAR_PS7_PL310_0_S_AXI_HIGHADDR 0xF8F02FFF


/* Peripheral Definitions for peripheral PS7_PMU_0 */
#define XPAR_PS7_PMU_0_S_AXI_BASEADDR 0xF8891000
#define XPAR_PS7_PMU_0_S_AXI_HIGHADDR 0xF8891FFF
#define XPAR_PS7_PMU_0_PMU1_S_AXI_BASEADDR 0xF8893000
#define XPAR_PS7_PMU_0_PMU1_S_AXI_HIGHADDR 0xF8893FFF


/* Peripheral Definitions for peripheral PS7_QSPI_LINEAR_0 */
#define XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR 0xFC000000
#define XPAR_PS7_QSPI_LINEAR_0_S_AXI_HIGHADDR 0xFCFFFFFF


/* Peripheral Definitions for peripheral PS7_RAM_0 */
#define XPAR_PS7_RAM_0_S_AXI_BASEADDR 0x00000000
#define XPAR_PS7_RAM_0_S_AXI_HIGHADDR 0x0003FFFF


/* Peripheral Definitions for peripheral PS7_RAM_1 */
#define XPAR_PS7_RAM_1_S_AXI_BASEADDR 0xFFFC0000
#define XPAR_PS7_RAM_1_S_AXI_HIGHADDR 0xFFFFFFFF


/* Peripheral Definitions for peripheral PS7_SCUC_0 */
#define XPAR_PS7_SCUC_0_S_AXI_BASEADDR 0xF8F00000
#define XPAR_PS7_SCUC_0_S_AXI_HIGHADDR 0xF8F000FC


/* Peripheral Definitions for peripheral PS7_SLCR_0 */
#define XPAR_PS7_SLCR_0_S_AXI_BASEADDR 0xF8000000
#define XPAR_PS7_SLCR_0_S_AXI_HIGHADDR 0xF8000FFF


/******************************************************************/






































/******************************************************************/

/* Definitions for driver GPIOPS */
#define XPAR_XGPIOPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_GPIO_0 */
#define XPAR_PS7_GPIO_0_DEVICE_ID 0
#define XPAR_PS7_GPIO_0_BASEADDR 0xE000A000
#define XPAR_PS7_GPIO_0_HIGHADDR 0xE000AFFF


/******************************************************************/

/* Canonical definitions for peripheral PS7_GPIO_0 */
#define XPAR_XGPIOPS_0_DEVICE_ID XPAR_PS7_GPIO_0_DEVICE_ID
#define XPAR_XGPIOPS_0_BASEADDR 0xE000A000
#define XPAR_XGPIOPS_0_HIGHADDR 0xE000AFFF


/******************************************************************/

/* Definitions for driver IICPS */
#define XPAR_XIICPS_NUM_INSTANCES 2

/* Definitions for peripheral PS7_I2C_0 */
#define XPAR_PS7_I2C_0_DEVICE_ID 0
#define XPAR_PS7_I2C_0_BASEADDR 0xE0004000
#define XPAR_PS7_I2C_0_HIGHADDR 0xE0004FFF
#define XPAR_PS7_I2C_0_I2C_CLK_FREQ_HZ 111111115


/* Definitions for peripheral PS7_I2C_1 */
#define XPAR_PS7_I2C_1_DEVICE_ID 1
#define XPAR_PS7_I2C_1_BASEADDR 0xE0005000
#define XPAR_PS7_I2C_1_HIGHADDR 0xE0005FFF
#define XPAR_PS7_I2C_1_I2C_CLK_FREQ_HZ 111111115


/******************************************************************/

/* Canonical definitions for peripheral PS7_I2C_0 */
#define XPAR_XIICPS_0_DEVICE_ID XPAR_PS7_I2C_0_DEVICE_ID
#define XPAR_XIICPS_0_BASEADDR 0xE0004000
#define XPAR_XIICPS_0_HIGHADDR 0xE0004FFF
#define XPAR_XIICPS_0_I2C_CLK_FREQ_HZ 111111115

/* Canonical definitions for peripheral PS7_I2C_1 */
#define XPAR_XIICPS_1_DEVICE_ID XPAR_PS7_I2C_1_DEVICE_ID
#define XPAR_XIICPS_1_BASEADDR 0xE0005000
#define XPAR_XIICPS_1_HIGHADDR 0xE0005FFF
#define XPAR_XIICPS_1_I2C_CLK_FREQ_HZ 111111115


/******************************************************************/

/* Definition for input Clock */
/* Definition for input Clock */
/* Definitions for driver QSPIPS */
#define XPAR_XQSPIPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_QSPI_0 */
#define XPAR_PS7_QSPI_0_DEVICE_ID 0
#define XPAR_PS7_QSPI_0_BASEADDR 0xE000D000
#define XPAR_PS7_QSPI_0_HIGHADDR 0xE000DFFF
#define XPAR_PS7_QSPI_0_QSPI_CLK_FREQ_HZ 200000000
#define XPAR_PS7_QSPI_0_QSPI_MODE 0
#define XPAR_PS7_QSPI_0_QSPI_BUS_WIDTH 2


/******************************************************************/

/* Canonical definitions for peripheral PS7_QSPI_0 */
#define XPAR_XQSPIPS_0_DEVICE_ID XPAR_PS7_QSPI_0_DEVICE_ID
#define XPAR_XQSPIPS_0_BASEADDR 0xE000D000
#define XPAR_XQSPIPS_0_HIGHADDR 0xE000DFFF
#define XPAR_XQSPIPS_0_QSPI_CLK_FREQ_HZ 200000000
#define XPAR_XQSPIPS_0_QSPI_MODE 0
#define XPAR_XQSPIPS_0_QSPI_BUS_WIDTH 2


/******************************************************************/

/* Definitions for driver SCUGIC */
#define XPAR_XSCUGIC_NUM_INSTANCES 1U

/* Definitions for peripheral PS7_SCUGIC_0 */
#define XPAR_PS7_SCUGIC_0_DEVICE_ID 0U
#define XPAR_PS7_SCUGIC_0_BASEADDR 0xF8F00100U
#define XPAR_PS7_SCUGIC_0_HIGHADDR 0xF8F001FFU
#define XPAR_PS7_SCUGIC_0_DIST_BASEADDR 0xF8F01000U


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUGIC_0 */
#define XPAR_SCUGIC_0_DEVICE_ID 0U
#define XPAR_SCUGIC_0_CPU_BASEADDR 0xF8F00100U
#define XPAR_SCUGIC_0_CPU_HIGHADDR 0xF8F001FFU
#define XPAR_SCUGIC_0_DIST_BASEADDR 0xF8F01000U


/******************************************************************/

/* Definitions for driver SCUTIMER */
#define XPAR_XSCUTIMER_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SCUTIMER_0 */
#define XPAR_PS7_SCUTIMER_0_DEVICE_ID 0
#define XPAR_PS7_SCUTIMER_0_BASEADDR 0xF8F00600
#define XPAR_PS7_SCUTIMER_0_HIGHADDR 0xF8F0061F


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUTIMER_0 */
#define XPAR_XSCUTIMER_0_DEVICE_ID XPAR_PS7_SCUTIMER_0_DEVICE_ID
#define XPAR_XSCUTIMER_0_BASEADDR 0xF8F00600
#define XPAR_XSCUTIMER_0_HIGHADDR 0xF8F0061F


/******************************************************************/

/* Definitions for driver SCUWDT */
#define XPAR_XSCUWDT_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SCUWDT_0 */
#define XPAR_PS7_SCUWDT_0_DEVICE_ID 0
#define XPAR_PS7_SCUWDT_0_BASEADDR 0xF8F00620
#define XPAR_PS7_SCUWDT_0_HIGHADDR 0xF8F006FF


/******************************************************************/

/* Canonical definitions for peripheral PS7_SCUWDT_0 */
#define XPAR_SCUWDT_0_DEVICE_ID XPAR_PS7_SCUWDT_0_DEVICE_ID
#define XPAR_SCUWDT_0_BASEADDR 0xF8F00620
#define XPAR_SCUWDT_0_HIGHADDR 0xF8F006FF


/******************************************************************/

/* Definitions for driver SDPS */
#define XPAR_XSDPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_SD_0 */
#define XPAR_PS7_SD_0_DEVICE_ID 0
#define XPAR_PS7_SD_0_BASEADDR 0xE0100000
#define XPAR_PS7_SD_0_HIGHADDR 0xE0100FFF
#define XPAR_PS7_SD_0_SDIO_CLK_FREQ_HZ 100000000
#define XPAR_PS7_SD_0_HAS_CD 0
#define XPAR_PS7_SD_0_HAS_WP 0
#define XPAR_PS7_SD_0_BUS_WIDTH 0
#define XPAR_PS7_SD_0_MIO_BANK 0
#define XPAR_PS7_SD_0_HAS_EMIO 0
#define XPAR_PS7_SD_0_SLOT_TYPE 0
#define XPAR_PS7_SD_0_CLK_50_SDR_ITAP_DLY 0
#define XPAR_PS7_SD_0_CLK_50_SDR_OTAP_DLY 0
#define XPAR_PS7_SD_0_CLK_50_DDR_ITAP_DLY 0
#define XPAR_PS7_SD_0_CLK_50_DDR_OTAP_DLY 0
#define XPAR_PS7_SD_0_CLK_100_SDR_OTAP_DLY 0
#define XPAR_PS7_SD_0_CLK_200_SDR_OTAP_DLY 0


/******************************************************************/

#define XPAR_PS7_SD_0_IS_CACHE_COHERENT 0
/* Canonical definitions for peripheral PS7_SD_0 */
#define XPAR_XSDPS_0_DEVICE_ID XPAR_PS7_SD_0_DEVICE_ID
#define XPAR_XSDPS_0_BASEADDR 0xE0100000
#define XPAR_XSDPS_0_HIGHADDR 0xE0100FFF
#define XPAR_XSDPS_0_SDIO_CLK_FREQ_HZ 100000000
#define XPAR_XSDPS_0_HAS_CD 0
#define XPAR_XSDPS_0_HAS_WP 0
#define XPAR_XSDPS_0_BUS_WIDTH 0
#define XPAR_XSDPS_0_MIO_BANK 0
#define XPAR_XSDPS_0_HAS_EMIO 0
#define XPAR_XSDPS_0_SLOT_TYPE 0
#define XPAR_XSDPS_0_IS_CACHE_COHERENT 0
#define XPAR_XSDPS_0_CLK_50_SDR_ITAP_DLY 0
#define XPAR_XSDPS_0_CLK_50_SDR_OTAP_DLY 0
#define XPAR_XSDPS_0_CLK_50_DDR_ITAP_DLY 0
#define XPAR_XSDPS_0_CLK_50_DDR_OTAP_DLY 0
#define XPAR_XSDPS_0_CLK_100_SDR_OTAP_DLY 0
#define XPAR_XSDPS_0_CLK_200_SDR_OTAP_DLY 0


/******************************************************************/

/* Definitions for driver V_FRMBUF_RD */
#define XPAR_XV_FRMBUFRD_NUM_INSTANCES 1

/* Definitions for peripheral V_FRMBUF_RD_0 */
#define XPAR_V_FRMBUF_RD_0_DEVICE_ID 0
#define XPAR_V_FRMBUF_RD_0_S_AXI_CTRL_BASEADDR 0x44000000
#define XPAR_V_FRMBUF_RD_0_S_AXI_CTRL_HIGHADDR 0x44FFFFFF
#define XPAR_V_FRMBUF_RD_0_SAMPLES_PER_CLOCK 1
#define XPAR_V_FRMBUF_RD_0_MAX_COLS 480
#define XPAR_V_FRMBUF_RD_0_MAX_ROWS 480
#define XPAR_V_FRMBUF_RD_0_MAX_DATA_WIDTH 8
#define XPAR_V_FRMBUF_RD_0_AXIMM_DATA_WIDTH 64
#define XPAR_V_FRMBUF_RD_0_AXIMM_ADDR_WIDTH 32
#define XPAR_V_FRMBUF_RD_0_HAS_RGBX8 1
#define XPAR_V_FRMBUF_RD_0_HAS_YUVX8 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUYV8 0
#define XPAR_V_FRMBUF_RD_0_HAS_RGBA8 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUVA8 0
#define XPAR_V_FRMBUF_RD_0_HAS_RGBX10 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUVX10 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV8 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV8_420 0
#define XPAR_V_FRMBUF_RD_0_HAS_RGB8 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUV8 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV10 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV10_420 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y8 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y10 0
#define XPAR_V_FRMBUF_RD_0_HAS_BGRA8 0
#define XPAR_V_FRMBUF_RD_0_HAS_BGRX8 0
#define XPAR_V_FRMBUF_RD_0_HAS_UYVY8 0
#define XPAR_V_FRMBUF_RD_0_HAS_BGR8 0
#define XPAR_V_FRMBUF_RD_0_HAS_RGBX12 0
#define XPAR_V_FRMBUF_RD_0_HAS_RGB16 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUVX12 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV12 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV12_420 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y12 0
#define XPAR_V_FRMBUF_RD_0_HAS_YUV16 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV16 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_UV16_420 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y16 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_U_V8 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_U_V10 0
#define XPAR_V_FRMBUF_RD_0_HAS_Y_U_V8_420 0
#define XPAR_V_FRMBUF_RD_0_HAS_INTERLACED 0


/******************************************************************/

/* Canonical definitions for peripheral V_FRMBUF_RD_0 */
#define XPAR_XV_FRMBUFRD_0_DEVICE_ID XPAR_V_FRMBUF_RD_0_DEVICE_ID
#define XPAR_XV_FRMBUFRD_0_S_AXI_CTRL_BASEADDR 0x44000000
#define XPAR_XV_FRMBUFRD_0_S_AXI_CTRL_HIGHADDR 0x44FFFFFF
#define XPAR_XV_FRMBUFRD_0_SAMPLES_PER_CLOCK 1
#define XPAR_XV_FRMBUFRD_0_MAX_COLS 480
#define XPAR_XV_FRMBUFRD_0_MAX_ROWS 480
#define XPAR_XV_FRMBUFRD_0_MAX_DATA_WIDTH 8
#define XPAR_XV_FRMBUFRD_0_AXIMM_DATA_WIDTH 64
#define XPAR_XV_FRMBUFRD_0_AXIMM_ADDR_WIDTH 32
#define XPAR_XV_FRMBUFRD_0_HAS_RGBX8 1
#define XPAR_XV_FRMBUFRD_0_HAS_YUVX8 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUYV8 0
#define XPAR_XV_FRMBUFRD_0_HAS_RGBA8 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUVA8 0
#define XPAR_XV_FRMBUFRD_0_HAS_RGBX10 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUVX10 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV8 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV8_420 0
#define XPAR_XV_FRMBUFRD_0_HAS_RGB8 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUV8 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV10 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV10_420 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y8 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y10 0
#define XPAR_XV_FRMBUFRD_0_HAS_BGRA8 0
#define XPAR_XV_FRMBUFRD_0_HAS_BGRX8 0
#define XPAR_XV_FRMBUFRD_0_HAS_UYVY8 0
#define XPAR_XV_FRMBUFRD_0_HAS_BGR8 0
#define XPAR_XV_FRMBUFRD_0_HAS_RGBX12 0
#define XPAR_XV_FRMBUFRD_0_HAS_RGB16 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUVX12 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV12 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV12_420 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y12 0
#define XPAR_XV_FRMBUFRD_0_HAS_YUV16 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV16 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_UV16_420 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y16 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_U_V8 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_U_V10 0
#define XPAR_XV_FRMBUFRD_0_HAS_Y_U_V8_420 0
#define XPAR_XV_FRMBUFRD_0_HAS_INTERLACED 0


/******************************************************************/

/* Definitions for driver XADCPS */
#define XPAR_XADCPS_NUM_INSTANCES 1

/* Definitions for peripheral PS7_XADC_0 */
#define XPAR_PS7_XADC_0_DEVICE_ID 0
#define XPAR_PS7_XADC_0_BASEADDR 0xF8007100
#define XPAR_PS7_XADC_0_HIGHADDR 0xF8007120


/******************************************************************/

/* Canonical definitions for peripheral PS7_XADC_0 */
#define XPAR_XADCPS_0_DEVICE_ID XPAR_PS7_XADC_0_DEVICE_ID
#define XPAR_XADCPS_0_BASEADDR 0xF8007100
#define XPAR_XADCPS_0_HIGHADDR 0xF8007120


/******************************************************************/

#endif  /* end of protection macro */
