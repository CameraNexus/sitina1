/**
 * @file cmd_help.c
 * @author Liber (lvlebin@outlook.com), Cédric CARRÉE (beg0@free.fr)
 * @brief nano-shell build in command: help
 * @version 1.0
 * @date 2020-03-25
 *
 * @copyright Copyright (c) Liber 2020
 *
 */


#include "command.h"
#include "shell_io/shell_io.h"

#include "shell_config.h"

/****************************** build in command: help ******************************/
#if CONFIG_SHELL_CMD_BUILTIN_HELP

static void shell_print_cmd_list(const shell_cmd_t* start, unsigned int count)
{
  const shell_cmd_t* tmp = start;
  while (count) {
#if CONFIG_SHELL_CMD_BRIEF_USAGE
    shell_printf("  %s: %s\r\n", tmp->name, tmp->brief_usage);
#else
    shell_printf("  %s\r\n", tmp->name);
#endif
    count--;
    tmp++;
  }
}

static void shell_print_cmd_help(const char *cmd_name,
                const shell_cmd_t* start, unsigned int count)
{
#if CONFIG_SHELL_CMD_LONG_HELP
  const shell_cmd_t *tmp = shell_find_cmd(cmd_name, start, count);

  if (tmp) {
#if CONFIG_SHELL_CMD_BRIEF_USAGE
    shell_printf("%s: %s\r\n", cmd_name, tmp->brief_usage);
#else
    shell_printf("%s:\r\n", cmd_name);
#endif

    // use puts() instead of printf() to avoid printf buffer overflow
    shell_puts(tmp->help);
  } else {
    shell_printf("%s: command not found.\r\n", cmd_name);
  }
#endif /* CONFIG_SHELL_CMD_LONG_HELP */
}


int shell_cmd_help(const shell_cmd_t *pcmd, int argc, char *const argv[])
{
  const shell_cmd_t *start = _shell_entry_start(shell_cmd_t);
  unsigned int count = _shell_entry_count(shell_cmd_t);
  return shell_help_generic(argc, argv,
                    "nano-shell, version 1.0.0.",
                    start, count);
}

int shell_help_generic(int argc, char *const argv[],
            const char* preamble,
            const shell_cmd_t* start, unsigned int count)
{
  if (argc == 1) {
    shell_puts(preamble);
    shell_puts("\r\n"
#if CONFIG_SHELL_CMD_LONG_HELP
               "Type `help name' to find out more about the function `name'.\r\n"
#endif
               "\r\n");
    shell_print_cmd_list(start, count);
    shell_puts("\r\n");
  }
#if CONFIG_SHELL_CMD_LONG_HELP
  else {
    for (int i = 1; i < argc; i++) {
      shell_print_cmd_help(argv[i], start, count);
    }
  }
#endif /* CONFIG_SHELL_CMD_LONG_HELP */
  return 0;
}

NANO_SHELL_ADD_CMD(help,
                   shell_cmd_help,
                   "help [pattern ...]",

                   "    Print information about builtin commands.\r\n"
                   "\r\n"
                   "    If PATTERN is specified, gives detailed help on all commands\r\n"
                   "    matching PATTERN, otherwise print the list of all available commands.\r\n"
                   "\r\n"
                   "    Arguments:\r\n"
                   "        PATTERN: specifiying the help topic\r\n");

#endif /* CONFIG_SHELL_CMD_BUILTIN_HELP */
