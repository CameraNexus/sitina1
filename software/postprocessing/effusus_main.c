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
#include "math.h"
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
    double gamma = 1.0;
    if(argc<2){
        printf("Usage: %s filename [--savepng] [--gamma=1.0]\n",argv[0]);
        return -1;
    }
    if(argc>2){
        for(int i=2;i<argc;i++){
            if(strcmp(argv[i],"--savepng")==0) {
                printf("Option: Save PNG Enabled\n");
                savepng = true;
            }
            if(strncmp(argv[i],"--gamma=",8)==0) {
                gamma = atof(argv[i]+8);
                printf("Option: Gamma Correction Enabled, gamma=%f\n",gamma);
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

    
    // Render to Screen
    // This is lossy, expect to see banding when dithering is disabled
    #define DITHER_FLSTEIN

    #if defined(DITHER_FLSTEIN)
    // Floyd-Steinberg
    double kernel[] = {
        0,        0,        0,
        0,        0,        7.0/16.0,
        3.0/16.0, 5.0/16.0, 1.0/16.0
    };
    printf("Using Floyd-Steinberg Dithering\n");
    #else
    double kernel[] = {
        0,        0,        0,
        0,        0,        0,
        0,        0,        0
    };
    #endif
    
    uint16_t* r_raw;
    uint16_t* g_raw;
    uint16_t* b_raw;
    uint16_t* r_q;
    uint16_t* g_q;
    uint16_t* b_q;
    r_raw = (uint16_t*)malloc(2*SCR_H*SCR_W);
    g_raw = (uint16_t*)malloc(2*SCR_H*SCR_W);
    b_raw = (uint16_t*)malloc(2*SCR_H*SCR_W);
    r_q   = (uint16_t*)malloc(2*SCR_H*SCR_W);
    g_q   = (uint16_t*)malloc(2*SCR_H*SCR_W);
    b_q   = (uint16_t*)malloc(2*SCR_H*SCR_W);

    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            uint16_t r  = raw[4*(y*SCR_W+x)+0];
            uint16_t g1 = raw[4*(y*SCR_W+x)+1];
            uint16_t g2 = raw[4*(y*SCR_W+x)+3];
            uint16_t b  = raw[4*(y*SCR_W+x)+2];
            uint16_t g  = (g1>>1)+(g2>>1);
            r_raw[y*SCR_W+x] = r;
            g_raw[y*SCR_W+x] = g;
            b_raw[y*SCR_W+x] = b;
        }
    }
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            // Quantize
            r_q[y*SCR_W+x] = r_raw[y*SCR_W+x]&0xF800;
            g_q[y*SCR_W+x] = g_raw[y*SCR_W+x]&0xF800;
            b_q[y*SCR_W+x] = b_raw[y*SCR_W+x]&0xF800;
            // Diffuse Error
            int r_err = r_raw[y*SCR_W+x] - r_q[y*SCR_W+x];
            int g_err = g_raw[y*SCR_W+x] - g_q[y*SCR_W+x];
            int b_err = b_raw[y*SCR_W+x] - b_q[y*SCR_W+x];
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                    int dx = i-1;
                    int dy = j-1;
                    if(x+dx>=0 && x+dx<SCR_W && y+dy>=0 && y+dy<SCR_H){
                        r_raw[(y+dy)*SCR_W+(x+dx)] += r_err*kernel[j*3+i];
                        g_raw[(y+dy)*SCR_W+(x+dx)] += g_err*kernel[j*3+i];
                        b_raw[(y+dy)*SCR_W+(x+dx)] += b_err*kernel[j*3+i];
                    }
                }
            }
            // gamma correction for preview
            double delta = 63487.0;
            uint8_t r_qg  = (uint8_t)(pow((double)r_q[y*SCR_W+x] /delta,1.0/gamma)*31.0);
            uint8_t g_qg  = (uint8_t)(pow((double)g_q[y*SCR_W+x] /delta,1.0/gamma)*31.0);
            uint8_t b_qg  = (uint8_t)(pow((double)b_q[y*SCR_W+x] /delta,1.0/gamma)*31.0);

            // Writing to FB
            vram[y*SCR_W+x] = (r_qg<<11)&0xF800 | (g_qg<<6)&0x07E0 | b_qg&0x001F;
        }
    }

    free(r_q);
    free(g_q);
    free(b_q);
    free(r_raw);
    free(g_raw);
    free(b_raw);
    

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
    if(savepng){

        // Save the four channels as separate PNG files.
        uint16_t* raw_rgb = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
        for(int y=0;y<SCR_H;y++){
            for(int x=0;x<SCR_W;x++){
                    int ptr3 = 3*(y*SCR_W+x);
                    int ptr4 = 4*(y*SCR_W+x);
                    raw_rgb[ptr3+0] = raw[ptr4+0];
                    raw_rgb[ptr3+1] = (raw[ptr4+1]>>1)+(raw[ptr4+3]>>1);
                    raw_rgb[ptr3+2] = raw[ptr4+2];
            }
        }
        printf("Saving PNG...\n");
        write_png("output.png", raw_rgb);
        free(raw_rgb);
    }

    free(raw);
	SDL_Quit();
    printf("Done!\n");
    printf("Gapsize: %ld, Gapshift: %ld\n", gapsize, gapshift);
    return 0;
}