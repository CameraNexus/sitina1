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
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pal_power.h"
#include "pal_input.h"
#include "pal_camera.h"
#include "pal_display.h"
#include "uilib.h"
#include "gui.h"
#include "metadata.h"
#include "assets/assets.h"

#define COLOR_FOCUSED   COLOR_WHITE
#define COLOR_DIM       COLOR_GREY41
#define COLOR_NORMAL    COLOR_WHITE
#define COLOR_BG        COLOR_BLACK

UICOMP label_run = {
    .type = COMP_LABEL,
    .x = 10,
    .y = 10,
    .w = 50,
    .h = 20,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_GREEN,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_LEFT,
        .string = "RUN"
    }
};

UICOMP bmp_shutter = {
    .type = COMP_BITMAP,
    .x = 400,
    .y = 0,
    .w = 80,
    .h = 80,
    .specifics.bitmap = {
        .pixelformat = PIXFMT_Y8,
        .buf_width = 80,
        .buf_height = 80,
        .pbuf = (uint8_t *)image_shutter
    }
};

UICOMP label_mode = {
    .type = COMP_LABEL,
    .x = 10,
    .y = 30,
    .w = 30,
    .h = 40,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_24x40,
        .align = ALIGN_LEFT,
        .string = "M"
    }
};

UICOMP label_fn1 = {
    .type = COMP_LABEL,
    .x = 320,
    .y = 60,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_RIGHT,
        .string = "Aper"
    }
};

UICOMP label_fn2 = {
    .type = COMP_LABEL,
    .x = 315,
    .y = 40,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_RIGHT,
        .string = "ISO"
    }
};

UICOMP label_fn3 = {
    .type = COMP_LABEL,
    .x = 325,
    .y = 20,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_RIGHT,
        .string = "Mode"
    }
};

UICOMP label_fn4 = {
    .type = COMP_LABEL,
    .x = 345,
    .y = 0,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_RIGHT,
        .string = "Focus"
    }
};

UICOMP label_logo = {
    .type = COMP_LABEL,
    .x = 394,
    .y = 44,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_8x14,
        .align = ALIGN_RIGHT,
        .string = "Sitina"
    }
};

UICOMP bmp_dpad1 = {
    .type = COMP_BITMAP,
    .x = 400,
    .y = 400,
    .w = 80,
    .h = 80,
    .specifics.bitmap = {
        .pixelformat = PIXFMT_Y8,
        .buf_width = 80,
        .buf_height = 80,
        .pbuf = (uint8_t *)image_dpad1
    }
};

UICOMP label_dpad = {
    .type = COMP_LABEL,
    .x = 400,
    .y = 421,
    .w = 80,
    .h = 40,
    .specifics.label = {
        .transparent = true,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_CENTER,
        .string = "Zoom"
    }
};

UICOMP label_shutter = {
    .type = COMP_LABEL,
    .x = 10,
    .y = 445,
    .w = 7*16,
    .h = 26,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_16x26,
        .align = ALIGN_LEFT,
        .string = "1/8000s"
    }
};

UICOMP label_aperture = {
    .type = COMP_LABEL,
    .x = 150,
    .y = 445,
    .w = 5*16,
    .h = 26,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_16x26,
        .align = ALIGN_LEFT,
        .string = "f/5.6"
    }
};

UICOMP label_iso = {
    .type = COMP_LABEL,
    .x = 260,
    .y = 445,
    .w = 8*16,
    .h = 26,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_16x26,
        .align = ALIGN_LEFT,
        .string = "ISO-6400"
    }
};

void gui_draw_battery(void *ptr);

UICOMP custom_battery = {
    .type = COMP_CUSTOM,
    .x = 330,
    .y = 410,
    .w = 60,
    .h = 20,
    .specifics.custom = {
        .user = 0, // unused
        .drawfunc = gui_draw_battery
    }
};

UICOMP label_exposure = {
    .type = COMP_LABEL,
    .x = 220,
    .y = 410,
    .w = 12 * 4,
    .h = 20,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_LEFT,
        .string = "+0.0"
    }
};

UICOMP label_focus = {
    .type = COMP_LABEL,
    .x = 290,
    .y = 410,
    .w = 12 * 2,
    .h = 20,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_12x20,
        .align = ALIGN_LEFT,
        .string = "MF"
    }
};

void gui_draw_exposure(void *ptr);

UICOMP custom_exposure = {
    .type = COMP_CUSTOM,
    .x = 10,
    .y = 405,
    .w = 200,
    .h = 30,
    .specifics.custom = {
        .user = 0, // unused
        .drawfunc = gui_draw_exposure
    }
};

void gui_draw_histogram(void *ptr);

