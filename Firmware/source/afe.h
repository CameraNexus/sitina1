//
// afe.h: AD9990 AFE driver
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
#pragma once

// AFE_CS, M11
#define AFE_CS_GPIO         (GPIO1)
#define AFE_CS_GPIO_PIN     (2)
// AFE_SCK, M14
#define AFE_SCK_GPIO        (GPIO1)
#define AFE_SCK_GPIO_PIN    (0)
// AFE_MOSI, H10
#define AFE_MOSI_GPIO       (GPIO1)
#define AFE_MOSI_GPIO_PIN   (1)
// AFE_RST, G11
#define AFE_RST_GPIO        (GPIO1)
#define AFE_RST_GPIO_PIN    (3)
// AFE_SYNC, A7
#define AFE_SYNC_GPIO       (GPIO3)
#define AFE_SYNC_GPIO_PIN   (26)

// Base Registers
                             //Base Offset Mask
// AFE registers
#define AFE_REG_STANDBY         0x00, 0, 0x3
#define AFE_REG_CLPENABLE       0x00, 2, 0x1
#define AFE_REG_CLPSPEED        0x00, 3, 0x1
#define AFE_REG_FASTUPDATE      0x00, 4, 0x1
#define AFE_REG_PBLK_LVL        0x00, 5, 0x1
#define AFE_REG_DCBYP           0x00, 6, 0x1
#define AFE_REG_DOUTDISABLE     0x01, 0, 0x1
#define AFE_REG_DOUTLATCH       0x01, 1, 0x1
#define AFE_REG_GRAYEN          0x01, 2, 0x1
#define AFE_REG_CDSGAIN         0x04, 0, 0x7
#define AFE_REG_VGAGAIN         0x05, 0, 0x1ff
#define AFE_REG_CLIDIVDE        0x06, 0, 0x1
#define AFE_REG_VDHD_IE         0x0e, 8, 0x1
// Miscellaneous Registers
#define AFE_REG_SW_RST          0x10, 0, 0x1
#define AFE_REG_OUTCONTROL      0x11, 0, 0x1
#define AFE_REG_RSTB_EN         0x12, 0, 0x1
#define AFE_REG_SYNCENABLE      0x13, 0, 0x1
#define AFE_REG_SYNCPOL         0x13, 1, 0x1
#define AFE_REG_SYNCSUSPEND     0x13, 2, 0x1
#define AFE_REG_ENH_SYNC_EN     0x13, 3, 0x1
#define AFE_REG_SYNC_MASK_HD    0x13, 4, 0x1
#define AFE_REG_SYNC_MASK_VD    0x13, 5, 0x1
#define AFE_REG_SYNC_MASK_V     0x13, 6, 0x1
#define AFE_REG_SNGL_EDGE_EN    0x13, 14, 0x1
#define AFE_REG_SYNC_RST_SHUTEN 0x13, 15, 0x1
#define AFE_REG_SYNC_RST_GPO    0x13, 16, 0x1
#define AFE_REG_SYNC_RST_FLD    0x13, 17, 0x1
#define AFE_REG_SYNC_DIV        0x13, 20, 0xf
#define AFE_REG_SWSYNC          0x13, 24, 0x1
#define AFE_REG_RST_SHUT        0x13, 25, 0x1
#define AFE_REG_TGCORE_RSTB     0x14, 0, 0x1
#define AFE_REG_OSC_RSTB        0x15, 0, 0x1
#define AFE_REG_UPDATE          0x17, 0, 0xfff
#define AFE_REG_PREVENTUP       0x17, 12, 0x1
#define AFE_REG_VSGSELECT       0x1c, 0, 0xffffff
#define AFE_REG_VSGMASK_CTL     0x1d, 0, 0xffffff
#define AFE_REG_VSGMASK_CTL_EN  0x1d, 24, 0x1
#define AFE_REG_HCNT14_EN       0x1f, 0, 0x1
#define AFE_REG_PBLK_MASK_EN    0x1f, 1, 0x1
// VD/HD Registers
#define AFE_REG_MASTER          0x20, 0, 0x1
#define AFE_REG_HIDDEN_HD_EN    0x20, 1, 0x1
#define AFE_REG_VDHDPOL         0x21, 0, 0x1
#define AFE_REG_MASK_VD_EN      0x21, 8, 0x1
#define AFE_REG_MASK_HD_EN      0x21, 9, 0x1
#define AFE_REG_HDRISE          0x22, 0, 0x1fff
#define AFE_REG_VDRISE          0x22, 13, 0x1fff
// IO Registers
#define AFE_REG_XV_NVR          0x23, 1, 0x1
#define AFE_REG_IO_NVR          0x23, 2, 0x1
#define AFE_REG_DATA_NVR        0x23, 3, 0x1
#define AFE_REG_LDO_32_EN       0x23, 6, 0x1
#define AFE_REG_HCLKMODE        0x24, 0, 0xf
#define AFE_REG_VT_STBY12       0x25, 0, 0x1ffffff
#define AFE_REG_VT_STBY3        0x26, 0, 0x1ffffff
#define AFE_REG_GP_STBY12       0x27, 0, 0xff
#define AFE_REG_GP_STBY3        0x27, 8, 0xff
// Memory Configuration and Mode Registers
#define AFE_REG_VPATNUM         0x28, 0, 0x1f
#define AFE_REG_SEQNUM          0x28, 5, 0x1f
#define AFE_REG_MODE            0x2a, 0, 0x7
#define AFE_REG_FIELD0          0x2b, 0, 0x1f
#define AFE_REG_FIELD1          0x2b, 5, 0x1f
#define AFE_REG_FIELD2          0x2b, 10, 0x1f
#define AFE_REG_FIELD3          0x2b, 15, 0x1f
#define AFE_REG_FIELD4          0x2b, 20, 0x1f
#define AFE_REG_FIELD5          0x2c, 0, 0x1f
#define AFE_REG_FILED6          0x2c, 5, 0x1f
// Timing Core Registers
#define AFE_REG_H1POSLOC        0x30, 0, 0x3f
#define AFE_REG_H1NEGLOC        0x30, 8, 0x3f
#define AFE_REG_H2POSLOC        0x31, 0, 0x3f
#define AFE_REG_H2NEGLOC        0x31, 8, 0x3f
#define AFE_REG_HLAPOSLOC       0x32, 0, 0x3f
#define AFE_REG_HLANEGLOC       0x32, 8, 0x3f
#define AFE_REG_HLBPOSLOC       0x33, 0, 0x3f
#define AFE_REG_HLBNEGLOC       0x33, 8, 0x3f
#define AFE_REG_RGPOSLOC        0x34, 0, 0x3f
#define AFE_REG_RGNEGLOC        0x34, 8, 0x3f
#define AFE_REG_H1HBLKRETIME    0x35, 0, 0x1
#define AFE_REG_H2HBLKRETIME    0x35, 1, 0x1
#define AFE_REG_HLAHBLKRETIME   0x35, 2, 0x1
#define AFE_REG_HLBHBLKRETIME   0x35, 3, 0x1
#define AFE_REG_HCLK_WIDTH      0x35, 4, 0xf
#define AFE_REG_HL_HBLK_EN      0x35, 9, 0x1
#define AFE_REG_H1ADRV          0x36, 0, 0x7
#define AFE_REG_H2ADRV          0x36, 4, 0x7
#define AFE_REG_H3ADRV          0x36, 8, 0x7
#define AFE_REG_H4ADRV          0x36, 12, 0x7
#define AFE_REG_HLADRV          0x36, 16, 0x7
#define AFE_REG_RGADRV          0x36, 20, 0x7
#define AFE_REG_H1BDRV          0x37, 0, 0x7
#define AFE_REG_H2BDRV          0x37, 4, 0x7
#define AFE_REG_H3BDRV          0x37, 8, 0x7
#define AFE_REG_H4BDRV          0x37, 12, 0x7
#define AFE_REG_HLBDRV          0x37, 16, 0x7
#define AFE_REG_RGBDRV          0x37, 20, 0x7
#define AFE_REG_SHDLOC          0x38, 0, 0x3f
#define AFE_REG_SHPLOC          0x38, 8, 0x3f
#define AFE_REG_SHPWIDTH        0x38, 16, 0x3f
#define AFE_REG_DOUTPHASEP      0x39, 0, 0x3f
#define AFE_REG_DOUTPHASEN      0x39, 8, 0x3f
#define AFE_REG_DCLKMODE        0x39, 16, 0x1
#define AFE_REG_DOUTDELAY       0x39, 17, 0x3
#define AFE_REG_DCLKINV         0x39, 19, 0x1
#define AFE_REG_DCLK_EDGEMODE   0x39, 20, 0x3
#define AFE_REG_DATA_SWAP       0x39, 22, 0x1
#define AFE_REG_HL_HBLKSTART    0x3a, 0, 0x1fff
#define AFE_REG_HL_HBLKEND      0x3a, 13, 0x1fff
#define AFE_REG_FINE_RETIME_H1  0x3b, 0, 0x3
#define AFE_REG_FINE_RETIME_H2  0x3b, 2, 0x3
#define AFE_REG_FINE_RETIME_HL1 0x3b, 4, 0x3
#define AFE_REG_FINE_RETIME_HL2 0x3b, 6, 0x3
// Shutter and GPO Registers
#define AFE_REG_PRIMARY_ACTION  0x70, 0, 0x7
#define AFE_REG_SECONARY_ACTION 0x70, 3, 0x7
#define AFE_REG_MANUAL_TRIG     0x70, 6, 0xff
#define AFE_REG_PRIMARY_MAX     0x71, 0, 0x1fff
#define AFE_REG_SECOND_MAX      0x71, 13, 0x1fff
#define AFE_REG_VDHD_MASK       0x71, 25, 0x7
#define AFE_REG_PRIMARY_DELAY   0x72, 0, 0x1fff
#define AFE_REG_PRIMARY_SKIP    0x72, 13, 0x1
#define AFE_REG_SECOND_DELAY    0x72, 14, 0x1fff
#define AFE_REG_SECOND_SKIP     0x72, 27, 0x1
#define AFE_REG_GP1_PROTOCOL    0x73, 0, 0x7
#define AFE_REG_GP2_PROTOCOL    0x73, 3, 0x7
#define AFE_REG_GP3_PROTOCOL    0x73, 6, 0x7
#define AFE_REG_GP4_PROTOCOL    0x73, 9, 0x7
#define AFE_REG_GP5_PROTOCOL    0x73, 12, 0x7
#define AFE_REG_GP6_PROTOCOL    0x73, 15, 0x7
#define AFE_REG_GP7_PROTOCOL    0x73, 18, 0x7
#define AFE_REG_GP8_PROTOCOL    0x73, 21, 0x7
#define AFE_REG_SGMASK_NUM      0x74, 0, 0x1fff
#define AFE_REG_SUBCKMASK_NUM   0x74, 13, 0x1fff
#define AFE_REG_SUBCKTOG_UPDATE 0x74, 26, 0x1
#define AFE_REG_SUBCKMASK_SKIP1 0x74, 27, 0x1
#define AFE_REG_SUBCKSUPPRESS   0x75, 1, 0x1fff
#define AFE_REG_SUBCKNUM        0x75, 14, 0x1fff
#define AFE_REG_SG_SUPPRESS     0x75, 27, 0x1
#define AFE_REG_SUBCK_POL       0x76, 0, 0x1
#define AFE_REG_XSUBTYPE        0x76, 1, 0x1
#define AFE_REG_SUBCK_TOG1      0x77, 0, 0x3fff
#define AFE_REG_SUBCK_TOG2      0x77, 14, 0x3fff
#define AFE_REG_SUBCKHP_TOG1    0x78, 0, 0x3fff
#define AFE_REG_SUBCKHP_TOG2    0x78, 14, 0x3fff
#define AFE_REG_GP_POL          0x7a, 0, 0xff
#define AFE_REG_SEL_GPO         0x7a, 8, 0xff
#define AFE_REG_GPO_OUTPUT_EN   0x7a, 16, 0xff
#define AFE_REG_GPO5_OVERRIDE   0x7a, 24, 0x1
#define AFE_REG_GPO6_OVERRIDE   0x7a, 25, 0x1
#define AFE_REG_GPO7_OVERRIDE   0x7a, 26, 0x1
#define AFE_REG_GPO8_OVERRIDE   0x7a, 27, 0x1
#define AFE_REG_GP_USE_LUT      0x7b, 0, 0xff
#define AFE_REG_LUT_FOR_GP12    0x7b, 8, 0xf
#define AFE_REG_LUT_FOR_GP34    0x7b, 12, 0xf
#define AFE_REG_LUT_FOR_GP56    0x7b, 16, 0xf
#define AFE_REG_LUT_FOR_GP78    0x7b, 20, 0xf
// Update Control Registers
#define AFE_REG_AFE_UPDT_SL     0xb0, 0, 0xffff
#define AFE_REG_AFE_UPDT_VD     0xb1, 0, 0xffff
#define AFE_REG_MISC_UPDT_SL    0xb2, 0, 0xffff
#define AFE_REG_MISC_UPDT_VD    0xb3, 0, 0xffff
#define AFE_REG_VDHD_UPDT_SL    0xb4, 0, 0xffff
#define AFE_REG_VDHD_UPDT_VD    0xb5, 0, 0xffff
#define AFE_REG_TC_UPDT_SL      0xb6, 0, 0x3f
#define AFE_REG_TC_UPDT_VD      0xb7, 0, 0x3f
// Extra Registers
#define AFE_REG_GPO_MASK_HIGH   0xc3, 0, 0xff
#define AFE_REG_GPO_MASK_LOW    0xc3, 8, 0xff
#define AFE_REG_DOUTDRV         0xd8, 0, 0xfffffff
