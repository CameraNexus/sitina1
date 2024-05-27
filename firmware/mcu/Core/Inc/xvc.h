/*
 * xvc.h
 * XVC compatible JTAG bridge implementation
 * Based on https://github.com/daedaleanai/stm32f103_xvcusb
 * See top level for license
 */

#ifndef INC_XVC_H_
#define INC_XVC_H_

void xvc_init(void);
void xvc_deinit(void);
void xvc_usb_push(uint8_t *buf, uint32_t len);
void xvc_usb_set_conn_state(uint8_t state);
void xvc_tick(void);

#endif /* INC_XVC_H_ */
