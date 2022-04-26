//
// syslog.c: Simple syslog implementation
//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
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
#include <stdarg.h>
#include <stdbool.h>
#include "umm_malloc.h"
#include "ringbuf.h"
#include "syslog.h"
#include "nano_shell.h"
#include "command/command.h"
#include "shell_io/shell_io.h"

Ringbuf *syslog_ringbuf;

void syslog_init(void) {
    syslog_ringbuf = ringbuf_init(SYSLOG_BUFFER_SIZE);
}

int syslog_printf(const char *format, ...)
{
    static char printf_buffer[SYSLOG_PRINTF_BUFFER_SIZE];

    int length = 0;

    va_list ap;
    va_start(ap, format);

    length = vsnprintf(printf_buffer, SYSLOG_PRINTF_BUFFER_SIZE, format, ap);

    va_end(ap);

    // Does not save the null terminator
    ringbuf_push(syslog_ringbuf, printf_buffer, length);

    return length;
}

void syslog_fatal(const char *msg)
{
    // Semihosting printf for now
    printf(msg);
    while(1);
}

int syslog_cmd(const shell_cmd_t *pcmd, int argc, char *const argv[]) {
    if (argc > 1) {
        shell_printf("syslog command does not take any arguments.\n");
        return 1;
    }
    char buf[128 + 1];
    int length;
    do {
        length = ringbuf_pop(syslog_ringbuf, buf, 128, false);
        if (length) {
            buf[length] = '\0';
            shell_puts(buf);
        }
    } while (length != 0);

    return 0;
}

NANO_SHELL_ADD_CMD(syslog, syslog_cmd, "show syslog",
        "  Prints out all syslog since last command call");
