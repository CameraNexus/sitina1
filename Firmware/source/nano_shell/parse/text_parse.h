/**
 * @file text_parse.h
 * @author Liber (lvlebin@outlook.com)
 * @brief
 * @version 1.0
 * @date 2020-03-28
 *
 * @copyright Copyright (c) Liber 2020
 *
 */


#ifndef __NANO_SHELL_TEXT_PARSE_H
#define __NANO_SHELL_TEXT_PARSE_H

/**
 * @brief parse the line, doesn't support backslash('\') in ver1.0
 *
 * @param input: the line to be parsed.
 * @param argv:
 * @param maxArgc: max number of arguments.
 * @return int: the number of parsed arguments.
 */
int nano_shell_parse_line(char *input, char *argv[], const int maxArgc);


#endif /* __NANO_SHELL_TEXT_PARSE_H */
