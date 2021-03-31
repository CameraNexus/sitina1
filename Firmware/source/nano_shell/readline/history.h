/**
 * @file history.h
 * @author Liber (lvlebin@outlook.com)
 * @brief history manage interface
 * @version 1.0
 * @date 2020-03-20
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_HISTORY_H
#define __NANO_SHELL_HISTORY_H


/**
 * @brief add a new record
 *
 * @param input
 */
void rl_history_add(char *input);


/**
 * @brief Get next record
 *
 * @return char*
 */
char *rl_history_next(void);


/**
 * @brief Get previous record
 *
 * @return char*
 */
char *rl_history_prev(void);


/**
 * @brief Remove last record
 *
 */
void rl_history_rm_last(void);

#endif /* __NANO_SHELL_HISTORY_H */
