/**
 * @file shell_io.h
 * @author Liber (lvlebin@outlook.com)
 * @brief I/O interface
 * @version 1.0
 * @date 2020-03-17
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_IO_H
#define __NANO_SHELL_IO_H


/*********************************************************************
   nano-shell uses these functions to get/send character from/to stream.
   You should implement these functions in your project.
 *********************************************************************/

/**
 * @brief send a chararcter...
 *
 */
void shell_putc(char ch);


/**
 * @brief send string...
 *
 */
void shell_puts(const char *str);


/**
 * @brief printf() for nano-shell
 *
 */
int shell_printf(const char *format, ...) __attribute__((format(gnu_printf, 1, 2)));


/**
 * @brief Get next character available from stream.
 *        not used in <interrupt mode>.
 *
 * @param ch Return the character in `ch` if there was...
 * @return Result is non-zero if there was a character, or 0 if there wasn't.
 *
 * @note if you run nano-shell in <main loop mode>, to avoid losing characters, you'd better use a
 * low layer receive fifo. Take uart for example, you can detect incoming data using interrupts and
 * then store each received character in a first-in-first out (FIFO) buffer.
 *
 * then `shell_getc(char *ch)` may be like this:
 *
 * int shell_getc(char *ch)
 * {
 *   if (fifo_empty()) {  // if no character in fifo,
 *     return 0;          // return false
 *   }
 *
 *   *ch = fifo_pop();  // fifo is not empty, get it.
 *   return 1;          // return true
 * }
 *
 */
int shell_getc(char *ch);


#endif /* __NANO_SHELL_IO_H */
