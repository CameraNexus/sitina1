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

#define SAVE_PNG
#include <png.h>

#define PNG_WIDTH (SCR_W)
#define PNG_HEIGHT (SCR_H)

void write_png(const char* filename, uint16_t* data) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file %s for writing\n", filename);
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Failed to create PNG write struct\n");
        fclose(file);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Failed to create PNG info struct\n");
        png_destroy_write_struct(&png, NULL);
        fclose(file);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr, "Failed to setjmp\n");
        png_destroy_write_struct(&png, &info);
        fclose(file);
        return;
    }

    png_init_io(png, file);

    png_set_IHDR(png, info, PNG_WIDTH, PNG_HEIGHT, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png, info);

    png_bytep row = (png_bytep)malloc(3 * PNG_WIDTH * sizeof(png_byte));
    for (int y = 0; y < PNG_HEIGHT; y++) {
        for (int x = 0; x < PNG_WIDTH; x++) {
            /*
            uint16_t pixel = data[y * PNG_WIDTH + x];
            row[3 * x + 0] = (pixel & 0xF800) >> 8;  // Red component
            row[3 * x + 1] = (pixel & 0x07E0) >> 3;  // Green component
            row[3 * x + 2] = (pixel & 0x001F) << 3;  // Blue component
            */
            row [3*x+0] = data[3*(y*PNG_WIDTH+x)+0]>>8;
            row [3*x+1] = data[3*(y*PNG_WIDTH+x)+1]>>8;
            row [3*x+2] = data[3*(y*PNG_WIDTH+x)+2]>>8;
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);

    free(row);

    png_destroy_write_struct(&png, &info);
    fclose(file);
    printf("%s saved\n",filename);
}

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
    .w=SCR_W/2,
    .h=SCR_H/2
};

bool savepng = false;
int main (int argc, char** argv){
    if(argc<2){
        printf("Usage: %s filename [--savepng]\n",argv[0]);
        return -1;
    }
    if(argc>2){
        for(int i=2;i<argc;i++){
            if(strcmp(argv[i],"--savepng")==0) {
                printf("Option: Save PNG Enabled\n");
                savepng = true;
            }
        }
    }
    const char* fn = argv[1];
    long flen = read_binary_file(fn, fbuf, sizeof(fbuf));
    if(flen==-1) return -1;
    printf("Read Successful, flen=%ld\n",flen);
    
	SDL_Init(SDL_INIT_VIDEO);
	swin = SDL_SetVideoMode(SCR_W/2, SCR_H/2, 16, false);
    semu = SDL_CreateRGBSurface(SDL_SWSURFACE,SCR_W, SCR_H,16,0,0,0,0);
    if ((semu == NULL) || (swin == NULL)) {
        fprintf(stderr, "could not create surface: %s\n", SDL_GetError());
        return -1;
    }
    // Render to SDL buffer
    
    const long bias = 768;
    long gapsize  = 175;    // Dirty Hack
    long gapshift = 1191;
    uint16_t* vram = (uint16_t*)(semu->pixels);
    uint16_t* raw = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*4);
    memset(raw,0xFF,sizeof(raw));
    
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            uint8_t l, h;
            uint16_t r, g1, g2, g, b;
            int wrptr;
            r=0;g=0;b=0;
            #define MAKE16(a8) (fbuf[(a8)*2+bias+1]<<8|fbuf[(a8)*2+bias])
            // Pixel Format:
            // +----+----+
            // | B  | G1 |
            // +----+----+
            // | G2 | R  |
            // +----+----+
            //
            if(x&0x01){
                b  = MAKE16((y*2+1)*CCD_W+((x+1)*2+1));
                g2 = MAKE16( y*2   *CCD_W+((x+1)*2+1));
                r  = MAKE16( y*2   *CCD_W+( x   *2+1));
                g1 = MAKE16((y*2+1)*CCD_W+( x   *2+1));
                wrptr = 4*(y*SCR_W+x/2);
                
            } else {
                r   = MAKE16( y   *2*CCD_W+( x   *2));
                g2  = MAKE16( y   *2*CCD_W+((x+1)*2));
                b   = MAKE16((y*2+1)*CCD_W+((x+1)*2));
                g1  = MAKE16((y*2+1)*CCD_W+( x   *2));
                wrptr = 4*(y*SCR_W+(SCR_W-x/2));
            }
            raw[wrptr+0] = r;
            raw[wrptr+1] = g1;
            raw[wrptr+2] = b;
            raw[wrptr+3] = g2;
        }
    }
    
    // Remove the gap from the output image
    
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            if(x>gapshift) {
                // vram[y*SCR_W+x-gapsize] = vram[y*SCR_W+x];
                raw[4*(y*SCR_W+x-gapsize)+0] = raw[4*(y*SCR_W+x)+0]; // r
                raw[4*(y*SCR_W+x-gapsize)+1] = raw[4*(y*SCR_W+x)+1]; // g1
                raw[4*(y*SCR_W+x-gapsize-1)+2] = raw[4*(y*SCR_W+x)+2]; // b
                raw[4*(y*SCR_W+x-gapsize-1)+3] = raw[4*(y*SCR_W+x)+3]; // g2
            }
        }
    }
    
    // Copy to Screen
    // This is lossy, expect to see banding. (Although the image output shouldn't have any banding)
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            vram[y*SCR_W+x] = ((raw[4*(y*SCR_W+x)+0]>>(16-5))<<11)&0xF800 | ((raw[4*(y*SCR_W+x)+1]>>(16-5))<<6)&0x07E0 | raw[4*(y*SCR_W+x)+2]>>(16-5)&0x001F;
        }
    }


    SDL_Event event;
	bool running = true;
    
    while(running){
		while(SDL_PollEvent(&event)){
			running = event.type != SDL_QUIT;
		}
        switch( event.type ){
            case SDL_KEYDOWN:
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
    

    SDL_SoftStretch(semu,&rsrc,swin,&rdst);
	SDL_Flip(swin);

    // Save the four channels as separate PNG files.
    uint16_t* raw_rgb = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
    uint16_t* raw_r   = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
    uint16_t* raw_g1  = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
    uint16_t* raw_b   = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
    uint16_t* raw_g2  = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
    memset(raw_r,0,sizeof(raw_r));
    memset(raw_r,0,sizeof(raw_g1));
    memset(raw_r,0,sizeof(raw_b));
    memset(raw_r,0,sizeof(raw_g2));
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
                int ptr3 = 3*(y*SCR_W+x);
                int ptr4 = 4*(y*SCR_W+x);
                raw_rgb[ptr3+0] = raw[ptr4+0];
                raw_rgb[ptr3+1] = (raw[ptr4+1]>>1)+(raw[ptr4+3]>>1);
                raw_rgb[ptr3+2] = raw[ptr4+2];
                raw_r  [ptr3+0] = raw[ptr4+0];
                raw_g1 [ptr3+1] = raw[ptr4+1];
                raw_b  [ptr3+2] = raw[ptr4+2];
                raw_g2 [ptr3+1] = raw[ptr4+3];
        }
    }

    if(savepng){
        printf("Saving PNG...\n");
        write_png("output.png", raw_rgb);
        write_png("output_r.png", raw_r);
        write_png("output_g1.png", raw_g1);
        write_png("output_b.png", raw_b);
        write_png("output_g2.png", raw_g2);
    }
    free(raw);
    free(raw_rgb);
    free(raw_r);
    free(raw_g1);
    free(raw_b);
    free(raw_g2);
	SDL_Quit();
    printf("Done!\n");
    printf("Gapsize: %ld, Gapshift: %ld\n", gapsize, gapshift);
    return 0;
}