//
// Sitina simulator
// Copyright 2024 Wenting Zhang
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
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <SDL.h>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vsystem.h"

#include "apbsim.h"
#include "axiep.h"
#include "camsim.h"
#include "dispsim.h"
#include "memmap.h" // AXI memmap, only use for RAM

#define GPIO_BASE               0x00000
#define CCDTG_BASE              0x10000
#define DSILITE_BASE            0x20000
#define PWMLITE_BASE            0x30000
#define DCIF_BASE               0x40000

#define GPIO_REG_ODR            0x0000
#define GPIO_REG_IDR            0x0004
#define GPIO_REG_BSR            0x0008
#define GPIO_REG_BCR            0x000C
#define GPIO_REG_OER            0x0010

#define CCDTG_REG_CTRL          0x0000
#define CCDTG_REG_DELAY_HTIME   0x0004
#define CCDTG_REG_DELAY_VTIME   0x0008
#define CCDTG_REG_ESHUT_LINE    0x000C

#define CCDTG_CTRL_VAL          ((1 << 9) | (1 << 8) | (0 << 4) | (1 << 2))
#define CCDTG_CTRL_VAL_STILL    ((1 << 9) | (1 << 8) | (0 << 4) | (1 << 3))

#define DSILITE_REG_PCTL        0x0000
#define DSILITE_REG_DMACTL      0x0004
#define DSILITE_REG_STARTADDR   0x0020
#define DSILITE_REG_ENDADDR     0x0028

#define LCD_VACT                120
#define LCD_HACT                160

#define FRAMEBUF_BASE           0x1000000
#define FRAMEBUF_SIZE           (LCD_VACT * LCD_HACT * 4)
#define FRAMEBUF_END            FRAMEBUF_BASE + FRAMEBUF_SIZE

#define CAM_VACT                10
#define CAM_HACT                1024
#define CAM_VBLK                5
#define CAM_HBLK                10

#define CAMBUF_BASE             0x2000000
#define CAMBUF_SIZE             (CAM_VACT * CAM_HACT * 2)
#define CAMBUF_END              CAMBUF_BASE + CAMBUF_SIZE

#define DMACTL_BURSTLEN         8   // in 64-bit words
#define DMACTL_MAXINFLIGHT      4   // Max number of outstanding requests

#define PWMLITE_REG_CTL         0x0000
#define PWMLITE_REG_PSC         0x0004
#define PWMLITE_REG_CNT         0x0008
#define PWMLITE_REG_CMP         0x000C

#define DCIF_REG_PCTL           0x0000
#define DCIF_REG_DMACTL         0x0004
#define DCIF_REG_FBSWAP         0x0008
#define DCIF_REG_VBLK           0x0010
#define DCIF_REG_VACT           0x0014
#define DCIF_REG_HBLK           0x0018
#define DCIF_REG_HACT           0x001C
#define DCIF_REG_STARTADDR0_L   0x0020
#define DCIF_REG_ENDADDR0_L     0x0028
#define DCIF_REG_STARTADDR1_L   0x0030
#define DCIF_REG_ENDADDR1_L     0x0038

static bool unlimited = true;
static uint64_t max_cycles = 100;
static bool enable_trace = false;
static bool reset_done = false;

#define SIM_STEP 100000

constexpr int DISP_FACTOR = 2;

// SDL
SDL_Surface *screen;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Rect texture_rect;

// Verilator related
Vsystem *core;
VerilatedVcdC *trace;
uint64_t tickcount;

