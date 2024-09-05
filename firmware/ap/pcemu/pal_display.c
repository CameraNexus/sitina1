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
#include <assert.h>
#include "pal_display.h"

static SDL_Surface *screen = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Rect textureRect;

// Only exist on this implementation, not part of public API
uint32_t pal_disp_fb[DISP_WIDTH * DISP_HEIGHT];

void pal_disp_init(void) {
    window = SDL_CreateWindow("Sitina",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            DISP_WIDTH + AOND_WIDTH, DISP_HEIGHT, SDL_SWSURFACE);

    if (window == NULL) {
        fprintf(stderr, "Unable to create window\n");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        fprintf(stderr, "Unable to create renderer\n");
        return;
    }

    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, DISP_WIDTH + AOND_WIDTH,
            DISP_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    textureRect.x = textureRect.y = 0;
    textureRect.w = DISP_WIDTH + AOND_WIDTH;
    textureRect.h = DISP_HEIGHT;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, DISP_WIDTH + AOND_WIDTH, DISP_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    if (screen == NULL || texture == NULL) {
        fprintf(stderr, "Unable to allocate framebuffer or texture\n");
        return;
    }

    memset(pal_disp_fb, 0, sizeof(pal_disp_fb));
    pal_disp_return_buffer(pal_disp_fb);
}

void pal_disp_deinit(void) {
    if (screen != NULL) {
        SDL_FreeSurface(screen);
    }

    if (texture) {
	    SDL_DestroyTexture(texture);
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }
}

void pal_disp_enter_sleep(void) {
    printf("pal_disp: enter sleep\n");
}

void pal_disp_exit_sleep(void) {
    printf("pal_disp: exit sleep\n");
}

uint32_t *pal_disp_get_buffer(void) {
    return (uint32_t *)pal_disp_fb;
}

static void pal_disp_render_copy(void) {
	void *texturePixels;
	int texturePitch;

	SDL_LockTexture(texture, NULL, &texturePixels, &texturePitch);
	memset(texturePixels, 0, textureRect.y * texturePitch);
	uint8_t *pixels = (uint8_t *)texturePixels + textureRect.y * texturePitch;
	uint8_t *src = (uint8_t *)screen->pixels;
	int leftPitch = textureRect.x * 4;
	int rightPitch = texturePitch - ((textureRect.x + textureRect.w) * 4);
	for (int y = 0; y < textureRect.h; y++, src += screen->pitch) {
		memset(pixels, 0, leftPitch); pixels += leftPitch;
		memcpy(pixels, src, (DISP_WIDTH + AOND_WIDTH) * 4);
        pixels += (DISP_WIDTH + AOND_WIDTH) * 4;
		memset(pixels, 0, rightPitch); pixels += rightPitch;
	}
	memset(pixels, 0, textureRect.y * texturePitch);
	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void pal_disp_return_buffer(uint32_t *buf) {
    assert((uint32_t *)pal_disp_fb == buf);

    // First copy fb into screen buffer
    uint32_t *rdptr = buf + DISP_HEIGHT * DISP_WIDTH;
    uint32_t *wrptr = screen->pixels;
    for (int y = 0; y < DISP_HEIGHT; y++) {
        for (int x = 0; x < DISP_WIDTH; x++) {
            *wrptr++ = *rdptr--;
        }
        wrptr += AOND_WIDTH;
    }

    pal_disp_render_copy();
}

void pal_disp_update_aondisp(uint8_t *buf) {
    uint8_t *rdptr = buf;
    uint32_t *wrptr = screen->pixels;
    wrptr += DISP_WIDTH;
    for (int y = 0; y < AOND_HEIGHT; y++) {
        for (int x = 0; x < AOND_WIDTH / 8; x++) {
            uint8_t pix = *rdptr++;
            for (int z = 0; z < 8; z++) {
                uint32_t cl = (pix & 0x1) ? 0xFFBBBBBB : 0xFF111111;
                *wrptr++ = cl;
                pix >>= 1;
            }
        }
        wrptr += DISP_WIDTH;
    }

    pal_disp_render_copy();
}
