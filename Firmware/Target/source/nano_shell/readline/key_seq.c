/**
 * @file key_seq.c
 * @author Liber (lvlebin@outlook.com)
 * @brief ESC Control Sequence recognize and key-sequence-map.
 * @version 1.0
 * @date 2020-03-21
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#ifndef __NANO_SEHLL_KEY_SEQ_MAP_H
#define __NANO_SHELL_KEY_SEQ_MAP_H

#include "key_seq.h"
#include "shell_io/shell_io.h"

#include "shell_config.h"

extern void rl_get_pre_history(void);   // `up arrow` or `Ctrl P`
extern void rl_get_next_history(void);  // `down arrow` or `Ctrl N`
extern void rl_backward_cursor(void);   // `right arrow` or `Ctrl F`
extern void rl_forward_cursor(void);    // `left arrow` or `Ctrl B`
extern void rl_line_home(void);         // `Home`
extern void rl_line_end(void);          // `End`
extern void rl_delete(void);            // `Delete`

#if CONFIG_SHELL_KEY_SEQ_BIND

const static key_seq_t key_seq_map[] = {
#if CONFIG_SHELL_HIST_MIN_RECORD > 0
  {CONVERT_KEY_SEQ('\033', '[', 'A', 0), rl_get_pre_history},   // up arrow
  {CONVERT_KEY_SEQ('\033', '[', 'B', 0), rl_get_next_history},  // down arrow
  // {CONVERT_KEY_SEQ('\033', 'O', 'A', 0), rl_get_pre_history},
  // {CONVERT_KEY_SEQ('\033', 'O', 'B', 0), rl_get_next_history},

#endif /* CONFIG_SHELL_HIST_MIN_RECORD */


#if CONFIG_SHELL_LINE_EDITING
  {CONVERT_KEY_SEQ('\033', '[', 'C', 0), rl_backward_cursor},  // right arrow
  {CONVERT_KEY_SEQ('\033', '[', 'D', 0), rl_forward_cursor},   // left arrow
  {CONVERT_KEY_SEQ('\033', '[', 'H', 0), rl_line_home},        // home
  {CONVERT_KEY_SEQ('\033', '[', 'F', 0), rl_line_end},         // end
  // {CONVERT_KEY_SEQ('\033', 'O', 'C', 0), rl_forward_cursor},
  // {CONVERT_KEY_SEQ('\033', 'O', 'D', 0), rl_backward_cursor},
  // {CONVERT_KEY_SEQ('\033', 'O', 'H', 0), rl_line_home},
  // {CONVERT_KEY_SEQ('\033', 'O', 'F', 0), rl_line_end},

  {CONVERT_KEY_SEQ('\033', '[', '3', '~'), rl_delete},  // delete
#endif /* CONFIG_SHELL_LINE_EDITING */

};

#define KEY_SEQ_MAP_SIZE (sizeof(key_seq_map) / sizeof(key_seq_t))


extern int _rl_key_seq_len;

void rl_dispatch_seq(char ch)
{
  static uint32_t key_seq_val, key_seq_mask;

  uint32_t offset, miss_match, tmp_val;

  _rl_key_seq_len++;

  offset = ((uint32_t)(sizeof(uint32_t) - _rl_key_seq_len)) << 3;  // (4-_rl_key_seq_len)*8
  key_seq_val |= (((uint32_t)ch) << offset);
  key_seq_mask |= (0xFF << offset);

  miss_match = 1;
  for (int i = 0; i < KEY_SEQ_MAP_SIZE; i++) {
    tmp_val = key_seq_map[i].key_seq_val;
    if ((tmp_val & key_seq_mask) == key_seq_val) {  // partial match
      if (key_seq_val == tmp_val) {                 // full match
        key_seq_val = 0;
        key_seq_mask = 0;
        _rl_key_seq_len = 0;

        key_seq_map[i].key_func();
        return;
      }
      miss_match = 0;
    }
  }

  if (miss_match) {
    key_seq_val = 0;
    key_seq_mask = 0;
    _rl_key_seq_len = 0;
    shell_putc('\a');
  }
}
#endif /* CONFIG_SHELL_KEY_SEQ_BIND */


#endif /* __NANO_SHELL_KEY_SEQ_MAP_H */
