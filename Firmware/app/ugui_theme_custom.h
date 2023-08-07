#pragma once

// This file should only be included in ugui.h

#include "ugui_colors.h"

#define C_DESKTOP_COLOR                0x5E8BEF
#define C_FORE_COLOR                   C_WHITE
#define C_BACK_COLOR                   C_BLACK
#define C_TITLE_FORE_COLOR             C_WHITE
#define C_TITLE_BACK_COLOR             C_BLUE
#define C_INACTIVE_TITLE_FORE_COLOR    C_WHITE
#define C_INACTIVE_TITLE_BACK_COLOR    C_GRAY

#define C_PAL_WINDOW                   /* Frame 0 */ \
                                       C_WHITE_39,   \
                                       C_WHITE_39,   \
                                       C_WHITE_39,   \
                                       C_WHITE_39,   \
                                       /* Frame 1 */ \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       /* Frame 2 */ \
                                       C_BLACK,      \
                                       C_BLACK,      \
                                       C_BLACK,   \
                                       C_BLACK

#define C_PAL_BUTTON_PRESSED           /* Frame 0 */ \
                                       C_WHITE,   \
                                       C_WHITE,   \
                                       C_WHITE,   \
                                       C_WHITE,   \
                                       /* Frame 1 */ \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       /* Frame 2 */ \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK,   \
                                       C_BLACK
#define C_PAL_BUTTON_RELEASED          C_PAL_WINDOW

#define C_PAL_CHECKBOX_PRESSED         C_PAL_BUTTON_PRESSED
#define C_PAL_CHECKBOX_RELEASED        C_PAL_BUTTON_RELEASED

#define C_PAL_PROGRESS                 C_PAL_BUTTON_RELEASED