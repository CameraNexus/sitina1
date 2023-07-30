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
#include "os_display.h"

static SDL_Surface *screen = NULL;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Rect textureRect;

void os_disp_init() {
    window = SDL_CreateWindow("Sitina", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            DISP_WIDTH, DISP_HEIGHT, SDL_SWSURFACE);

    if (window == NULL) {
        fprintf(stderr, "Unable to create window\n");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        fprintf(stderr, "Unable to create renderer\n");
        return;
    }

    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, DISP_WIDTH, DISP_HEIGHT, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    textureRect.x = textureRect.y = 0;
    textureRect.w = DISP_WIDTH; 
    textureRect.h = DISP_HEIGHT;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, DISP_WIDTH, DISP_HEIGHT);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    if (screen == NULL || texture == NULL)
    {
        fprintf(stderr, "Unable to allocate framebuffer or texture\n");
        return;
    }

    SDL_FillRect(screen, &textureRect, 0xFF0000FF);
    os_disp_return_buffer(screen->pixels);
}

void os_disp_deinit() {
    if (screen != NULL)
    {
        SDL_FreeSurface(screen);
    }

    if (texture)
    {
	    SDL_DestroyTexture(texture);
    }

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    if (window)
    {
        SDL_DestroyWindow(window);
    }
}

void os_disp_enter_sleep() {
    printf("os_disp: enter sleep\n");
}

void os_disp_exit_sleep() {
    printf("os_disp: exit sleep\n");
}

uint32_t *os_disp_get_buffer() {
    return screen->pixels;
}

void os_disp_return_buffer(uint32_t *buf) {
    assert(screen->pixels == buf);

	void *texturePixels;
	int texturePitch;

	SDL_LockTexture(texture, NULL, &texturePixels, &texturePitch);
	memset(texturePixels, 0, textureRect.y * texturePitch);
	uint8_t *pixels = (uint8_t *)texturePixels + textureRect.y * texturePitch;
	uint8_t *src = (uint8_t *)screen->pixels;
	int leftPitch = textureRect.x << 2;
	int rightPitch = texturePitch - ((textureRect.x + textureRect.w) << 2);
	for (int y = 0; y < textureRect.h; y++, src += screen->pitch)
	{
		memset(pixels, 0, leftPitch); pixels += leftPitch;
		memcpy(pixels, src, DISP_WIDTH << 2); pixels += DISP_WIDTH << 2;
		memset(pixels, 0, rightPitch); pixels += rightPitch;
	}
	memset(pixels, 0, textureRect.y * texturePitch);
	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
