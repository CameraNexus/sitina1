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
#pragma once

// This driver is based on https://github.com/martinberlin/FT6X36-IDF

#define FT6X36_ADDR                     0x48

#define FT6X36_REG_DEVICE_MODE          0x00
#define FT6X36_REG_GESTURE_ID           0x01
#define FT6X36_REG_NUM_TOUCHES          0x02
#define FT6X36_REG_P1_XH                0x03
#define FT6X36_REG_P1_XL                0x04
#define FT6X36_REG_P1_YH                0x05
#define FT6X36_REG_P1_YL                0x06
#define FT6X36_REG_P1_WEIGHT            0x07
#define FT6X36_REG_P1_MISC              0x08
#define FT6X36_REG_P2_XH                0x09
#define FT6X36_REG_P2_XL                0x0A
#define FT6X36_REG_P2_YH                0x0B
#define FT6X36_REG_P2_YL                0x0C
#define FT6X36_REG_P2_WEIGHT            0x0D
#define FT6X36_REG_P2_MISC              0x0E
#define FT6X36_REG_THRESHOLD            0x80
#define FT6X36_REG_FILTER_COEF          0x85
#define FT6X36_REG_CTRL                 0x86
#define FT6X36_REG_TIME_ENTER_MONITOR   0x87
#define FT6X36_REG_TOUCHRATE_ACTIVE     0x88
#define FT6X36_REG_TOUCHRATE_MONITOR    0x89 // value in ms
#define FT6X36_REG_RADIAN_VALUE         0x91
#define FT6X36_REG_OFFSET_LEFT_RIGHT    0x92
#define FT6X36_REG_OFFSET_UP_DOWN       0x93
#define FT6X36_REG_DISTANCE_LEFT_RIGHT  0x94
#define FT6X36_REG_DISTANCE_UP_DOWN     0x95
#define FT6X36_REG_DISTANCE_ZOOM        0x96
#define FT6X36_REG_LIB_VERSION_H        0xA1
#define FT6X36_REG_LIB_VERSION_L        0xA2
#define FT6X36_REG_CHIPID               0xA3
#define FT6X36_REG_INTERRUPT_MODE       0xA4
#define FT6X36_REG_POWER_MODE           0xA5
#define FT6X36_REG_FIRMWARE_VERSION     0xA6
#define FT6X36_REG_PANEL_ID             0xA8
#define FT6X36_REG_STATE                0xBC

#define FT6X36_PMODE_ACTIVE             0x00
#define FT6X36_PMODE_MONITOR            0x01
#define FT6X36_PMODE_STANDBY            0x02
#define FT6X36_PMODE_HIBERNATE          0x03

/* Possible values returned by FT6X36_GEST_ID_REG */
#define FT6X36_GEST_ID_NO_GESTURE       0x00
#define FT6X36_GEST_ID_MOVE_UP          0x10
#define FT6X36_GEST_ID_MOVE_RIGHT       0x14
#define FT6X36_GEST_ID_MOVE_DOWN        0x18
#define FT6X36_GEST_ID_MOVE_LEFT        0x1C
#define FT6X36_GEST_ID_ZOOM_IN          0x48
#define FT6X36_GEST_ID_ZOOM_OUT         0x49

#define FT6X36_VENDID                   0x11
#define FT6206_CHIPID                   0x06
#define FT6236_CHIPID                   0x36
#define FT6336_CHIPID                   0x64

#define FT6X36_DEFAULT_THRESHOLD        22

// From: https://github.com/lvgl/lv_port_esp32/blob/master/components/lvgl_esp32_drivers/lvgl_touch/ft6x36.h
#define FT6X36_MSB_MASK                 0x0F
#define FT6X36_LSB_MASK                 0xFF

void ft6336_init(void);
