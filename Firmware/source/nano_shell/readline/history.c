/**
 * @file history.c
 * @author Liber (lvlebin@outlook.com)
 * @brief history manager
 * @version 1.0
 * @date 2020-03-18
 *
 * @copyright Copyright (c) Liber 2020
 *
 */

#include <string.h>

#include "history.h"
#include "shell_config.h"

#if CONFIG_SHELL_HIST_MIN_RECORD > 0

typedef unsigned char len_t;

/**
memory view of the history buffer:

-------------------------------------------------------------------------------------------
|length| string |length| *** |length| string |length|                                     |
|<---- 1st record ---->| *** |<----   record   ---->|                                     |
^                            ^                      ^                                     ^
&buffer[0]                   cursor                 tail                                  buffer end
                                                    (new record will be saved here)
-------------------------------------------------------------------------------------------

Let the data type of `length` be `len_t`,
then the value of length: length = strlen(string) + 1 + 2*sizeof(len_t)
*/

#define TOTAL_BUFFER_SIZE \
  (CONFIG_SHELL_HIST_MIN_RECORD * (CONFIG_SHELL_INPUT_BUFFSIZE + 1 + 2 * sizeof(len_t)))

static char historyBuffer[TOTAL_BUFFER_SIZE];  // history buffer

#define HISTORY_BUFFER_BEGIN() (&historyBuffer[0])
#define HISTORY_BUFFER_END() (&historyBuffer[TOTAL_BUFFER_SIZE])

#define GET_RECORD_SIZE(pRecord) ((len_t)(*((len_t *)(pRecord))))

static char *historyCursor = HISTORY_BUFFER_BEGIN();
static char *historyTail = HISTORY_BUFFER_BEGIN();  // new record will be saved here


char *rl_history_next(void)
{
  if (historyCursor >= historyTail ||                                // cursor point to the tail
      historyCursor + GET_RECORD_SIZE(historyCursor) >= historyTail  // cursor point to the last one
  ) {
    return NULL;
  }

  historyCursor += GET_RECORD_SIZE(historyCursor);
  return historyCursor + sizeof(len_t);
}


char *rl_history_prev(void)
{
  if (historyTail != HISTORY_BUFFER_BEGIN() &&  // buffer is not empty
      historyCursor > HISTORY_BUFFER_BEGIN()    // cursor does not point to the first
  ) {
    historyCursor -= GET_RECORD_SIZE(historyCursor - sizeof(len_t));
    return historyCursor + sizeof(len_t);
  }

  return NULL;
}


void rl_history_add(char *input)
{
  size_t freeSpace = HISTORY_BUFFER_END() - historyTail;

  len_t inputLength = strlen(input) + 1;
  len_t newRecordLength = inputLength + 2 * sizeof(len_t);

  if (freeSpace < newRecordLength) {
    len_t tmpLength;
    char *tmpRecord = HISTORY_BUFFER_BEGIN();
    do {
      tmpLength = GET_RECORD_SIZE(tmpRecord);
      freeSpace += tmpLength;
      tmpRecord += tmpLength;
    } while (freeSpace < newRecordLength);

    memmove(HISTORY_BUFFER_BEGIN(), tmpRecord, historyTail - tmpRecord);
    historyTail -= (tmpRecord - HISTORY_BUFFER_BEGIN());
  }

  /* put the new record in the history buffer */
  *((len_t *)historyTail) = newRecordLength;
  memcpy(historyTail + sizeof(len_t), input, inputLength);
  historyTail += newRecordLength;  // move tail to the end of the new record
  *((len_t *)(historyTail - sizeof(len_t))) = newRecordLength;

  /* set cursor point to the end */
  historyCursor = historyTail;
}


void rl_history_rm_last(void)
{
  if (historyTail > HISTORY_BUFFER_BEGIN()) {
    historyTail -= GET_RECORD_SIZE(historyTail - sizeof(len_t));
    historyCursor = historyTail;
  }
}

#endif /* CONFIG_SHELL_HIST_MIN_RECORD > 0 */
