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
#include <SDL.h>

#include "effusus.h"
#include "effusus_rawio.h"
#include "effusus_bin2raw.h"
#include "effusus_imageio.h"

#define CCD_H (2718)
#define CCD_W (4080)
#define SCR_H (CCD_H/2)
#define SCR_W (CCD_W/2)

#include <tiffio.h>

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
    bool   gui = false;
    bool   savepng = false;
    bool   savebayer = false;
    
    if(argc<2){
        printf("Usage: %s filename [--gui] [--savepng] [--savebayer] [--gamma=1.0]\n",argv[0]);
        return -1;
    }
    for(int i=2;i<argc;i++){
        if(strcmp(argv[i],"--savepng")==0){
            savepng = true;
        }
        if(strcmp(argv[i],"--savebayer")==0){
            savebayer = true;
        }
        if(strcmp(argv[i],"--gui")==0){
            gui = true;
        }
        if(strncmp(argv[i],"--gamma=",8)==0) {
            gamma = atof(argv[i]+8);
        }
    }
    const char* fn = argv[1];

    fprintf(stdout,"--------------------EFFUSUS--------------------\n");
    fprintf(stdout,"Gamma:    \t %f\n",gamma);
    fprintf(stdout,"GUI:      \t %s\n",gui?"true":"false");
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

    if(gui) {
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
    }

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
        // 16-bit per pixel, Little-Endian
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

    // Write to mosaic tiff
    // print libtiff version
    printf("LIBTIFF %s\n", TIFFGetVersion());
    TIFF* tiff = TIFFOpen("output.tiff","w");
    if(!tiff){
        fprintf(stderr,"Failed to open output.tiff for writing\n");
        return -1;
    }
    // Set TIFF tags
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH,  SCR_W*2);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, SCR_H*2);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_CFA);

    TIFFSetField(tiff, TIFFTAG_MAKE, "DummyMake");
    TIFFSetField(tiff, TIFFTAG_MODEL, "DummyModel");
    TIFFSetField(tiff, TIFFTAG_SOFTWARE, "DummySoftware");
    TIFFSetField(tiff, TIFFTAG_ORIGINALRAWFILENAME, 1, "DummyName.dng");
    TIFFSetField(tiff, TIFFTAG_UNIQUECAMERAMODEL, "DummyUniqueModel");
    TIFFSetField(tiff, TIFFTAG_IMAGEDESCRIPTION, "DummyImageDescription");
    TIFFSetField(tiff, TIFFTAG_COPYRIGHT, "DummyCopyright");
    TIFFSetField(tiff, TIFFTAG_DATETIME, "2016:06:30 11:11:15");
    TIFFSetField(tiff, TIFFTAG_DNGVERSION, "\01\01\00\00");
    TIFFSetField(tiff, TIFFTAG_SUBFILETYPE, 0);
    TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(tiff, TIFFTAG_DNGBACKWARDVERSION, "\01\00\00\00");
    
    // Custom TIFF tag for CFA pattern
    TIFFSetField(tiff, TIFFTAG_CFAPATTERN, 4, "\000\001\001\002"); // RGGB

    // Write bayer data
    tsize_t linebytes = SCR_W*4 * sizeof(uint8_t);
    uint8_t *buf = (uint8_t*)_TIFFmalloc(linebytes);
    for (uint32_t row = 0; row < SCR_H*2; row++) {
        // Copy data from bayerbuf to buf, big endian
        for (uint32_t col = 0; col < SCR_W*4; col+=2) {
            buf[col]   = bayerbuf[row*SCR_W*4+col];
            buf[col+1] = bayerbuf[row*SCR_W*4+col+1];
        }
        if (TIFFWriteScanline(tiff, buf, row, 0) < 0) {
            fprintf(stderr, "Failed to write TIFF scanline\n");
            break;
        }
    }
    _TIFFfree(buf);
    //

    // Close TIFF
    TIFFClose(tiff);

    free(bayerbuf);
    free(raw);
    if(gui)	SDL_Quit();
    printf("Done!\n");
    
    return 0;
}