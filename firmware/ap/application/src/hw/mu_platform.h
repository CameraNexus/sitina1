//
// Copyright (c) 2024 Wenting Zhang
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

#define IOREG       (volatile uint32_t *)

// Common definition
#define GPIO_PIN_0  0x00000001
#define GPIO_PIN_1  0x00000002
#define GPIO_PIN_2  0x00000004
#define GPIO_PIN_3  0x00000008
#define GPIO_PIN_4  0x00000010
#define GPIO_PIN_5  0x00000020
#define GPIO_PIN_6  0x00000040
#define GPIO_PIN_7  0x00000080
#define GPIO_PIN_8  0x00000100
#define GPIO_PIN_9  0x00000200
#define GPIO_PIN_10 0x00000400
#define GPIO_PIN_11 0x00000800
#define GPIO_PIN_12 0x00001000
#define GPIO_PIN_13 0x00002000
#define GPIO_PIN_14 0x00004000
#define GPIO_PIN_15 0x00008000
#define GPIO_PIN_16 0x00010000
#define GPIO_PIN_17 0x00020000
#define GPIO_PIN_18 0x00040000
#define GPIO_PIN_19 0x00080000
#define GPIO_PIN_20 0x00100000
#define GPIO_PIN_21 0x00200000
#define GPIO_PIN_22 0x00400000
#define GPIO_PIN_23 0x00800000
#define GPIO_PIN_24 0x01000000
#define GPIO_PIN_25 0x02000000
#define GPIO_PIN_26 0x04000000
#define GPIO_PIN_27 0x08000000
#define GPIO_PIN_28 0x10000000
#define GPIO_PIN_29 0x20000000
#define GPIO_PIN_30 0x40000000
#define GPIO_PIN_31 0x80000000

// Platform specific definition
#define GPIO0_BASE          0x40000000
#define CCDTG_BASE	        0x40010000
#define DSILITE_BASE        0x40020000
#define PWMLITE_BASE        0x40030000
#define DCIF_BASE           0x40040000

#define GPIO0_ODR           (IOREG(GPIO0_BASE + 0x00))
#define GPIO0_IDR           (IOREG(GPIO0_BASE + 0x04))
#define GPIO0_BSR           (IOREG(GPIO0_BASE + 0x08))
#define GPIO0_BCR           (IOREG(GPIO0_BASE + 0x0C))
#define GPIO0_OER           (IOREG(GPIO0_BASE + 0x10))
#define GPIO0_OESR          (IOREG(GPIO0_BASE + 0x14))
#define GPIO0_OECR          (IOREG(GPIO0_BASE + 0x18))

#define CCDTG_CTRL          (IOREG(CCDTG_BASE + 0x00))
#define CCDTG_DELAY_HTIME   (IOREG(CCDTG_BASE + 0x04))
#define CCDTG_DELAY_VTIME   (IOREG(CCDTG_BASE + 0x08))
#define CCDTG_ESHUT_LINE    (IOREG(CCDTG_BASE + 0x0C))

#define CCDTG_CTRL_EN_OFFSET            0
#define CCDTG_CTRL_EMBED_ESHUT_OFFSET   2
#define CCDTG_CTRL_START_ESHUT_OFFSET   3
#define CCDTG_CTRL_VSKIP_OFFSET         4
#define CCDTG_CTRL_CCD_OEN_OFFSET       8
#define CCDTG_CTRL_SYNC_OEN_OFFSET      9

#define DSILITE_PCTL        (IOREG(DSILITE_BASE + 0x00))
#define DSILITE_DMACTL      (IOREG(DSILITE_BASE + 0x04))
#define DSILITE_STARTADDR   (IOREG(DSILITE_BASE + 0x20))
#define DSILITE_ENDADDR     (IOREG(DSILITE_BASE + 0x28))

#define PWMLITE_CTL         (IOREG(PWMLITE_BASE + 0x00))
#define PWMLITE_PSC         (IOREG(PWMLITE_BASE + 0x04))
#define PWMLITE_CNT         (IOREG(PWMLITE_BASE + 0x08))
#define PWMLITE_CMP         (IOREG(PWMLITE_BASE + 0x0C))

#define DCIF_PCTL           (IOREG(DCIF_BASE + 0x00))
#define DCIF_DMACTL         (IOREG(DCIF_BASE + 0x04))
#define DCIF_FBSWAP         (IOREG(DCIF_BASE + 0x08))
#define DCIF_VBLK           (IOREG(DCIF_BASE + 0x10))
#define DCIF_VACT           (IOREG(DCIF_BASE + 0x14))
#define DCIF_HBLK           (IOREG(DCIF_BASE + 0x18))
#define DCIF_HACT           (IOREG(DCIF_BASE + 0x1C))
#define DCIF_STARTADDR0_L   (IOREG(DCIF_BASE + 0x20))
#define DCIF_ENDADDR0_L     (IOREG(DCIF_BASE + 0x28))
#define DCIF_STARTADDR1_L   (IOREG(DCIF_BASE + 0x30))
#define DCIF_ENDADDR1_L     (IOREG(DCIF_BASE + 0x38))

#define DCIF_CTRL_EN_OFFSET             0
#define DCIF_CTRL_USE_VSYNC_OFFSET      1
#define DCIF_CTRL_USE_HSYNC_OFFSET      2
#define DCIF_CTRL_VSYNC_POL_OFFSET      16
#define DCIF_CTRL_HSYNC_POL_OFFSET      17