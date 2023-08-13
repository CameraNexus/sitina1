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
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "os_camera.h"
#include "os_display.h"
#include "os_input.h"
#include "gui.h"
#include "ugui.h"
#include "app_main.h"

static UG_GUI gui;
static uint32_t *framebuffer;

#define DEFAULT_FONT FONT_terminus_bold_14X28
#define LARGE_FONT FONT_terminus_bold_16X32

#define WND_PREVIEW_MAX_OBJECTS 10

static UG_WINDOW wnd_preview;
static UG_BUTTON btn_iso_inc;
static UG_BUTTON btn_iso_dec;
static UG_BUTTON btn_shutter_inc;
static UG_BUTTON btn_shutter_dec;
static UG_BUTTON btn_shutter_release;
static UG_TEXTBOX tb_iso;
static UG_TEXTBOX tb_iso_val;
static UG_TEXTBOX tb_shutter;
static UG_TEXTBOX tb_shutter_val;
static UG_OBJECT obj_buff_wnd_preview[WND_PREVIEW_MAX_OBJECTS];

#define BTN_ISO_INC_ID      OBJ_ID_0
#define BTN_ISO_DEC_ID      OBJ_ID_1
#define BTN_SHUTTER_INC_ID  OBJ_ID_4
#define BTN_SHUTTER_DEC_ID  OBJ_ID_5
#define BTN_SHUTTER_REL_ID  OBJ_ID_8
#define TB_ISO_TITLE_ID     OBJ_ID_2
#define TB_ISO_VAL_ID       OBJ_ID_3
#define TB_SHUTTER_TITLE_ID OBJ_ID_6
#define TB_SHUTTER_VAL_ID   OBJ_ID_7

#define WND_PROGRESS_MAX_OBJECTS 3

static UG_WINDOW wnd_progress;
static UG_TEXTBOX tb_saving_label;
static UG_PROGRESS pg_saving;
static UG_OBJECT obj_buff_wnd_progress[WND_PROGRESS_MAX_OBJECTS];

#define TB_SAVING_LABEL_ID  OBJ_ID_0
#define PG_SAVING_ID        OBJ_ID_1

static uint32_t iso_setpoints[] = {
    100, 200, 400, 800, 1600, 3200, 6400
};

#define DEFAULT_ISO_INDEX 0
#define MAX_ISO_INDEX (sizeof(iso_setpoints) / sizeof(iso_setpoints[0]))

static char iso_labels[MAX_ISO_INDEX][5];

static uint32_t shutter_setpoints[] = {
    3000, 2000, 1500, 1000, 800, 640, 500, 400, 320, 250, 200, 160, 125, 100,
    80, 60, 50, 40, 30, 25, 20, 15, 10, 8, 4, 2, 1
};

#define DEFAULT_SHUTTER_INDEX 13
#define MAX_SHUTTER_INDEX (sizeof(shutter_setpoints) / sizeof(shutter_setpoints[0]))

static char shutter_labels[MAX_SHUTTER_INDEX][7];

static size_t iso_index = DEFAULT_ISO_INDEX;
static size_t shutter_index = DEFAULT_SHUTTER_INDEX;

static void gui_set_pixel(int16_t x, int16_t y, uint32_t c) {
    uint32_t *fbptr = framebuffer;
    fbptr[y * DISP_WIDTH + x] = c;
}