void testmain(void) {
    static int step = 0;
    if (!reset_done)
        return;
    // This function is called every tick
    switch (step) {
    case 0:
        apbsim_write(GPIO_BASE | GPIO_REG_OER, 0x01);
        apbsim_write(GPIO_BASE | GPIO_REG_ODR, 0x01);
        apbsim_write(DSILITE_BASE | DSILITE_REG_STARTADDR, FRAMEBUF_BASE);
        apbsim_write(DSILITE_BASE | DSILITE_REG_ENDADDR, FRAMEBUF_END);
        apbsim_write(DSILITE_BASE | DSILITE_REG_DMACTL, (DMACTL_MAXINFLIGHT << 8) | DMACTL_BURSTLEN);
        apbsim_write(DSILITE_BASE | DSILITE_REG_PCTL, 0x1);
        apbsim_write(PWMLITE_BASE | PWMLITE_REG_PSC, 255);
        apbsim_write(PWMLITE_BASE | PWMLITE_REG_CNT, 255);
        apbsim_write(PWMLITE_BASE | PWMLITE_REG_CMP, 127);
        apbsim_write(PWMLITE_BASE | PWMLITE_REG_CTL, 1);
        apbsim_write(DCIF_BASE | DCIF_REG_DMACTL, (DMACTL_MAXINFLIGHT << 8) | DMACTL_BURSTLEN);
        apbsim_write(DCIF_BASE | DCIF_REG_VBLK, CAM_VBLK);
        apbsim_write(DCIF_BASE | DCIF_REG_VACT, CAM_VACT);
        apbsim_write(DCIF_BASE | DCIF_REG_HBLK, CAM_HBLK);
        apbsim_write(DCIF_BASE | DCIF_REG_HACT, CAM_HACT);
        apbsim_write(DCIF_BASE | DCIF_REG_STARTADDR0_L, CAMBUF_BASE);
        apbsim_write(DCIF_BASE | DCIF_REG_ENDADDR0_L, CAMBUF_END);
        apbsim_write(DCIF_BASE | DCIF_REG_PCTL, 0x07 | (1 << 16) | (1 << 17));
        apbsim_write(CCDTG_BASE | CCDTG_REG_CTRL, CCDTG_CTRL_VAL);
        apbsim_write(CCDTG_BASE | CCDTG_REG_ESHUT_LINE, 0);
        apbsim_write(CCDTG_BASE | CCDTG_REG_CTRL, CCDTG_CTRL_VAL | 1);
        apbsim_write(DCIF_BASE | DCIF_REG_FBSWAP, 1);
        apbsim_write(CCDTG_BASE | CCDTG_REG_DELAY_HTIME, 4 - 1);
        apbsim_write(CCDTG_BASE | CCDTG_REG_DELAY_VTIME, 0);
        apbsim_write(CCDTG_BASE | CCDTG_REG_CTRL, CCDTG_CTRL_VAL_STILL | 1);
        step = 1;
        break;
    case 1:
        break;
    default:
        break;
    // Do more test here
    }
}

void tick(void) {
    // Create local copy of input signals
    uint8_t m_axi_awready = core->m_axi_awready;
    uint8_t m_axi_wready = core->m_axi_wready;
    uint8_t m_axi_bid = core->m_axi_bid;
    uint8_t m_axi_bresp = core->m_axi_bresp;
    uint8_t m_axi_bvalid = core->m_axi_bvalid;
    uint8_t m_axi_arready = core->m_axi_arready;
    uint8_t m_axi_rid = core->m_axi_rid;
    uint64_t m_axi_rdata = core->m_axi_rdata;
    uint8_t m_axi_rresp = core->m_axi_rresp;
    uint8_t m_axi_rlast = core->m_axi_rlast;
    uint8_t m_axi_rvalid = core->m_axi_rvalid;

    uint8_t s_apb_pwrite = core->s_apb_pwrite;
    uint8_t s_apb_penable = core->s_apb_penable;
    uint8_t s_apb_psel = core->s_apb_psel;
    uint32_t s_apb_pwdata = core->s_apb_pwdata;
    uint32_t s_apb_paddr = core->s_apb_paddr;

    uint16_t dvp_d = core->dvp_d;

    // Call simulated modules
    axiep_apply(
        core->m_axi_awid,
        core->m_axi_awaddr,
        core->m_axi_awlen,
        core->m_axi_awsize,
        core->m_axi_awburst,
        core->m_axi_awvalid,
        m_axi_awready,
        core->m_axi_wdata,
        core->m_axi_wstrb,
        core->m_axi_wlast,
        core->m_axi_wvalid,
        m_axi_wready,
        core->m_axi_bready,
        m_axi_bid,
        m_axi_bresp,
        m_axi_bvalid,
        core->m_axi_arid,
        core->m_axi_araddr,
        core->m_axi_arlen,
        core->m_axi_arsize,
        core->m_axi_arburst,
        core->m_axi_arvalid,
        m_axi_arready,
        core->m_axi_rready,
        m_axi_rid,
        m_axi_rdata,
        m_axi_rresp,
        m_axi_rlast,
        m_axi_rvalid
    );

    apbsim_apply(
        s_apb_pwrite,
        s_apb_pwdata,
        s_apb_paddr,
        s_apb_penable,
        s_apb_psel,
        core->s_apb_pready,
        core->s_apb_prdata
    );

    // dispsim_apply(
    //     (uint32_t *)screen->pixels,
    //     core->dpi_vsync,
    //     core->dpi_hsync,
    //     core->dpi_enable,
    //     core->dpi_data
    // );

    camsim_apply(
        core->dvp_hsync,
        core->dvp_vsync,
        dvp_d
    );

    // Posedge
    core->clk = 1;
    core->clk_lcd = 1;
    core->clk_ccd_4x = 1;
    core->dvp_pclk = 1;
    core->eval();

    // Apply changed input signals after clock edge
    core->m_axi_awready = m_axi_awready;
    core->m_axi_wready = m_axi_wready;
    core->m_axi_bid = m_axi_bid;
    core->m_axi_bresp = m_axi_bresp;
    core->m_axi_bvalid = m_axi_bvalid;
    core->m_axi_arready = m_axi_arready;
    core->m_axi_rid = m_axi_rid;
    core->m_axi_rdata = m_axi_rdata;
    core->m_axi_rresp = m_axi_rresp;
    core->m_axi_rlast = m_axi_rlast;
    core->m_axi_rvalid = m_axi_rvalid;

    core->s_apb_pwrite = s_apb_pwrite;
    core->s_apb_penable = s_apb_penable;
    core->s_apb_psel = s_apb_psel;
    core->s_apb_pwdata = s_apb_pwdata;
    core->s_apb_paddr = s_apb_paddr;

    core->dvp_d = dvp_d;

    static uint8_t clkdiv = 0;
    core->clk_ccd_1x = (clkdiv >= 2) ? 1 : 0;
    if (clkdiv == 3)
        clkdiv = 0;
    else 
        clkdiv ++;

    // Let combinational changes propagate
    core->eval();
    if (enable_trace)
        trace->dump(tickcount * 10);

    // Negedge
    core->clk = 0;
    core->clk_lcd = 0;
    core->clk_ccd_4x = 0;
    core->dvp_pclk = 0;
    core->eval();
    if (enable_trace)
        trace->dump(tickcount * 10 + 5);

    tickcount++;

    testmain();
}

