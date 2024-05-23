//
// Sitina simulator
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
#include <string.h>
#include "dispsim.h"
#include <SDL.h>

static int x;
static int y;
static int last_hs;

//#define USE_DBG_OUTPUT

void render_copy(); // Function in main.cpp

void dispsim_reset() {
    x = 0;
    y = 0;
    last_hs = 0;
}

void dispsim_render() {
    static float time_delta = 0.0f;
    static int last_ticks = SDL_GetTicks();
    const float TARGET_FPS = 60.0f;

    int cur_ticks = SDL_GetTicks();
    time_delta -= cur_ticks - last_ticks; // Actual ticks passed since last iteration
    time_delta += 1000.0f / TARGET_FPS; // Time allocated for this iteration
    last_ticks = cur_ticks;

    render_copy();

    int time_to_wait = time_delta - (SDL_GetTicks() - last_ticks);
    if (time_to_wait > 0)
        SDL_Delay(time_to_wait);
}

void dispsim_apply(uint32_t *pixels, const uint8_t vs, const uint8_t hs,
        const uint8_t de, const uint32_t data) {
    static bool line_valid;

    if (!last_hs && hs) {
        x = 0;
        if (vs) {
            y = 0;
            line_valid = false;
            dispsim_render();
        }
        else {
            if (line_valid)
                y++;
        }
    }

    if (de) {
        if ((y < DISP_HEIGHT) && (x < DISP_WIDTH)) {
            pixels[y * DISP_WIDTH + x] = (data & 0xfffffful) | 0xff000000ul;
        }
        x++;
        line_valid = true;
    }

    last_hs = hs;
}
