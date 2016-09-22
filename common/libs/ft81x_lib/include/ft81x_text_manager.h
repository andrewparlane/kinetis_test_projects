#ifndef __FT81X_TEXT_MANAGER
#define __FT81X_TEXT_MANAGER

#include "ft81x.h"

#include <stdint.h>

// ----------------------------------------------------------------------------
// Built in font sizes (see datasheet table 4-8)
// ----------------------------------------------------------------------------
#define FT81X_TEXT_MANAGER_FONT_16_MAX_WIDTH    8
#define FT81X_TEXT_MANAGER_FONT_17_MAX_WIDTH    8
#define FT81X_TEXT_MANAGER_FONT_18_MAX_WIDTH    8
#define FT81X_TEXT_MANAGER_FONT_19_MAX_WIDTH    8
#define FT81X_TEXT_MANAGER_FONT_20_MAX_WIDTH    11
#define FT81X_TEXT_MANAGER_FONT_21_MAX_WIDTH    13
#define FT81X_TEXT_MANAGER_FONT_22_MAX_WIDTH    17
#define FT81X_TEXT_MANAGER_FONT_23_MAX_WIDTH    18
#define FT81X_TEXT_MANAGER_FONT_24_MAX_WIDTH    25
#define FT81X_TEXT_MANAGER_FONT_25_MAX_WIDTH    34
#define FT81X_TEXT_MANAGER_FONT_26_MAX_WIDTH    13
#define FT81X_TEXT_MANAGER_FONT_27_MAX_WIDTH    15
#define FT81X_TEXT_MANAGER_FONT_28_MAX_WIDTH    19
#define FT81X_TEXT_MANAGER_FONT_29_MAX_WIDTH    21
#define FT81X_TEXT_MANAGER_FONT_30_MAX_WIDTH    28
#define FT81X_TEXT_MANAGER_FONT_31_MAX_WIDTH    37
#define FT81X_TEXT_MANAGER_FONT_32_MAX_WIDTH    49
#define FT81X_TEXT_MANAGER_FONT_33_MAX_WIDTH    63
#define FT81X_TEXT_MANAGER_FONT_34_MAX_WIDTH    82

#define FT81X_TEXT_MANAGER_FONT_16_HEIGHT       8
#define FT81X_TEXT_MANAGER_FONT_17_HEIGHT       8
#define FT81X_TEXT_MANAGER_FONT_18_HEIGHT       16
#define FT81X_TEXT_MANAGER_FONT_19_HEIGHT       16
#define FT81X_TEXT_MANAGER_FONT_20_HEIGHT       13
#define FT81X_TEXT_MANAGER_FONT_21_HEIGHT       17
#define FT81X_TEXT_MANAGER_FONT_22_HEIGHT       20
#define FT81X_TEXT_MANAGER_FONT_23_HEIGHT       22
#define FT81X_TEXT_MANAGER_FONT_24_HEIGHT       29
#define FT81X_TEXT_MANAGER_FONT_25_HEIGHT       38
#define FT81X_TEXT_MANAGER_FONT_26_HEIGHT       16
#define FT81X_TEXT_MANAGER_FONT_27_HEIGHT       20
#define FT81X_TEXT_MANAGER_FONT_28_HEIGHT       25
#define FT81X_TEXT_MANAGER_FONT_29_HEIGHT       28
#define FT81X_TEXT_MANAGER_FONT_30_HEIGHT       36
#define FT81X_TEXT_MANAGER_FONT_31_HEIGHT       49
#define FT81X_TEXT_MANAGER_FONT_32_HEIGHT       63
#define FT81X_TEXT_MANAGER_FONT_33_HEIGHT       83
#define FT81X_TEXT_MANAGER_FONT_34_HEIGHT       108


// Note: If you want to write static test directly to the display list
//       You can use the EVE Screen editor tool.
//       Add a text widget to the display, choose your font and string.
//       Look at the inspector tab and copy the DL commands.
//       The vertex2ii call have the corecct spacing for each character.

ft81x_result ft81x_text_manager_write_fixed_width_font_string(FT81X_Handle *handle, uint8_t font_id, uint8_t font_width, uint8_t font_height, char *str, uint32_t x, uint32_t y);

#endif