UICOMP custom_histogram = {
    .type = COMP_CUSTOM,
    .x = 70,
    .y = 9,
    .w = 258,
    .h = 65,
    .specifics.custom = {
        .user = 0, // unused
        .drawfunc = gui_draw_histogram
    }
};

UICOMP label_debug1 = {
    .type = COMP_LABEL,
    .x = 10,
    .y = 405,
    .w = 8 * 16,
    .h = 28,
    .specifics.label = {
        .transparent = false,
        .fgcl = COLOR_NORMAL,
        .bgcl = COLOR_BG,
        .font = &font_8x14,
        .align = ALIGN_LEFT,
        .string = "*L SHD 15 SHP 35 R SHD 15 SHP 35"
    }
};

UIDRAWLIST capture_screen_drawlist = {
    .ncomp = 18,
    .comp = {
        &label_run,
        &label_mode,
        &label_focus,
        &bmp_shutter,
        &label_fn1,
        &label_fn2,
        &label_fn3,
        &label_fn4,
        &label_logo,
        &bmp_dpad1,
        &label_dpad,
        &label_shutter,
        &label_aperture,
        &label_iso,
        &custom_battery,
        &label_exposure,
        //&custom_exposure,
        &label_debug1,
        &custom_histogram
    }
};

UIDRAWLIST capture_screen_always_update_drawlist = {
    // TODO: Add exposure meter here
    .ncomp = 1,
    .comp = {
        &custom_histogram
    }
};

void gui_draw_battery(void *ptr) {
    UICOMP *uicomp = (UICOMP *)ptr;
    uint32_t x = uicomp->x;
    uint32_t y = uicomp->y;
    uint32_t w = uicomp->w;
    uint32_t h = uicomp->h;
    uilib_fill_rect(x, y, w, h, COLOR_BG);
    const uint32_t W = 2; // Width of shell
    const uint32_t T = 4; // Width of tab
    const uint32_t TD = 4; // Distance from edge to tab vertically
    const uint32_t S = 1; // Spacing between battery inner and outer shell
    const uint32_t CL = COLOR_NORMAL;
    uilib_fill_rect(x, y, w - T, W, CL);
    uilib_fill_rect(x, y + h - W, w - T, W, CL);
    uilib_fill_rect(x, y + 2, W, h - 2 * W, CL);
    uilib_fill_rect(x + w - T - W, y + 2, W, h - 2 * W, CL);
    uilib_fill_rect(x + w - T, y + TD, T, h - 2 * TD, CL);
    uint8_t bat_pct = pal_pwr_get_battery_percent();
    bool bat_charging = pal_pwr_is_battery_charging();
    uint32_t bat_w = w - TD - W * 2 - S * 2;
    bat_w = bat_w * bat_pct / 100;
    uilib_fill_rect(x + W + S, y + W + S, bat_w, h - 2 * W - 2 * S,
        (bat_charging) ? COLOR_GREEN :
        (bat_pct > 20) ? COLOR_NORMAL : COLOR_RED);
}

void gui_draw_exposure(void *ptr) {
    UICOMP *uicomp = (UICOMP *)ptr;
    uint32_t x = uicomp->x;
    uint32_t y = uicomp->y;
    uint32_t w = uicomp->w;
    uint32_t h = uicomp->h;
    uilib_fill_rect(x, y, w, h, COLOR_BG);
    uint32_t vcenter = y + h / 2;
    uint32_t hcenter = x + w / 2;
    for (int i = 0; i < 6*3+1; i++) {
        uint32_t xx = x + 18 + i * 9;
        uint32_t yy;
        uint32_t hh;
        bool draw_bar; // based on current exposure
        if (i < 3*3) {
            hh = ((i % 3) == 0) ? 10 : 5;
            yy = vcenter - hh;
            // TODO
            draw_bar = false;
        }
        else if (i == 3*3) {
            hh = 18;
            yy = vcenter - hh + 5;
            draw_bar = true;
        }
        else {
            hh = ((i % 3) == 0) ? 10 : 5;
            yy = vcenter - hh;
            // TODO
            draw_bar = false;
        }
        if (draw_bar)
            uilib_fill_rect(xx, vcenter + 4, 4, h / 2 - 4, COLOR_NORMAL);
        uilib_fill_rect(xx, yy, 4, hh, COLOR_NORMAL);
    }
    uilib_fill_rect(x, vcenter-1, 13, 3, COLOR_NORMAL);
    uilib_fill_rect(x + 188, vcenter-1, 13, 3, COLOR_NORMAL);
    uilib_fill_rect(x + 193, vcenter-6, 3, 13, COLOR_NORMAL);
}