void reset(void) {
    reset_done = false;
    core->rst = 1;
    core->rst_ccd = 1;
    core->rst_lcd = 1;
    tick();
    tick();
    core->rst = 0;
    core->rst_ccd = 0;
    core->rst_lcd = 0;
    reset_done = true;
    apbsim_reset();
    axiep_reset();
    camsim_reset();
    dispsim_reset();
    memmap_reset();
    //memmap_load("test_image.bin", FRAMEBUF_BASE - RAM_BASE);
    for (uint64_t i = FRAMEBUF_BASE; i < FRAMEBUF_END; i += 8) {
        memmap_write(i, 0x01234567893a01efull, 0xff);
    }
}

void render_copy(void) {
    void *texture_pixel;
	int texture_pitch;

	SDL_LockTexture(texture, NULL, &texture_pixel, &texture_pitch);
	memset(texture_pixel, 0, texture_rect.y * texture_pitch);
	uint8_t *pixels = (uint8_t *)texture_pixel + texture_rect.y * texture_pitch;
	uint8_t *src = (uint8_t *)screen->pixels;
	int left_pitch = texture_rect.x * 4;
	int right_pitch = texture_pitch - ((texture_rect.x + texture_rect.w) * 4);
	for (int y = 0; y < texture_rect.h; y++, src += screen->pitch)
	{
		memset(pixels, 0, left_pitch); pixels += left_pitch;
		memcpy(pixels, src, DISP_WIDTH * 4); pixels += DISP_WIDTH * 4;
		memset(pixels, 0, right_pitch); pixels += right_pitch;
	}
	memset(pixels, 0, texture_rect.y * texture_pitch);
	SDL_UnlockTexture(texture);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    // Initialize testbench
    Verilated::commandArgs(argc, argv);

    core = new Vsystem;
    Verilated::traceEverOn(true);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--limit") == 0) {
            if (i == (argc - 1)) {
                fprintf(stderr, "Error: no cycle limit provided\n");
                exit(1);
            }
            else {
                max_cycles = atoll(argv[i + 1]);
                printf("Cycle limit set to %lu.\n", max_cycles);
                unlimited = false;
            }
        }
        else if (strcmp(argv[i], "--trace") == 0) {
            enable_trace = true;
            trace = new VerilatedVcdC;
            core->trace(trace, 99);
            trace->open("trace.vcd");
        }
    }

    // Initialize window
    window = SDL_CreateWindow("Sitina",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            DISP_WIDTH * DISP_FACTOR, DISP_HEIGHT * DISP_FACTOR, SDL_SWSURFACE);
    assert(window);
    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert(renderer);
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, DISP_WIDTH, DISP_HEIGHT, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    assert(screen);
    texture_rect.x = texture_rect.y = 0;
    texture_rect.w = DISP_WIDTH;
    texture_rect.h = DISP_HEIGHT;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING, DISP_WIDTH, DISP_HEIGHT);
    assert(texture);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_FillRect(screen, &texture_rect, 0xFFFFFFFF);

    // Start simulation
    printf("Simulation start.\n");

    reset();

    uint32_t ms_tick = SDL_GetTicks();
    bool running = true;
    while (running) {
        for (int i = 0; i < 10; i++) {
            for (int i = 0; i < SIM_STEP / 10; i++) {
                tick();
            }

            SDL_Event event;
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = false;
                }
            }
        }
        
        uint32_t ms_delta = SDL_GetTicks() - ms_tick;
        if (ms_delta == 0)
            ms_delta = 1;
        char title[50];
        snprintf(title, 50, "Sitina Sim (%d kHz)", SIM_STEP / ms_delta);
        SDL_SetWindowTitle(window, title);
        ms_tick = SDL_GetTicks();

        if (!unlimited && (tickcount > max_cycles))
            break;
    }

    printf("Stop.\n");

    if (enable_trace)
        trace->close();

    SDL_FreeSurface(screen);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
