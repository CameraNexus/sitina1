/**
 * @file key_seq.h
 * @author Liber (lvlebin@outlook.com)
 * @brief
 * @version 1.0
 * @date 2020-03-21
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_KEY_SEQ_H
#define __NANO_SHELL_KEY_SEQ_H

#include <stdint.h>

typedef uint32_t u32;
typedef uint8_t u8;

#define CONVERT_KEY_SEQ(c1, c2, c3, c4) \
  ((u32)((((u8)(c1)) << 24) | (((u8)(c2)) << 16) | (((u8)(c3)) << 8) | (((u8)(c4)) << 0)))


typedef struct {
  u32 key_seq_val;
  void (*key_func)(void);
} key_seq_t;


void rl_dispatch_seq(char ch);


#endif /* __NANO_SHELL_KEY_SEQ_H */
