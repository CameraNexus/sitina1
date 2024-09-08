#include "effusus_bin2raw.h"
#include <stdio.h>
#include <malloc.h>

// TODO: remove the magic numbers
uint16_t* effusus_bin2raw(uint16_t* rawbuf, uint8_t* binbuf, int CCD_W, int CCD_H, int SCR_W, int SCR_H){
    const long bias = 0;
    long gapsize  = 8;    // Dirty Hack
    long gapshift = 1023;   // Dirty Hack
    
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            uint8_t l, h;
            uint16_t r, g1, g2, g, b;
            int wrptr;
            r=0;g=0;b=0;
            #define MAKE16(a8) (((uint16_t)binbuf[(a8)*2+bias+1])<<8|((uint16_t)binbuf[(a8)*2+bias]))
            // Pixel Format:
            // +----+----+
            // | B  | G1 |
            // +----+----+
            // | G2 | R  |
            // +----+----+
            //

            // Pixel Format:
            // +----+----+
            // | G1 | B  |
            // +----+----+
            // | R  | G2 |
            // +----+----+
            //
            if(x&0x01){
                g1 = MAKE16((y*2+1)*CCD_W+((x+1)*2+1));
                r  = MAKE16( y*2   *CCD_W+((x+1)*2+1));
                g2 = MAKE16( y*2   *CCD_W+( x   *2+1));
                b  = MAKE16((y*2+1)*CCD_W+( x   *2+1));
                wrptr = 4*(y*SCR_W+x/2);
                
            } else {
                g2  = MAKE16( y   *2*CCD_W+( x   *2));
                r   = MAKE16( y   *2*CCD_W+((x+1)*2));
                g1  = MAKE16((y*2+1)*CCD_W+((x+1)*2));
                b   = MAKE16((y*2+1)*CCD_W+( x   *2));
                wrptr = 4*(y*SCR_W+(SCR_W-x/2));
            }
            rawbuf[wrptr+0] = r;
            rawbuf[wrptr+1] = g1;
            rawbuf[wrptr+2] = b;
            rawbuf[wrptr+3] = g2;
        }
    }
    
    // Remove the gap from the output image
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            if(x>gapshift) {
                rawbuf[4*(y*SCR_W+x-gapsize)+0]   = rawbuf[4*(y*SCR_W+x)+0]; // r
                rawbuf[4*(y*SCR_W+x-gapsize)+1]   = rawbuf[4*(y*SCR_W+x)+1]; // g1
                rawbuf[4*(y*SCR_W+x-gapsize-1)+2] = rawbuf[4*(y*SCR_W+x)+2]; // b
                rawbuf[4*(y*SCR_W+x-gapsize-1)+3] = rawbuf[4*(y*SCR_W+x)+3]; // g2
            }
        }
    }
    PRINTINFO("Gapsize: %ld, Gapshift: %ld\n", gapsize, gapshift);
    return rawbuf;
}

uint8_t* effusus_raw2bayer(uint16_t* rawbuf, uint8_t* bayerbuf, int SCR_W, int SCR_H){
    // Unpack to Bayer
    uint16_t* r_raw;
    uint16_t* g1_raw;
    uint16_t* g2_raw;
    uint16_t* b_raw;
    
    r_raw  = (uint16_t*)malloc(SCR_W*SCR_H*2);
    g1_raw = (uint16_t*)malloc(SCR_W*SCR_H*2);
    g2_raw = (uint16_t*)malloc(SCR_W*SCR_H*2);
    b_raw  = (uint16_t*)malloc(SCR_W*SCR_H*2);
    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            r_raw [y*SCR_W+x] = rawbuf[4*(y*SCR_W+x)+0];
            g1_raw[y*SCR_W+x] = rawbuf[4*(y*SCR_W+x)+1];
            b_raw [y*SCR_W+x] = rawbuf[4*(y*SCR_W+x)+2];
            g2_raw[y*SCR_W+x] = rawbuf[4*(y*SCR_W+x)+3];
        }
    }

    for (int y = 0; y < SCR_H; y++) {
        for (int x = 0; x < SCR_W; x++) {
            bayerbuf[4*((2*y)*SCR_W+x)+1] = r_raw [y*SCR_W+x]>>8;
            bayerbuf[4*((2*y)*SCR_W+x)+0] = r_raw [y*SCR_W+x]&0xFF;
            bayerbuf[4*((2*y)*SCR_W+x)+3] = g2_raw[y*SCR_W+x]>>8;
            bayerbuf[4*((2*y)*SCR_W+x)+2] = g2_raw[y*SCR_W+x]&0xFF;
        }
        for (int x = 0; x < SCR_W; x++) {
            bayerbuf[4*((2*y+1)*SCR_W+x)+1] = g1_raw[y*SCR_W+x]>>8;
            bayerbuf[4*((2*y+1)*SCR_W+x)+0] = g1_raw[y*SCR_W+x]&0xFF;
            bayerbuf[4*((2*y+1)*SCR_W+x)+3] = b_raw [y*SCR_W+x]>>8;
            bayerbuf[4*((2*y+1)*SCR_W+x)+2] = b_raw [y*SCR_W+x]&0xFF;
        }
    }

    free(r_raw);
    free(g1_raw);
    free(g2_raw);
    free(b_raw);
    return bayerbuf;
}