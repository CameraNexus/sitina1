/**
 * @file text_parse.c
 * @author Liber (lvlebin@outlook.com)
 * @brief
 * @version 1.0
 * @date 2020-03-28
 *
 * @copyright Copyright (c) Liber 2020
 *
 */


#include <stddef.h>
#include "text_parse.h"

#define isblank(c) ((c) == ' ' || (c) == '\t')


int nano_shell_parse_line(char *input, char *argv[], const int maxArgc)
{
  char tmp;
  int nargc = 0;

  while (nargc < maxArgc) {
    while (isblank(*input)) {
      input++;
    }
    if (*input == '\0') {  // end of input
      argv[nargc] = NULL;
      break;
    }

    tmp = *input;
    // single quotes ('') and double quotes ("")
    if (tmp == '\'' || tmp == '"') {
      argv[nargc] = ++input;
      while (*input && (*input != tmp)) {
        input++;
      }
    } else {  // normal character
      argv[nargc] = input++;
      while (*input && !isblank(*input)) {
        input++;
      }
    }

    nargc++;
    if (*input) {
      *input++ = '\0'; /* terminate current arg */
    }
  }

  return nargc;
}