void gui_draw_histogram(void *ptr) {
    UICOMP *uicomp = (UICOMP *)ptr;
    uint32_t x = uicomp->x;
    uint32_t y = uicomp->y;
    uint32_t w = uicomp->w;
    uint32_t h = uicomp->h;
    //uilib_fill_rect(x, y, w, h, COLOR_BLUE);
    uilib_fill_rect(x, y, w, 1, COLOR_NORMAL);
    uilib_fill_rect(x, y + h - 1, w, 1, COLOR_NORMAL);
    uilib_fill_rect(x, y + 1, 1, h - 2, COLOR_NORMAL);
    uilib_fill_rect(x + w - 1, y + 1, 1, h - 2, COLOR_NORMAL);
    // TODO: Feed source from actual histogram
    for (int i = 0; i < 256; i++) {
        uint8_t level = histogram[i];
        level /= 4; // 0-63
        if (level != 63)
            uilib_fill_rect(x + 1 + i, y + 1, 1, 63-level, COLOR_BG);
        uilib_fill_rect(x + 1 + i, y + (63-level) + 1, 1, level, COLOR_NORMAL);
    }
}

void gui_init(void) {

}

CAP_ACT gui_run_capture_screen(bool redraw) {
    bool update_needed = false;
    CAP_ACT action = CAP_ACT_NOTHING;

    typedef enum {
        SEL_NONE,
        SEL_SHUTTER,
        SEL_AECOMP,
        SEL_APER,
        SEL_ISO,
        SEL_AF,
        SEL_AEM
    } ROTENC_SEL;

    // Capture screen internal states
    // A forced redraw should bring these back to correct states
    static bool sel_highlight = false;
    static ROTENC_SEL sel_item = SEL_NONE;

    bool need_highlight = false;
    ROTENC_SEL current_sel;

    // TODO: GUI logic!
    uint32_t btn = pal_input_get_keys();

    if (btn & KEY_MASK_FN1) {
        need_highlight = true;
        current_sel =
            (current_aem == AEM_P) ? SEL_AECOMP :
            (current_aem == AEM_A) ? SEL_AECOMP :
            (current_aem == AEM_S) ? SEL_AECOMP :
            (current_aem == AEM_M) ? SEL_APER : SEL_NONE;
    }
    else if (btn & KEY_MASK_FN2) {
        need_highlight = true;
        current_sel = SEL_ISO;
    }
    else if (btn & KEY_MASK_FN3) {
        need_highlight = true;
        current_sel = SEL_AEM;
    }
    else if (btn & KEY_MASK_FN4) {
        need_highlight = true;
        current_sel = SEL_AF;
    }
    else {
        need_highlight = false;
        current_sel =
            (current_aem == AEM_P) ? SEL_NONE :
            (current_aem == AEM_A) ? SEL_APER :
            (current_aem == AEM_S) ? SEL_SHUTTER :
            (current_aem == AEM_M) ? SEL_SHUTTER : SEL_NONE;
    }

    if ((need_highlight != sel_highlight) || (sel_item != current_sel)
            || redraw) {
        update_needed = true;
        if (need_highlight) {
            label_mode.specifics.label.fgcl = COLOR_DIM;
            label_shutter.specifics.label.fgcl = COLOR_DIM;
            label_aperture.specifics.label.fgcl = COLOR_DIM;
            label_iso.specifics.label.fgcl = COLOR_DIM;
            label_exposure.specifics.label.fgcl = COLOR_DIM;
            label_focus.specifics.label.fgcl = COLOR_DIM;
            switch (current_sel) {
            case SEL_SHUTTER:
                label_shutter.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            case SEL_AECOMP:
                label_exposure.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            case SEL_APER:
                label_aperture.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            case SEL_ISO:
                label_iso.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            case SEL_AF:
                label_focus.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            case SEL_AEM:
                label_mode.specifics.label.fgcl = COLOR_FOCUSED;
                break;
            }
        }
        else {
            label_mode.specifics.label.fgcl = COLOR_NORMAL;
            label_shutter.specifics.label.fgcl = COLOR_NORMAL;
            label_aperture.specifics.label.fgcl = COLOR_NORMAL;
            label_iso.specifics.label.fgcl = COLOR_NORMAL;
            label_exposure.specifics.label.fgcl = COLOR_NORMAL;
            label_focus.specifics.label.fgcl = COLOR_NORMAL;
        }
        sel_highlight = need_highlight;
        sel_item = current_sel;
    }

    int8_t rotenc = pal_input_get_encoder(0);
    bool update_labels = false;

    if (rotenc != 0) {
        int *sel_val = NULL;
        int sel_val_max;

        switch (current_sel) {
        case SEL_SHUTTER:
            sel_val = &current_shutter;
            sel_val_max = no_shut_setpoints - 1;
            break;
        case SEL_AECOMP:
            // Not implemented
            sel_val = NULL;
            break;
        case SEL_APER:
            // Not implemented
            sel_val = NULL;
            break;
        case SEL_ISO:
            sel_val = &current_iso;
            sel_val_max = no_iso_setpoints - 1;
            break;
        case SEL_AF:
            // Not implemented
            sel_val = NULL;
            break;
        case SEL_AEM:
            // Not implemented
            sel_val = NULL;
            break;
        }

        if (sel_val != NULL) {
            update_needed = true;
            update_labels = true;
            if (rotenc > 0) {
                if (*sel_val + rotenc >= sel_val_max) {
                    *sel_val = sel_val_max;
                }
                else {
                    *sel_val += rotenc;
                }
            }
            else {
                if (*sel_val + rotenc <= 0) {
                    *sel_val = 0;
                }
                else {
                    *sel_val += rotenc;
                }
            }
        }
    }

    if (update_labels || redraw) {
        // Update UI strings
        snprintf(label_shutter.specifics.label.string, UILIB_LABEL_MAXLEN,
                "%ss", shut_setpoints[current_shutter].name);
        snprintf(label_iso.specifics.label.string, UILIB_LABEL_MAXLEN,
                "ISO-%s", iso_setpoints[current_iso].name);

        // Apply settings as well
        pal_cam_set_gain(iso_setpoints[current_iso].cdsgain,
                iso_setpoints[current_iso].vgagain);
        pal_cam_set_preview_shutter_speed(
                shut_setpoints[current_shutter].draft_lines);
    }

    if (btn & KEY_MASK_SHUTTER) {
        action = CAP_ACT_CAPTURE;
    }

    if (btn & KEY_MASK_PEK_SHORT) {
        action = CAP_ACT_SHUTDOWN;
    }

    // Debug control
    static int sh_right = 0;
    static uint8_t shd[2] = {14, 06};
    static uint8_t shp[2] = {33, 35};
    // static uint8_t shd[2] = {13, 07};
    // static uint8_t shp[2] = {33, 33};
    bool update_debug1 = false;
    if (btn & KEY_MASK_FN5) {
        sh_right = !sh_right;
        update_debug1 = true;
    }

    if (btn & KEY_MASK_LEFT) {
        if (shd[sh_right] > 0) {
            shd[sh_right] --;
            update_debug1 = true;
        }
    }

    if (btn & KEY_MASK_RIGHT) {
        if (shd[sh_right] < 63) {
            shd[sh_right] ++;
            update_debug1 = true;
        }
    }

    if (btn & KEY_MASK_FN7) {
        if (shp[sh_right] > 0) {
            shp[sh_right] --;
            update_debug1 = true;
        }
    }

    if (btn & KEY_MASK_FN8) {
        if (shp[sh_right] < 63) {
            shp[sh_right] ++;
            update_debug1 = true;
        }
    }

    if (update_debug1 || redraw) {
        snprintf(label_debug1.specifics.label.string, UILIB_LABEL_MAXLEN,
                "%cL SHD %02d SHP %02d%cR SHD %02d SHP %02d",
                sh_right ? ' ' : '*', shd[0], shp[0],
                sh_right ? '*' : ' ', shd[1], shp[1]);
        pal_cam_set_shl(sh_right, shd[sh_right], shp[sh_right]);
        update_needed = true;
    }



    if (redraw || update_needed) {
        uilib_mark_update();
        // Draw need to be called
        uilib_draw(&capture_screen_drawlist);
    }
    else {
        // No update requested, but uilib draw need to be called on the full
        // drawlist anyway to ensure proper operation on dual buffered mode
        uilib_draw(&capture_screen_drawlist);
        // And update things that's always need to be updated
        uilib_mark_update();
        uilib_draw(&capture_screen_always_update_drawlist);
    }

    return action;
}

PB_ACT gui_run_playback_screen(bool redraw) {
    return PB_ACT_NOTHING;
}

SET_ACT gui_run_setting_screen(bool redraw) {
    return SET_ACT_NOTHING;
}

void gui_show_fatal(char *msg) {
    // Show fatal error message
    uint32_t *disp_buf = pal_disp_get_buffer();
    uilib_set_framebuffer((uint8_t *)disp_buf);
    //printf("Fatal: %s\n", msg);
    uilib_set_font(&font_16x26);
    uilib_draw_string(0, 0, UILIB_FB_WIDTH, "FATAL ERROR", 11, false, COLOR_RED,
            COLOR_BLACK);
    uilib_draw_string(0, 32, UILIB_FB_WIDTH, msg, 1024, false, COLOR_WHITE,
            COLOR_BLACK);
    pal_disp_return_buffer(disp_buf);
    while (1) {
        //TODO
        pal_input_scan();
    }
}
