//
// Sitina1
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
#pragma once

// Possible actions after ticking the screen
typedef enum {
    CAP_ACT_NOTHING,
    CAP_ACT_CAPTURE,
    CAP_ACT_PLAYBACK,
    CAP_ACT_SETTING
} CAP_ACT;

typedef enum {
    PB_ACT_NOTHING,
    PB_ACT_PREV,
    PB_ACT_NEXT,
    PB_ACT_EXIT
} PB_ACT;

typedef enum {
    SET_ACT_NOTHING,
    SET_ACT_EXIT
} SET_ACT;

void gui_init(void);
CAP_ACT gui_run_capture_screen(bool redraw);
PB_ACT gui_run_playback_screen(bool redraw);
SET_ACT gui_run_setting_screen(bool redraw);
