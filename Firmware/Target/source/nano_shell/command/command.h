/**
 * @file command.h
 * @author Liber (lvlebin@outlook.com), Cédric CARRÉE (beg0@free.fr)
 * @brief
 * @version 1.0
 * @date 2020-03-23
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_COMMAND_H
#define __NANO_SHELL_COMMAND_H

#include "shell_linker.h"

#include "shell_config.h"

// Forward delecation of shell command structure
struct _shell_cmd_s;

/**
 * @brief this is the implementation function of the command.
 *
 * @param pCmdt: pointer of the structure.
 * @param argc: the count of arguments.
 * @param argv: argument vector.
 * @return 0 if succeed, else non-zero. (return value is not used in ver1.0)
 *
 * @note the command name is the first argument, argv[0], so argc is always at least 1.
 */
typedef  int (*shell_cmd_cb_t)(const struct _shell_cmd_s *pCmdt, int argc, char *const argv[]);

// shell command structure
typedef struct _shell_cmd_s {
  const char *name; // command name

  shell_cmd_cb_t cmd; // Callback function to run the shell command

#if CONFIG_SHELL_CMD_BRIEF_USAGE
  const char *brief_usage; // brief usage of the command.
#endif

#if CONFIG_SHELL_CMD_LONG_HELP
  const char *help;  // detailed help information of the command.
#endif
} shell_cmd_t;


// shell function structure
typedef struct {
  const char *name;  // function name

  const int param_n; // number of parameters

  int (*func)(); // function pointr.

#if CONFIG_SHELL_FUNC_BRIEF_USAGE
  const char *brief;  // brief summaries of the command.
#endif
} shell_func_t;


#if CONFIG_SHELL_CMD_BRIEF_USAGE
  #define _CMD_BRIEF(x) x,
#else
  #define _CMD_BRIEF(x)
#endif /* CONFIG_SHELL_CMD_BRIEF_USAGE */

#if CONFIG_SHELL_CMD_LONG_HELP
  #define _CMD_HELP(x) x,
#else
  #define _CMD_HELP(x)
#endif /* CONFIG_SHELL_CMD_LONG_HELP */

#if CONFIG_SHELL_FUNC_BRIEF_USAGE
  #define _FUNC_BRIEF(x) x,
#else
  #define _FUNC_BRIEF(x)
#endif /* CONFIG_SHELL_FUNC_BRIEF_USAGE */



