//
// Sitina1
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
#include <SDL.h>
#include <stdbool.h>
#include "pal_input.h"

uint32_t key_state;
int encoder_count;

void pal_input_init(void) {
    key_state = 0;
    encoder_count = 0;
}

void pal_input_deinit(void) {

}

uint32_t pal_input_get_keys(void) {
    return key_state;
}

static void _pal_input_handle_sdl_keyevent(SDL_KeyboardEvent event) {
    uint32_t key_mask;

    switch (event.keysym.sym) {
    case SDLK_LEFT:
        key_mask = KEY_MASK_LEFT;
        break;
    case SDLK_RIGHT:
        key_mask = KEY_MASK_RIGHT;
        break;
    case SDLK_UP:
        key_mask = KEY_MASK_UP;
        break;
    case SDLK_DOWN:
        key_mask = KEY_MASK_DOWN;
        break;
    case SDLK_q:
        key_mask = KEY_MASK_FN1;
        break;
    case SDLK_w:
        key_mask = KEY_MASK_FN2;
        break;
    case SDLK_e:
        key_mask = KEY_MASK_FN3;
        break;
    case SDLK_r:
        key_mask = KEY_MASK_FN4;
        break;
    case SDLK_z:
        key_mask = KEY_MASK_FN5;
        break;
    case SDLK_x:
        key_mask = KEY_MASK_FN6;
        break;
    case SDLK_o:
        encoder_count--;
        break;
    case SDLK_p:
        encoder_count++;
        break;
    default:
        // Ignore
        return;
    }

    if (event.type == SDL_KEYDOWN) {
        key_state |= key_mask;
    }
    else {
        key_state &= ~key_mask;
    }
}

int pal_input_get_encoder(uint32_t id) {
    if (id != 0)
        return 0;
    return encoder_count;
}

void pal_input_scan(void) {
    SDL_Event event;

    if (SDL_PollEvent(&event)) {
        switch (event.type)
        {
        case SDL_QUIT:
            printf("Exiting...\n");
            exit(0);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            _pal_input_handle_sdl_keyevent(event.key);
            break;

        default:
            break;
        }
    }
}
