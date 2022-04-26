/**
 * @file readline.h
 * @author Liber (lvlebin@outlook.com)
 * @brief readline interface
 * @version 1.0
 * @date 2020-03-21
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_READLINE_H
#define __NANO_SHELL_READLINE_H


// read a line of input. Prompt with PROMPT. A NULL PROMPT means none.
char *readline(const char *promot);


/**
 * @brief react to the input character `ch`.
 *
 * @param[in] ch: input character.
 * @return: NULL means the current line has not been completed (need more input).
 *
 */
char *readline_react(char ch);


#endif /* __NANO_SHELL_READLINE_H */
