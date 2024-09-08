#include "effusus_imageio.h"
#include <malloc.h>
#include <math.h>

#include <libpng/png.h>

void effusus_dither_to_vram(uint16_t* vram, uint16_t* rawbuf, double gamma, \
    int SCR_W, int SCR_H, effusus_imageio_dither_t dither){
    // Render to Screen
    // This is lossy, expect to see banding when dithering is disabled
    // Floyd-Steinberg
    double kernel_fs[] = {
        0,        0,        0,
        0,        0,        7.0/16.0,
        3.0/16.0, 5.0/16.0, 1.0/16.0
    };
    double kernel_none[] = {
        0,        0,        0,
        0,        0,        0,
        0,        0,        0
    };

    double* kernel;
    switch(dither){
        case EFFUSUS_DITHER_NONE:
            kernel = kernel_none;
            PRINTINFO("Dithering disabled\n");
            break;
        case EFFUSUS_DITHER_FS:
            kernel = kernel_fs;
            PRINTINFO("Using Floyd-Steinberg Dithering\n");
            break;
        default:
            kernel = kernel_none;
            PRINTINFO("Unknown dithering mode, using none\n");
            break;
    }
    
    uint16_t* r_raw;
    uint16_t* g_raw;
    uint16_t* b_raw;
    uint16_t* r_q;
    uint16_t* g_q;
    uint16_t* b_q;
    r_raw  = (uint16_t*)malloc(2*SCR_H*SCR_W);
    g_raw  = (uint16_t*)malloc(2*SCR_H*SCR_W);
    b_raw  = (uint16_t*)malloc(2*SCR_H*SCR_W);
    r_q    = (uint16_t*)malloc(2*SCR_H*SCR_W);
    g_q    = (uint16_t*)malloc(2*SCR_H*SCR_W);
    b_q    = (uint16_t*)malloc(2*SCR_H*SCR_W);

    for(int y=0;y<SCR_H;y++){
        for(int x=0;x<SCR_W;x++){
            uint16_t r  = rawbuf[4*(y*SCR_W+x)+0];
            uint16_t g1 = rawbuf[4*(y*SCR_W+x)+1];
            uint16_t g2 = rawbuf[4*(y*SCR_W+x)+3];
            uint16_t b  = rawbuf[4*(y*SCR_W+x)+2];
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
}

void effusus_writepng(const char* fn, uint16_t* raw, int SCR_W, int SCR_H){
    FILE* file = fopen(fn, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file %s for writing\n", fn);
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

    png_set_IHDR(png, info, SCR_W, SCR_H, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png, info);

    uint16_t* raw_rgb = (uint16_t*)malloc(2*(SCR_W+1)*SCR_H*3);
        for(int y=0;y<SCR_H;y++){
            for(int x=0;x<SCR_W;x++){
                    int ptr3 = 3*((SCR_H - y - 1)*SCR_W+(SCR_W - x - 1));
                    int ptr4 = 4*(y*SCR_W+x);
                    raw_rgb[ptr3+0] = raw[ptr4+0];
                    raw_rgb[ptr3+1] = (raw[ptr4+1]>>1)+(raw[ptr4+3]>>1);
                    raw_rgb[ptr3+2] = raw[ptr4+2];
            }
        }

    png_bytep row = (png_bytep)malloc(3 * SCR_W * sizeof(png_byte));
    for (int y = 0; y < SCR_H; y++) {
        for (int x = 0; x < SCR_W; x++) {
            row [3*x+0] = raw_rgb[3*(y*SCR_W+x)+0]>>8;
            row [3*x+1] = raw_rgb[3*(y*SCR_W+x)+1]>>8;
            row [3*x+2] = raw_rgb[3*(y*SCR_W+x)+2]>>8;
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);

    free(row);
    free(raw_rgb);

    png_destroy_write_struct(&png, &info);
    fclose(file);
    PRINTINFO("PNG written to %s\n", fn);
}

void effusus_writetiff();