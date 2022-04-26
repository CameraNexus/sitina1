/**
 * @file fifo.h
 * @author Liber (lvlebin@outlook.com)
 * @brief simple fifo based on ring buffer
 * @version 1.0
 * @date 2020-03-24
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SHELL_FIFO_H
#define __NANO_SHELL_FIFO_H

/**
 * @brief: declare a STATIC fifo
 *
 * @name: name of the fifo, THIS IS NOT a string.
 * @size: MUST be a CONSTANT, and MUST be a POWER OF 2!!! such as 64, 128, 256, 512, etc ...
 * @size_type: data type of size, MUST be unsigned!!! such as, uint8_t, uint16_t, uint32_t, etc...
 *             and max_value(size_type) >= size-1
 * @data_type: data type of element.
 *
 * @example:
 *   1). declare a char type fifo: static_fifo_declare(my_fifo, 256, unsigned char, char);
 *   2). add a char to the fifo:   fifo_push(my_fifo, ch);
 *   3). get and remove a char:    char ch = fifo_pop(my_fifo);
 *
 *   4). check manually and then get and remove a char:
 *          char ch = 0;
 *          if (is_fifo_empty(my_fifo)) {
 *              ch = fifo_pop_unsafe(my_fifo);
 *          }
 *
 *   5). get a char without remove: char ch = fifo_peek(my_fifo);
 *   6). get capacity of the fifo:  size_t capacity = get_fifo_capacity(my_fifo);
 *   7). get size of the fifo:      size_t size = get_fifo_size(my_fifo);
 *
 * @note: All operations on the same FIFO declared by `static_fifo_declare` must be in the same scope.
 *
 */
#define static_fifo_declare(name, size, size_type, data_type)       \
  static struct {                                                   \
    size_type head;                                                 \
    size_type tail;                                                 \
    data_type buff[size];                                           \
  } _fifo_##name = { 0, 0, {0, } };                                 \
  static const size_type _fifo_size_##name = ((size)-1); // the actual available size is (size-1). use const type for gcc optimization


#define get_fifo_capacity(name) (_fifo_size_##name)

#define get_fifo_size(name) ((unsigned int)(_fifo_##name.head - _fifo_##name.tail))

#define is_fifo_empty(name) (_fifo_##name.head == _fifo_##name.tail)

#define is_fifo_full(name) (_fifo_##name.head - _fifo_##name.tail >= _fifo_size_##name)


/**
 * @brief: add a new element to the fifo. if the fifo is full, do nothing.
 *
 * @name: name of the fifo.
 * @data: the new element to be added.
 */
#define fifo_push(name, data)                                              \
  do {                                                                     \
    if (!is_fifo_full(name)) {                                             \
      _fifo_##name.buff[_fifo_##name.head++ & _fifo_size_##name] = (data); \
    }                                                                      \
  } while (0)


/**
 * @brief: Returns the last element in the fifo, without remove it.
 *         If the fifo is empty, return 0.
 *
 * @name: name of the fifo.
 *
 */
#define fifo_peek(name) \
  (is_fifo_empty(name) ? 0 : _fifo_##name.buff[_fifo_##name.tail & _fifo_size_##name])


/**
 * @brief: remove and return last element without checking if the fifo is empty.
 *
 * @name: name of the fifo
 */
#define fifo_pop_unsafe(name) \
  (_fifo_##name.buff[_fifo_##name.tail++ & _fifo_size_##name])


/**
 * @brief: Remove and return last element in the fifo.
 *         If the fifo is empty, return 0.
 *
 * @name: name of the fifo.
 */
#define fifo_pop(name) \
  (is_fifo_empty(name) ? 0 : _fifo_##name.buff[_fifo_##name.tail++ & _fifo_size_##name])


#endif
