/**
 * @file command.h
 * @author Liber (lvlebin@outlook.com)
 * @brief
 * @version 1.0
 * @date 2020-03-23
 *
 */

#ifndef __NANO_SHELL_LINKER_H
#define __NANO_SHELL_LINKER_H

#define __align(x) __attribute__((aligned(x)))

/**
 * @brief array entry declare.
 * @_type: data type of the entry.
 * @_name: name of the entry.
 */
#define _shell_entry_declare(_type, _name)                   \
  static const _type nano_shell_##_type##_##_name __align(4) \
    __attribute__((used, section(".nano_shell_" #_type "_1_" #_name)))


/**
 * @brief: get the pointer of first entry.
 * @_type: data type of the entry.
 */
#define _shell_entry_start(_type)                                   \
  ({                                                                \
    static char start[0] __align(4)                                 \
      __attribute__((unused, section(".nano_shell_" #_type "_0"))); \
    (_type *)&start;                                                \
  })


/**
 * @brief: get the pointer after last entry.
 * @_type: data type of the entry.
 */
#define _shell_entry_end(_type)                                     \
  ({                                                                \
    static char end[0] __align(4)                                   \
      __attribute__((unused, section(".nano_shell_" #_type "_2"))); \
    (_type *)&end;                                                  \
  })


/**
 * @brief: get the number of elements.
 * @_type: data type of the entry.
 */
#define _shell_entry_count(_type)                   \
  ({                                                \
    _type *start = _shell_entry_start(_type);       \
    _type *end = _shell_entry_end(_type);           \
    unsigned int count = end - start;               \
    count;                                          \
  })


#endif /* __NANO_SHELL_LINKER_H */
