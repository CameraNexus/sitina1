//
// memtest.c: Simple memory tester
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
#include <stdlib.h>
#include <stdint.h>
#include "memtest.h"

#include "nano_shell.h"
#include "command/command.h"
#include "shell_io/shell_io.h"

//static uint32_t *test_area;
//static uint32_t test_size;

void memtest_test(uint32_t addr, uint32_t size) {
    int size_mb = size / 1048576;
    uint32_t *ptr;
    uint32_t testval;
    uint32_t actual;
    for (int i = 0; i < size_mb; i++) {
        ptr = (uint32_t *)(addr + i * 1048576);
        for (int j = 0; j < 128 * 1024; j++) {
            testval = ~(uint32_t)ptr;
            *ptr++ = testval;
        }
        ptr = (uint32_t *)(addr + i * 1048576);
        for (int j = 0; j < 128 * 1024; j++) {
            testval = ~(uint32_t)ptr;
            actual = *ptr++;
            if (actual != testval) {
                shell_printf("Test failed at %p, exp %08x, got %08x\r\n",
                        ptr, testval, actual);
                return;
            }
        }
        shell_printf("%d MB tested\r\n", i);
    }
}

int memtest_cmd(const shell_cmd_t *pcmd, int argc, char *const argv[]) {
    if (argc == 1) {
        memtest_test(0x80000000, 32 * 1024 * 1024);
        return 1;
    }

    return 1;
}

NANO_SHELL_ADD_CMD(memtest, memtest_cmd, "test memory", "command to test memory region");
