/**
 * @file nano_shell.c
 * @author Liber (lvlebin@outlook.com), Cédric CARRÉE (beg0@free.fr)
 * @brief nano-shell: a light but powerful shell designed for embedded systems.
 * @version 1.0
 * @date 2020-03-27
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#include <string.h>
#include "nano_shell.h"
#include "shell_io/shell_io.h"
#include "command/command.h"
#include "readline/readline.h"
#include "parse/text_parse.h"

#include "shell_config.h"

#define NANO_SHELL_BUILD_VERDION "1.0"

/**
 * @brief
 *
 * @param argc: MUST be larger than 0
 * @param argv:
 * @return int
 */
int nano_shell_run_cmd(int argc, char *const argv[])
{
  const shell_cmd_t *pCmdt = shell_find_top_cmd(argv[0]);

  if (pCmdt) {
    return pCmdt->cmd(pCmdt, argc, argv);
  }
  shell_printf("  %s: command not found.\r\n", argv[0]);

  return -1;
}


#if (CONFIG_SHELL_CMD_MAX_ARGC < 1)
#error "CONFIG_SHELL_CMD_MAX_ARGC must be no less than 1."
#endif

void nano_shell_loop(void *argument)
{
  static char *argv[CONFIG_SHELL_CMD_MAX_ARGC + 1];
  char *line;
  int argc;

  shell_puts("\r\n"
             " _   _                     ____  _          _ _\r\n"
             "| \\ | | __ _ _ __   ___   / ___|| |__   ___| | |\r\n"
             "|  \\| |/ _` | '_ \\ / _ \\  \\___ \\| '_ \\ / _ \\ | |\r\n"
             "| |\\  | (_| | | | | (_) |  ___) | | | |  __/ | |\r\n"
             "|_| \\_|\\__,_|_| |_|\\___/  |____/|_| |_|\\___|_|_|\r\n"
             "\r\n"
             "Welcome to Nano-Shell "NANO_SHELL_BUILD_VERDION"\r\n"
             "\r\n"
             " * Build:     "__DATE__" - "__TIME__"\r\n"
             " * Source:    https://github.com/lebinlv/nano-shell\r\n"
             " * Copyright: (c) Liber 2020\r\n"
             "\r\n");

  for (;;) {
    line = readline(CONFIG_SHELL_PROMPT);

    argc = nano_shell_parse_line(line, argv, CONFIG_SHELL_CMD_MAX_ARGC + 1);

    // shell_printf("[DEBUG] argc: %d\r\n", argc);
    // for (int i = 0; i < argc; i++) {
    //   shell_printf("[DEBUG] ARGV[%d]: %s\r\n", i, argv[i]);
    // }

    if (argc > CONFIG_SHELL_CMD_MAX_ARGC) {
      argc--;
      shell_printf("** WARNING: too many args (max: %d)! ", CONFIG_SHELL_CMD_MAX_ARGC);
      shell_printf("arguments after \"%s\" will be ignored. **\r\n", argv[argc - 1]);
    }

    if (argc > 0) {
      nano_shell_run_cmd(argc, argv);
    }
  }
}


void nano_shell_react(char ch)
{
  static char *argv[CONFIG_SHELL_CMD_MAX_ARGC + 1];
  int argc;

  char *line = readline_react(ch);

  if (line) {
    /**
     * in react mode, use if (* line) to avoid unnecessary process
     * to improve speed.
     */
    if (*line) {
      argc = nano_shell_parse_line(line, argv, CONFIG_SHELL_CMD_MAX_ARGC + 1);

      if (argc > CONFIG_SHELL_CMD_MAX_ARGC) {
        argc--;
        shell_printf("** WARNING: too many args (max: %d)! ", CONFIG_SHELL_CMD_MAX_ARGC);
        shell_printf("arguments after \"%s\" will be ignored. **\r\n", argv[argc - 1]);
      }

      if (argc > 0) {
        nano_shell_run_cmd(argc, argv);
      }
    }

    if (CONFIG_SHELL_PROMPT) {
      shell_puts(CONFIG_SHELL_PROMPT);
    }
  }
}