#define _shell_cmd_complete(_name, _func, _brief, _help) \
  { #_name, _func, _CMD_BRIEF(_brief) _CMD_HELP(_help) }


#define _shell_func_complete(_name, _nparam, _func, _brief) \
  { #_name, _nparam, _func, _FUNC_BRIEF(_brief) }


/**
 * @brief add a command to nano-shell
 *
 * @_name: name of the command. Note: THIS IS NOT a string.
 * @_func: function pointer: (*cmd)(const shell_cmd_t *, int, int, char *const[]).
 * @_brief: brief summaries of the command. This is a string.
 * @_help: detailed help information of the command. This is a string.
 */
#define NANO_SHELL_ADD_CMD(_name, _func, _brief, _help) \
  _shell_entry_declare(shell_cmd_t, _name) = _shell_cmd_complete(_name, _func, _brief, _help)


/**
 * @brief add a function to nano-shell.
 *
 * @_name: name of the function. Note: THIS IS NOT a string.
 * @_nparam: param num of the function.
 * @_func: pointer of the function.
 * @_brief: brief summaries of the function. This is a string.
 */
#define NANO_SHELL_ADD_FUNC(_name, _nparam, _func, _brief) \
  _shell_entry_declare(shell_func_t, _name) = _shell_func_complete(_name, _nparam, _func, _brief)

#if CONFIG_SHELL_CMD_BUILTIN_HELP
  #define _shell_help_subcmd_entry(_name) \
    NANO_SHELL_SUBCMD_ENTRY(help, _name ## _subcmd_help, \
                   "help [pattern ...]", \
                   "    Print information about subcommands of " # _name ".\r\n" \
                   "\r\n" \
                   "    If PATTERN is specified, gives detailed help on all commands\r\n" \
                   "    matching PATTERN, otherwise print the list of all available commands.\r\n" \
                   "\r\n" \
                   "    Arguments:\r\n" \
                   "        PATTERN: specifiying the help topic\r\n"),
  #define _shell_help_subcmd_declare(_name) \
    static int _name ## _subcmd_help(const shell_cmd_t* pCmd, int argc, char* const argv[]);
  #define _shell_help_subcmd_define(_name) \
    static int _name ## _subcmd_help(const shell_cmd_t* pCmd, int argc, char* const argv[]) \
    { \
      const unsigned int subcommands_count = sizeof(_name ## _subcommands)/sizeof(shell_cmd_t); \
      return shell_help_generic( \
                      argc, argv, \
                      "Help for " #_name, \
                      _name ## _subcommands, subcommands_count); \
    }
#else
  #define _shell_help_subcmd_entry(_name)
  #define _shell_help_subcmd_declare(_name)
  #define _shell_help_subcmd_define(_name)
#endif  /* CONFIG_SHELL_CMD_BUILTIN_HELP */

/**
 * @brief Add a sub command in a group of sub-command
 *
 * To be used as the last arguments of @ref NANO_SHELL_DEFINE_SUBCMDS()
 * The syntax is similar to @ref NANO_SHELL_ADD_CMD()
 *
 * @param _name: name of the command. Note: THIS IS NOT a string.
 * @param _func: function pointer: (*cmd)(const shell_cmd_t *, int, int, char *const[]).
 * @param _brief: brief summaries of the command. This is a string.
 * @param _help: detailed help information of the command. This is a string.
 */
#define NANO_SHELL_SUBCMD_ENTRY(_name, _func, _brief, _help) _shell_cmd_complete(_name, _func, _brief, _help)


/**
 * @brief Get the name of the function implementing a sub-command group in nano-shell
 *
 * @param _name name of the group of sub-commands
 *
 * @note this macro is to be used for the @c _func parameter of @ref NANO_SHELL_ADD_CMD() or @c _func parameter of @ref NANO_SHELL_SUBCMD_ENTRY()
 */
#define NANO_SHELL_SUBCMDS_FCT(_name) _name ## _shell_cmd

/**
 * @brief Define a group of sub-commands in nano-shell
 *
 * @param _name name of the group of sub-commands
 * @param fallback_fct: function that will be run if no subcommand can be found (either @c argc is 1 or argv[1] is not found in @c subcommand)
 * @param ... A list of @ref NANO_SHELL_SUBCMD_ENTRY() that define the list of sub-commands
 */
#define NANO_SHELL_DEFINE_SUBCMDS(_name, fallback_fct, ...) \
  _shell_help_subcmd_declare(_name) \
  static  const shell_cmd_t _name ## _subcommands[] = { \
          _shell_help_subcmd_entry(_name) \
          __VA_ARGS__ }; \
  _shell_help_subcmd_define(_name) \
  int NANO_SHELL_SUBCMDS_FCT(_name)(const shell_cmd_t* pCmd, int argc, char* const argv[]) \
  { \
    const unsigned int subcommands_count = sizeof(_name ## _subcommands)/sizeof(shell_cmd_t); \
    return shell_run_subcmd_implem(pCmd, argc, argv, \
      fallback_fct, _name ## _subcommands, subcommands_count); \
  }

/**
 * @brief Find a shell command by name
 *
 * Find in the list of commandes registred by @ref NANO_SHELL_ADD_CMD().
 *
 * @param cmd_name name of the shell command to search
 * @return const shell_cmd_t*
 */
const shell_cmd_t *shell_find_top_cmd(const char *cmd_name);


/**
 * @brief Find a shell command by name in a specific list of commands
 *
 * @param cmd_name name of the shell command to search
 * @param cmds list of commands to search
 * @count number of entries in @c cmds
 * @return const shell_cmd_t*
 */
const shell_cmd_t *shell_find_cmd(const char *cmd_name, const shell_cmd_t* cmds, unsigned int count);


/**
 * @brief Run a shell command from a parsed line
 *
 * @param argc
 * @param argv
 * @return int
 */
int shell_run_cmd(int argc, char *const argv[]);

/**
 * @brief Implementation function for @ref NANO_SHELL_ADD_CMD_WITH_SUB
 *
 * @param pCmdt: pointer of the structure.
 * @param argc: the count of arguments.
 * @param argv: argument vector.
 * @param fallback_fct: function that will be run if no subcommand can be found (either @c argc is 1 or argv[1] is not found in @c subcommand)
 * @param subcommands: a list of sub-commands
 * @param subcommands_count: number of entries in @c subcommands
 *
 * @return 0 if succeed, else non-zero. (return value is not used in ver1.0)
 */
int shell_run_subcmd_implem(const shell_cmd_t* pCmdt,
            int argc, char* const argv[],
            shell_cmd_cb_t fallback_fct,
            const shell_cmd_t* subcommands, unsigned int  subcommands_count);

/**
 * @brief Implementation function for 'help' command (or sub-command)
 *
 * @param argc: the count of arguments.
 * @param argv: argument vector.
 * @param preamble: text that will appears before the list of commands
 * @param start first command in the list of commands that we want to display helps for
 * @param count number of command in the list of command
 *
 * @return 0 if succeed, else non-zero. (return value is not used in ver1.0)
 */
int shell_help_generic(int argc, char *const argv[],
            const char* preamble,
            const shell_cmd_t* start, unsigned int count);

#endif /* __NANO_SHELL_COMMAND_H */
