/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 NXP
 * Copyright (c) 2021 Wenting Zhang <zephray@outlook.com>
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_SUPPORT_H_
#define _USB_SUPPORT_H_

void usbsup_init(void);
bool usbsup_sendbuf(char *buf, int size);
int usbsup_recvbuf(char *buf, int maxSize);
void usbsup_waitconnect(void);

#endif /* _USB_SUPPORT_H_ */
