/**
 * @file shell_io.c
 * @author Liber (lvlebin@outlook.com)
 * @brief Example implementation of some functions in file "shell_io.h".
 * @version 1.0
 * @date 2020-03-24
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "shell_io.h"
#include "shell_config.h"

#include "usb_support.h"

#if defined(__GNUC__)
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */


/****************************************************************/

#if !(CONFIG_SHELL_PRINTF_BUFFER_SIZE > 0)
#error "the value of CONFIG_SHELL_PRINTF_BUFFER_SIZE must be greater than 0!"
#endif


int shell_printf(const char *format, ...)
{
  static char shell_printf_buffer[CONFIG_SHELL_PRINTF_BUFFER_SIZE];

  int length = 0;

  va_list ap;
  va_start(ap, format);

  length = vsnprintf(shell_printf_buffer, CONFIG_SHELL_PRINTF_BUFFER_SIZE, format, ap);

  va_end(ap);

  usbsup_sendbuf(shell_printf_buffer, length);

  return length;
}


void shell_puts(const char *str)
{
  // Does not send the null terminator
  int length = strlen(str);
  bool sent = false;
  while (!sent) {
    sent = usbsup_sendbuf((char *)str, length);
  }
}


void shell_putc(char ch)
{
  usbsup_sendbuf(&ch, 1);
}

int shell_getc(char *ch)
{
  return usbsup_recvbuf(ch, 1);
}
