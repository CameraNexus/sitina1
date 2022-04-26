/**
 * @file command.c
 * @author Liber (lvlebin@outlook.com), Cédric CARRÉE (beg0@free.fr)
 * @brief
 * @version 1.0
 * @date 2020-03-24
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#include <string.h>

#include "shell_io/shell_io.h"
#include "command.h"

const shell_cmd_t *shell_find_top_cmd(const char *cmd_name)
{
  const shell_cmd_t *start = _shell_entry_start(shell_cmd_t);
  unsigned int count = _shell_entry_count(shell_cmd_t);
  return shell_find_cmd(cmd_name, start, count);
}

const shell_cmd_t *shell_find_cmd(const char *cmd_name, const shell_cmd_t* start, unsigned int count)
{
  const shell_cmd_t *tmp = start;

  if (cmd_name == NULL || start == NULL) {
    return NULL;
  }

  while (count) {
    if (strcmp(cmd_name, tmp->name) == 0) {
      return tmp;
    }
    count--;
    tmp++;
  }
  return NULL;
}


int shell_run_cmd(int argc, char *const argv[])
{
  if (argc > 0) {
    const shell_cmd_t *pCmdt = shell_find_top_cmd(argv[0]);

    if (pCmdt) {
      return pCmdt->cmd(pCmdt, argc, argv);
    }

    shell_printf("  %s: command not found.\r\n", argv[0]);
  }
  return -1;
}

int shell_run_subcmd_implem(const shell_cmd_t* pCmdt,
                int argc, char* const argv[],
                shell_cmd_cb_t fallback_fct,
                const shell_cmd_t* subcommands, unsigned int  subcommands_count)
{
  if (argc > 1) {
    const shell_cmd_t* pSubCmdt = shell_find_cmd(argv[1], subcommands, subcommands_count);

    if (pSubCmdt) {
      return pSubCmdt->cmd(pSubCmdt, argc - 1, argv + 1);
    }
    else if(fallback_fct) {
      return fallback_fct(pCmdt, argc, argv);
    }
    else {
      shell_printf("  %s: sub-command not found.\r\n", argv[1]);
    }
  }
  else if(fallback_fct) {
    return fallback_fct(pCmdt, argc, argv);
  }

  return -1;
}
