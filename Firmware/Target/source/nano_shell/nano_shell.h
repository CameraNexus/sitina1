/**
 * @file nano_shell.h
 * @author Liber (lvlebin@outlook.com)
 * @brief nano-shell interface. include this file in your project.
 * @version 1.0
 * @date 2020-03-27
 *
 * @copyright Copyright (c) Liber 2020
 *
 */


#ifndef __NANO_SHELL_H
#define __NANO_SHELL_H

/**
 * @brief nano-shell infinite loop
 *
 * @param argument not used in ver1.0
 */
void nano_shell_loop(void *argument);


/**
 * @brief nano-shell non-block interface, just react to the input character.
 * It is non-blocked (unless there is an infinite loop in your command function)
 * you can call it when get a new character.
 *
 * @param ch input character
 */
void nano_shell_react(char ch);


#endif /*__NANO_SHELL_H */