static void gui_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        uint32_t c) {
    uint32_t *fbptr = framebuffer;
    for (size_t y = y0; y <= y1; y++) {
        for (size_t x = x0; x <= x1; x++) {
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

    for (int i = 0; i < MAX_ISO_INDEX; i++) {
        snprintf(iso_labels[i], 5, "%d", iso_setpoints[i]);
    }

    for (int i = 0; i < MAX_SHUTTER_INDEX; i++) {
        snprintf(shutter_labels[i], 7, "1/%d", shutter_setpoints[i]);
    }
}

void gui_deinit(void) {
    // Nothing to do here
}

static void update_iso() {
    UG_TextboxSetText(&wnd_preview, TB_ISO_VAL_ID, iso_labels[iso_index]);
    //os_cam_set_gain(iso_setpoints[iso_index]);
}

static void update_shutter_speed() {
    UG_TextboxSetText(&wnd_preview, TB_SHUTTER_VAL_ID, shutter_labels[shutter_index]);
    uint32_t shutter_ns = 1000000000 / shutter_setpoints[shutter_index];
    os_cam_set_shutter_speed(shutter_ns);
}

static void wnd_preview_callback(UG_MESSAGE *msg) {
    //
    if ((msg->type == MSG_TYPE_OBJECT) && (msg->id == OBJ_TYPE_BUTTON) &&
            (msg->event == OBJ_EVENT_RELEASED)) {
        switch (msg->sub_id) {
        case BTN_ISO_INC_ID:
            if (iso_index < (MAX_ISO_INDEX - 1)) {
                iso_index++;
            }
            update_iso();
            break;
        case BTN_ISO_DEC_ID:
            if (iso_index > 0) {
                iso_index--;
            }
            update_iso();
            break;
        case BTN_SHUTTER_INC_ID:
            if (shutter_index < (MAX_SHUTTER_INDEX - 1)) {
                shutter_index++;
            }
            update_shutter_speed();
            break;
        case BTN_SHUTTER_DEC_ID:
            if (shutter_index > 0) {
                shutter_index--;
            }
            update_shutter_speed();
            break;
        case BTN_SHUTTER_REL_ID:
            shutter_release();
            break;
        default:
            break;
        }
    }
}

static void _gui_create_button(UG_WINDOW* wnd, UG_BUTTON *handle, UG_U8 id,
        char *text, UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h) {
    UG_ButtonCreate(wnd, handle, id, x, y, x+w-1, y+h-1);
    UG_ButtonSetText(wnd, id, text);
}

static void _gui_create_textbox(UG_WINDOW* wnd, UG_TEXTBOX *handle, UG_U8 id,
        char *text, UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h, UG_U8 align) {
    UG_TextboxCreate(wnd, handle, id, x, y, x+w-1, y+h-1);
    UG_TextboxSetText(wnd, id, text);
    UG_TextboxSetAlignment(wnd, id, align);
}

static void _gui_create_progress(UG_WINDOW* wnd, UG_PROGRESS *handle, UG_U8 id,
        UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h) {
    UG_ProgressCreate(wnd, handle, id, x, y, x+w-1, y+h-1);
}

void gui_setup_preview_screen(void) {
    // Create the window
    UG_WindowCreate(&wnd_preview, obj_buff_wnd_preview, WND_PREVIEW_MAX_OBJECTS, wnd_preview_callback);
    // Window Title
    UG_WindowSetStyle(&wnd_preview, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
    UG_WindowSetXStart(&wnd_preview, 0);
    UG_WindowSetYStart(&wnd_preview, 0);
    UG_WindowSetXEnd(&wnd_preview, DISP_WIDTH-1);
    UG_WindowSetYEnd(&wnd_preview, DISP_HEIGHT-1);

    // Create Buttons
    _gui_create_button(&wnd_preview, &btn_iso_inc, BTN_ISO_INC_ID, "+", 250, 620, 80, 80);
    _gui_create_button(&wnd_preview, &btn_iso_dec, BTN_ISO_DEC_ID, "-", 20, 620, 80, 80);
    _gui_create_button(&wnd_preview, &btn_shutter_inc, BTN_SHUTTER_INC_ID, "+", 390, 620, 80, 80);
    _gui_create_button(&wnd_preview, &btn_shutter_dec, BTN_SHUTTER_DEC_ID, "-", 620, 620, 80, 80);

    _gui_create_button(&wnd_preview, &btn_shutter_release, BTN_SHUTTER_REL_ID, "REL", 620, 20, 80, 80);

    // Create Textbox
    _gui_create_textbox(&wnd_preview, &tb_iso, TB_ISO_TITLE_ID, "ISO", 110, 620, 140, 40, ALIGN_CENTER_LEFT);
    _gui_create_textbox(&wnd_preview, &tb_iso_val, TB_ISO_VAL_ID, "-", 100, 650, 150, 40, ALIGN_CENTER);
    _gui_create_textbox(&wnd_preview, &tb_shutter, TB_SHUTTER_TITLE_ID, "Shutter", 480, 620, 140, 40, ALIGN_CENTER_LEFT);
    _gui_create_textbox(&wnd_preview, &tb_shutter_val, TB_SHUTTER_VAL_ID, "-", 470, 650, 150, 40, ALIGN_CENTER);

    UG_TextboxSetFont(&wnd_preview, TB_ISO_VAL_ID, LARGE_FONT);
    UG_TextboxSetFont(&wnd_preview, TB_SHUTTER_VAL_ID, LARGE_FONT);

    update_iso();
    update_shutter_speed();

    UG_WindowHide(&wnd_preview);
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

static void wnd_progress_callback(UG_MESSAGE *msg) {

}

void gui_show_preview_screen(void) {
    UG_WindowShow(&wnd_preview);
    UG_Update();
}

void gui_hide_preview_screen(void) {
    UG_WindowHide(&wnd_preview);
    UG_Update();
}

void gui_setup_progress_screen(void) {
    UG_WindowCreate(&wnd_progress, obj_buff_wnd_progress, WND_PROGRESS_MAX_OBJECTS, wnd_progress_callback);
    UG_WindowSetStyle(&wnd_progress, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
    UG_WindowSetXStart(&wnd_progress, 0);
    UG_WindowSetYStart(&wnd_progress, 0);
    UG_WindowSetXEnd(&wnd_progress, DISP_WIDTH-1);
    UG_WindowSetYEnd(&wnd_progress, DISP_HEIGHT-1);

    _gui_create_textbox(&wnd_progress, &tb_saving_label, TB_SAVING_LABEL_ID, "Image saving in progress...",
            100, 610, 520, 50, ALIGN_CENTER);
    
    _gui_create_progress(&wnd_progress, &pg_saving, PG_SAVING_ID, 100, 670, 520, 30);

    UG_WindowHide(&wnd_progress);
}

void gui_show_progress_screen(void) {
    UG_WindowShow(&wnd_progress);
    UG_Update();
}

void gui_hide_progress_screen(void) {
    UG_WindowHide(&wnd_progress);
    UG_Update();
}

void gui_set_progress(uint8_t val) {
    UG_ProgressSetProgress(&wnd_progress, PG_SAVING_ID, val);
    UG_Update();
}
