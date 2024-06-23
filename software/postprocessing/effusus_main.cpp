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
// File : effusus_main.cpp
// Brief: Raw CCD Data Post-Processing - PC Main

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <SDL/SDL.h>
#include <libraw/libraw.h>

#include "effusus.h"
#include "effusus_rawio.h"
#include "effusus_bin2raw.h"
#include "effusus_imageio.h"

#define CCD_H (2725)
#define CCD_W (4416)
#define SCR_H (CCD_H/2)
#define SCR_W (CCD_W/2)

#include <png.h>

#define PNG_WIDTH (SCR_W)
#define PNG_HEIGHT (SCR_H)

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

int main(int argc, char** argv) {
    double gamma = 1.0;
    bool   savepng = false;
    bool   savebayer = false;
    
    if(argc<2){
        printf("Usage: %s filename [--savepng] [--savebayer] [--gamma=1.0]\n",argv[0]);
        return -1;
    }
    for(int i=2;i<argc;i++){
        if(strcmp(argv[i],"--savepng")==0){
            savepng = true;
        }
        if(strcmp(argv[i],"--savebayer")==0){
            savebayer = true;
        }
        if(strncmp(argv[i],"--gamma=",8)==0) {
            gamma = atof(argv[i]+8);
        }
    }
    const char* fn = argv[1];

    fprintf(stdout,"--------------------EFFUSUS--------------------\n");
    fprintf(stdout,"Gamma:    \t %f\n",gamma);
    fprintf(stdout,"Save PNG: \t %s\n",savepng?"true":"false");
    fprintf(stdout,"Save Bayer:\t %s\n",savebayer?"true":"false");
    fprintf(stdout,"Filename: \t %s\n",fn);
    fprintf(stdout,"-----------------------------------------------\n");
    // Flush printf
    fflush(stdout);

    // Load binary
    uint8_t* fbuf = (uint8_t*)malloc((CCD_H+1)*CCD_W*2);
    long flen = read_binary_file(fn, fbuf, (CCD_H+1)*CCD_W*2);
    if(flen==-1) return -1;
    printf("Read Successful, flen=%ld\n",flen);

    // Convert binary to rggb raw data
    uint16_t* raw = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*4);
    memset(raw,0xFF,sizeof(raw));
    effusus_bin2raw(raw, fbuf, CCD_W, CCD_H, SCR_W, SCR_H);

    // Init SDL
	SDL_Init(SDL_INIT_VIDEO);
	swin = SDL_SetVideoMode(SCR_W/2, SCR_H/2, 16, false);
    semu = SDL_CreateRGBSurface(SDL_SWSURFACE,SCR_W, SCR_H,16,0,0,0,0);
    if ((semu == NULL) || (swin == NULL)) {
        fprintf(stderr, "could not create surface: %s\n", SDL_GetError());
        return -1;
    }
    uint16_t* vram = (uint16_t*)(semu->pixels);

    // Dither to VRAM
    effusus_dither_to_vram(vram, raw, gamma, SCR_W, SCR_H, EFFUSUS_DITHER_FS);
    
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

    unsigned bayerbuf_size = SCR_W*SCR_H*4*2;
    uint8_t* bayerbuf;
    bayerbuf = (uint8_t*)malloc(bayerbuf_size);
    memset(bayerbuf,0x00,bayerbuf_size);
    effusus_raw2bayer(raw, bayerbuf, SCR_W, SCR_H);
    if(savebayer){
        // Store bayer raw to binary file
        // This will result in the following image format:
        // 4416 x 2726 RGGB(2x2)
        // Row-Stride = 8832, Pixel-Stride = 4 
        // 16-bit per pixel, Big-Endian
        //
        // The file can be opened by GIMP, RawTherapee, or LibRaw
        FILE* fbayer = fopen("bayer.raw","wb");
        fwrite(bayerbuf,1,bayerbuf_size,fbayer);
        fclose(fbayer);
        PRINTINFO("Bayer RAW saved to bayer.raw\n");
    }

    if(savepng){
        // Store raw to PNG
        effusus_writepng("output.png", raw, SCR_W, SCR_H);
    }

    // Init libraw    
    LibRaw rp;
    rp.imgdata.params.output_tiff = 1;
    rp.imgdata.params.output_bps  = 16;
    int ret = rp.open_bayer(\
        bayerbuf, bayerbuf_size, 
        SCR_W, SCR_H, 
        0, 0, 0, 0, 
        1, LIBRAW_OPENBAYER_RGGB, 0, 0, 1400);
        
    if (ret != LIBRAW_SUCCESS) return -1;
    if ((ret = rp.unpack()) != LIBRAW_SUCCESS)
        printf("Unpack error: %d\n", ret);

    if ((ret = rp.dcraw_process()) != LIBRAW_SUCCESS)
        printf("Processing error: %d\n", ret);

    char outfn[256];
    sprintf(outfn, "%s.tif", fn);
    if (LIBRAW_SUCCESS != (ret = rp.dcraw_ppm_tiff_writer(outfn)))
        printf("Cannot write %s: %s\n", outfn, libraw_strerror(ret));
    else
        printf("Created %s\n", outfn);

quit:
    free(raw);
	SDL_Quit();
    printf("Done!\n");
    
    return 0;
}