/*
 * xvc.c
 * XVC compatible JTAG bridge implementation
 * Based on https://github.com/daedaleanai/stm32f103_xvcusb
 * See top level for license
 */


#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include "ringbuf.h"
#include "xvc.h"

#define JTAG_GPIO	GPIOB
#define TCK_PIN		GPIO_PIN_6
#define TDI_PIN		GPIO_PIN_7
#define TDO_PIN		GPIO_PIN_8
#define TMS_PIN		GPIO_PIN_9
#define LED_GPIO	GPIOA
#define LED_PIN		GPIO_PIN_5

#define LED_ON()    LED_GPIO->BSRR = LED_PIN
#define LED_OFF()   LED_GPIO->BRR = LED_PIN

#define RXBUF_SIZE	    2048
#define JTAGBUF_SIZE    APP_TX_DATA_SIZE

#define TCK_MIN_PREIOD  10000     // 3.3MHz
#define TCK_MAX_PERIOD  100000   // 100KHz

#define TIM2_FREQ_MHZ   48

typedef enum {
    TOK_NONE, TOK_GETINFO, TOK_SETTCK, TOK_SHIFT
} tok_t;

static const char xvc_info[] = "xvcServer_v1.0:2048\n";
static bool been_configured;
static Ringbuf *serial_buf = NULL;
static uint8_t cmd[10];
static int cmd_idx;
static uint8_t tms_vector[JTAGBUF_SIZE];
static uint8_t tdx_vector[JTAGBUF_SIZE]; // tdi out, tdo in
static size_t num_bits;
static size_t shift_count; // counts to 2* num_bits
static tok_t tok;

static void xvc_reset(void) {
    // Reset internal states
    been_configured = false;
    cmd_idx = 0;
    num_bits = 0;
    shift_count = 0;
    tok = TOK_NONE;
    // This function may be called by USB stack before XVC is initialized
    if (serial_buf)
        ringbuf_reset(serial_buf);
}

void xvc_init(void) {
    // TIM2 Init
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->PSC = 0; // 48MHz
    TIM2->ARR = 30 - 1; // 1.6MHz

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // USB Init
    // USB initialization is done by the CubeMX

    serial_buf = ringbuf_init(RXBUF_SIZE);

    xvc_reset();
}

void xvc_deinit(void) {
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);

    ringbuf_free(serial_buf);
}

void xvc_usb_push(uint8_t *buf, uint32_t len) {
    while (!ringbuf_push(serial_buf, buf, len)) {
        xvc_tick(); // Attempt to free up some space in the ring buffer
    }
}

void xvc_usb_set_conn_state(uint8_t state) {
    been_configured = (state == 1);
    if (been_configured)
        LED_ON();
    else
        LED_OFF();
    xvc_reset();
}

static bool xvc_usb_send(uint8_t *buf, uint32_t len) {
    uint8_t result = CDC_Transmit_FS(buf, len);
    return (result == USBD_OK);
}

static bool xvc_usb_pop(uint8_t *buf, uint32_t len) {
    return (ringbuf_pop(serial_buf, buf, len, true) == len);
}

static tok_t xvc_getcmd(void) {
    if (cmd_idx == sizeof(cmd)) {
        // Error, reset
        cmd_idx = 0;
    }
    // Try getting a byte
    if (xvc_usb_pop(&cmd[cmd_idx], 1)) {
        // Successfully got a byte
        if (cmd[cmd_idx] == ':') {
            // Got a token
            cmd_idx = 0;
            if (memcmp(cmd, "getinfo:", 8) == 0)
                return TOK_GETINFO;
            if (memcmp(cmd, "settck:", 7) == 0)
                return TOK_SETTCK;
            if (memcmp(cmd, "shift:", 6) == 0)
                return TOK_SHIFT;
        }
        else {
            cmd_idx++;
        }
    }
    return TOK_NONE;
}

static bool xvc_getuint32(uint32_t *val) {
    uint8_t buf[4];
    if (!xvc_usb_pop(buf, 4))
        return false;
    *val = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    return true;
}

void TIM2_IRQHandler(void) {
    if ((TIM2->SR & TIM_SR_UIF) == 0)
        return;
    TIM2->SR = ~TIM_SR_UIF; // W0C UIF

    size_t idx = (shift_count/2) / 8;
    uint8_t msk = 1U << ((shift_count/2) % 8);

    if ((shift_count & 1) == 0) {
        // start of phase: clock goes low and set TMS and TDI

        if (shift_count/2 < num_bits) {
            if (tms_vector[idx] & msk)
                JTAG_GPIO->BSRR = TMS_PIN;
            else
                JTAG_GPIO->BRR = TMS_PIN;

            if (tdx_vector[idx] & msk)
                JTAG_GPIO->BSRR = TDI_PIN;
            else
                JTAG_GPIO->BRR = TDI_PIN;

            JTAG_GPIO->BRR = TCK_PIN;
        }

    } else {
        // mid phase: clock goes high and sample TDO
        JTAG_GPIO->BSRR = TCK_PIN;

        if (JTAG_GPIO->IDR & TDO_PIN)
            tdx_vector[idx] |= msk;
        else
            tdx_vector[idx] &= ~msk;
    }

    ++shift_count;

    if ((shift_count + 1) == 2*num_bits)
        TIM2->CR1 &= ~TIM_CR1_CEN;
}

