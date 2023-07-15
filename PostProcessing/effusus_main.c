//
// SitinaNe
// Copyright 2023 Wenting Zhang, Anhang Li
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
// File : effusus_rawio.h
// Brief: Raw CCD Data Post-Processing - PC Main

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define CCD_H (2725)
#define CCD_W (4416)
#define SCR_H (CCD_H/2)
#define SCR_W (CCD_W/2)

#include "effusus.h"
uint8_t fbuf[(CCD_H+1)*CCD_W*2];
SDL_Surface* swin;
SDL_Surface* semu;
SDL_Rect rsrc = {
    .x=0,
    .y=0,
    .w=SCR_W,
    .h=SCR_H
};        
SDL_Rect rdst = {
    .x=0,
    .y=0,
    .w=SCR_W,
    .h=SCR_H
};

int main (int argc, char** argv){
    if(argc!=2){
        printf("Usage: %s filename\n",argv[0]);
        return -1;
    }
    const char* fn = argv[1];
    long flen = read_binary_file(fn, fbuf, sizeof(fbuf));
    if(flen==-1) return -1;
    printf("Read Successful, flen=%ld\n",flen);
    
	SDL_Init(SDL_INIT_VIDEO);
	swin = SDL_SetVideoMode(SCR_W, SCR_H, 16, false);
    semu = SDL_CreateRGBSurface(SDL_SWSURFACE,SCR_W, SCR_H,16,0,0,0,0);
    if ((semu == NULL) || (swin == NULL)) {
        fprintf(stderr, "could not create surface: %s\n", SDL_GetError());
        return -1;
    }
    // Render to SDL buffer
    
    const long bias = CCD_W;
    long gapsize  = 176;    // Dirty Hack
    long gapshift = 1191;
    uint16_t* vram = (uint16_t*)(semu->pixels);

    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            uint8_t l, h;
            uint16_t r, g1, g2, g, b;
            r=0;g=0;b=0;
            #define MAKE16(a8) (fbuf[(a8)*2+bias+1]<<8|fbuf[(a8)*2+bias])
            if(x&0x01){
                g1  = MAKE16((y+1)*2*CCD_W+((x+1)*2+1))>>(16-5);
                b   = MAKE16( y   *2*CCD_W+((x+1)*2+1))>>(16-5);
                g2  = MAKE16( y   *2*CCD_W+( x   *2+1))>>(16-5);
                r   = MAKE16((y+1)*2*CCD_W+( x   *2+1))>>(16-5);
                g   = g1+g2;
                vram[y*SCR_W+x/2] = (r<<11)&0xF800 | (g<<5)&0x07E0 | b&0x001F;
            } else {
                g1 = MAKE16( y   *2*CCD_W+( x   *2))>>(16-5);
                b  = MAKE16( y   *2*CCD_W+((x+1)*2))>>(16-5);
                g2 = MAKE16((y+1)*2*CCD_W+((x+1)*2))>>(16-5);
                r  = MAKE16((y+1)*2*CCD_W+( x   *2))>>(16-5);
                g = g1+g2;
                vram[y*SCR_W+(SCR_W-x/2)] = (r<<11)&0xF800 | (g<<5)&0x07E0 | b&0x001F;
            }
        }
    }
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            if(x>gapshift) vram[y*SCR_W+x-gapsize] = vram[y*SCR_W+x];
        }
    }

    SDL_Event event;
	bool running = true;
    while(running){
		while(SDL_PollEvent(&event)){
			running = event.type != SDL_QUIT;
		}
        switch( event.type ){
            /* Look for a keypress */
            case SDL_KEYDOWN:
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ){
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SoftStretch(semu,&rsrc,swin,&rdst);
		SDL_Flip(swin);
	}

	SDL_Quit();
    printf("Gapsize: %d, Gapshift: %d", gapsize, gapshift);
    printf("Done!\n");
    return 0;
}