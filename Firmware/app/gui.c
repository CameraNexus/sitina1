//
// Sitina1
// Copyright 2022 Wenting Zhang
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
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "os_display.h"
#include "os_input.h"
#include "gui.h"
#include "ugui.h"

static UG_GUI gui;
static uint32_t *framebuffer;

#define DEFAULT_FONT FONT_arial_25X28

#define MAX_OBJECTS 10

static UG_WINDOW window_1;
static UG_BUTTON btn_iso_inc;
static UG_BUTTON btn_iso_dec;
static UG_BUTTON btn_shutter_inc;
static UG_BUTTON btn_shutter_dec;
static UG_BUTTON btn_shutter_release;
static UG_TEXTBOX tb_iso;
static UG_TEXTBOX tb_iso_val;
static UG_TEXTBOX tb_shutter;
static UG_TEXTBOX tb_shutter_val;
static UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];

static void gui_set_pixel(int16_t x, int16_t y, uint32_t c) {
    uint32_t *fbptr = framebuffer;
    fbptr[y * DISP_WIDTH + x] = c;
}

static void gui_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        uint32_t c) {
    uint32_t *fbptr = framebuffer;
    for (size_t y = y0; y < y1; y++) {
        for (size_t x = x0; x < x1; x++) {
            fbptr[y * DISP_WIDTH + x] = c;
        }
    }
}

static void gui_flush(void) {
    //os_disp_return_buffer(framebuffer);
    // Do nothing. GUI subsystem does not control the screen update
}

static UG_DEVICE device = {
    .x_dim = DISP_WIDTH,
    .y_dim = DISP_HEIGHT,
    .pset = gui_set_pixel,
    .flush = gui_flush
};

void gui_init(void) {
    // Only works if underlying hardware is single buffered.
    framebuffer = os_disp_get_buffer();
    UG_Init(&gui, &device);
    // UG_DriverRegister(DRIVER_DRAW_LINE, something);
    UG_DriverRegister(DRIVER_FILL_FRAME, gui_fill);
    // UG_DriverRegister(DRIVER_FILL_AREA, something);
    // UG_DriverRegister(DRIVER_DRAW_BMP, something);
    UG_FontSetHSpace(0);
    UG_FontSetVSpace(0);
    UG_FontSelect(DEFAULT_FONT);
    UG_FillScreen(C_BLACK);
    UG_Update();
}

void gui_deinit(void) {
    // Nothing to do here
}

static void window_1_callback(UG_MESSAGE *msg)
{
    //
    if(msg->type == MSG_TYPE_OBJECT)
    {
        if(msg->id == OBJ_TYPE_BUTTON)
        {
            if(msg->event == OBJ_EVENT_PRESSED)
            {
                switch(msg->sub_id)
                {
                    case BTN_ID_0:
                    {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "Pressed!");
                        break;
                    }
                }
            }
            if(msg->event == OBJ_EVENT_RELEASED)
            {
                if(msg->sub_id == BTN_ID_0)
                {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "This is a \n test sample window!");
                }
            }
        }
    }
}

static void _gui_create_button(UG_WINDOW* wnd, UG_BUTTON *handle, UG_U8 id,
        char *text, UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h) {
    UG_ButtonCreate(wnd, handle, id, x, y, x+w, y+h);
    UG_ButtonSetText(wnd, id, text);
}

static void _gui_create_textbox(UG_WINDOW* wnd, UG_TEXTBOX *handle, UG_U8 id,
        char *text, UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h, UG_U8 align) {
    UG_TextboxCreate(wnd, handle, id, x, y, x+w, y+h);
    UG_TextboxSetText(wnd, id, text);
    UG_TextboxSetAlignment(wnd, id, align);
}

void gui_setup_preview_screen(void) {
    // Create the window
    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);
    // Window Title
    UG_WindowSetStyle(&window_1, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
    UG_WindowSetXStart(&window_1, 0);
    UG_WindowSetYStart(&window_1, 0);
    UG_WindowSetXEnd(&window_1, DISP_WIDTH-1);
    UG_WindowSetYEnd(&window_1, DISP_HEIGHT-1);

    // Create Buttons
    _gui_create_button(&window_1, &btn_iso_inc, OBJ_ID_0, "+", 250, 650, 50, 50);
    _gui_create_button(&window_1, &btn_iso_dec, OBJ_ID_1, "-", 50, 650, 50, 50);
    _gui_create_button(&window_1, &btn_shutter_inc, OBJ_ID_4, "+", 620, 650, 50, 50);
    _gui_create_button(&window_1, &btn_shutter_dec, OBJ_ID_5, "-", 420, 650, 50, 50);

    _gui_create_button(&window_1, &btn_shutter_release, OBJ_ID_8, "REL", 620, 20, 80, 80);

    // Create Textbox
    _gui_create_textbox(&window_1, &tb_iso, OBJ_ID_2, "ISO", 50, 600, 200, 50, ALIGN_CENTER_LEFT);
    _gui_create_textbox(&window_1, &tb_iso_val, OBJ_ID_3, "100", 100, 650, 150, 50, ALIGN_CENTER);
    _gui_create_textbox(&window_1, &tb_shutter, OBJ_ID_6, "Shutter", 420, 600, 200, 50, ALIGN_CENTER_LEFT);
    _gui_create_textbox(&window_1, &tb_shutter_val, OBJ_ID_7, "1/100", 470, 650, 150, 50, ALIGN_CENTER);

    UG_WindowShow(&window_1);
    UG_Update();
}

void gui_scan_input(void) {
    touch_point touch_state;
    os_input_get_touch(&touch_state);
    if (touch_state.touched) {
        UG_TouchUpdate(touch_state.x, touch_state.y, TOUCH_STATE_PRESSED);
    }
    else {
        UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
    }
    UG_Update();
}