void jtag_shift_loop() {
//#pragma GCC push_options
//#pragma GCC optimize ("O3")
    size_t bytes_fast = num_bits / 8;
    size_t bits_left = num_bits - bytes_fast * 8;

    uint32_t gpio_state = JTAG_GPIO->ODR & ~(TMS_PIN | TDI_PIN | TCK_PIN);
    for (size_t i = 0; i < bytes_fast; i++) {
        uint8_t readin_val = 0x00;
        for (uint8_t msk = 0x01; msk != 0; msk <<= 1) {
            uint32_t new_gpio_state = gpio_state;
            if (tms_vector[i] & msk) new_gpio_state |= TMS_PIN;
            if (tdx_vector[i] & msk) new_gpio_state |= TDI_PIN;
            JTAG_GPIO->ODR = new_gpio_state;
            readin_val >>=1;
            asm("nop");
            JTAG_GPIO->BSRR = TCK_PIN;
            readin_val |= (JTAG_GPIO->IDR & TDO_PIN) >> 1; // TDO needs to be Px8!
        }
        tdx_vector[i] = readin_val;
    }

    // Last few bits
    if (bits_left != 0) {
        size_t i = bytes_fast;
        uint8_t readin_val = 0x00;
        uint8_t msk = 0x01;
        for (int j = 0; j < bits_left; j++) {
            uint32_t new_gpio_state = gpio_state;
            if (tms_vector[i] & msk) new_gpio_state |= TMS_PIN;
            if (tdx_vector[i] & msk) new_gpio_state |= TDI_PIN;
            JTAG_GPIO->ODR = new_gpio_state;
            asm("nop");
            asm("nop");
            JTAG_GPIO->BSRR = TCK_PIN;
            if (JTAG_GPIO->IDR & TDO_PIN)
                readin_val |= msk;
            msk <<= 1;
        }
        tdx_vector[i] = readin_val;
    }
//#pragma GCC pop_options
}

void xvc_tick(void) {
    static uint32_t val;
    static int i;
    static uint32_t num_bytes;
    static uint8_t buf[4];
    static enum {
        ST_IDLE,
        ST_SEND_RESP,
        ST_DISCARD,
        ST_RX_TMS,
        ST_RX_TDI,
        ST_WAIT_JTAG,
        ST_TX_TDO
    } state;

    if (tok == TOK_NONE) {
        // No token currently processing, try get new TOK
        tok = xvc_getcmd();
    }
    switch (tok) {
    case TOK_GETINFO:
        LED_OFF();
        if (xvc_usb_send((uint8_t *)xvc_info, sizeof(xvc_info) - 1)) {
            // Successfully sent, message done
            tok = TOK_NONE;
            LED_ON();
        }
        break;
    case TOK_SETTCK:
        if (state == ST_IDLE) {
            LED_OFF();
            // Hasn't got TCK period yet
            if (xvc_getuint32(&val)) {
                // Got desired TCK period in ns
                if (val < TCK_MIN_PREIOD) val = TCK_MIN_PREIOD;
                if (val > TCK_MAX_PERIOD) val = TCK_MAX_PERIOD;
                // Set TIM2 PSC and ARR
                TIM2->ARR = (TIM2_FREQ_MHZ / 2 * val / 1000) - 1;
                // Calculate actual period
                val = (TIM2->PSC + 1) * (TIM2->ARR + 1) * 1000;
                val /= (TIM2_FREQ_MHZ / 2);
                buf[0] = val & 0xff;
                buf[1] = (val >> 8) & 0xff;
                buf[2] = (val >> 16) & 0xff;
                buf[3] = (val >> 24) & 0xff;
                state = ST_SEND_RESP;
            }
        }
        if (state == ST_SEND_RESP) {
            if (xvc_usb_send(buf, 4)) {
                tok = TOK_NONE;
                state = ST_IDLE;
                LED_ON();
            }
        }
        break;
    case TOK_SHIFT:
        if (state == ST_IDLE) {
            LED_OFF();
            // Hasn't got data size yet
            if (xvc_getuint32(&val)) {
                num_bits = val;
                num_bytes = (num_bits + 7) / 8;
                if (num_bytes > JTAGBUF_SIZE) {
                    // Too large, can't handle
                    tok = TOK_NONE;
                    state = ST_IDLE;
                    LED_ON();
                }
                else {
                    state = ST_RX_TMS;
                }
                i = 0;
            }
        }
        if (state == ST_RX_TMS) {
            if (xvc_usb_pop(tms_vector, num_bytes)) {
                state = ST_RX_TDI;
                i = 0;
            }
        }
        if (state == ST_RX_TDI) {
            if (xvc_usb_pop(tdx_vector, num_bytes)) {
                state = ST_WAIT_JTAG;
                shift_count = 0;
                TIM2->CR1 |= TIM_CR1_CEN;
            }
        }
        if (state == ST_WAIT_JTAG) {
            if (!(TIM2->CR1 & TIM_CR1_CEN)) {
            //jtag_shift_loop();
                // Shift done
                state = ST_TX_TDO;
                i = 0;
            }
        }
        if (state == ST_TX_TDO) {
            if (xvc_usb_send(&tdx_vector[i], num_bytes - i)) {
                // Done
                tok = TOK_NONE;
                state = ST_IDLE;
                LED_ON();
            }
        }
        break;
    case TOK_NONE:
        break;
    }
}
