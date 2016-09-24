#ifndef __FT81X_CO_PROCESSOR_H
#define __FT81X_CO_PROCESSOR_H

#include "ft81x_config.h"

// if we write directly to DL RAM
// then we can't use any of these commands
#ifndef FT81X_DL_WRITE_DIRECTLY_TO_DL_RAM

#include "ft81x.h"

// ----------------------------------------------------------------------------
// Option flags
// ----------------------------------------------------------------------------
#define FT81X_COPROC_OPTION_CENTREX         0x0200
#define FT81X_COPROC_OPTION_CENTREY         0x0400
#define FT81X_COPROC_OPTION_CENTRE          ((FT81X_COPROC_OPTION_CENTREX) | (FT81X_COPROC_OPTION_CENTREY))
#define FT81X_COPROC_OPTION_RIGHTX          0x0800

// Starts a new display list
ft81x_result ft81x_coproc_cmd_dlstart(FT81X_Handle *handle);

// Swaps the display list (uses REG_DLSWAP)
ft81x_result ft81x_coproc_cmd_swap(FT81X_Handle *handle);

// Draws font to the screen
//  x    - x co-ord of text (see options)
//  y    - y co-ord of text (see options)
//  font - bitmap id for font 0-31 (0-14 are for custom RAM fonts)
//                                    (15 is for scratch)
//                                    (16-31 are ROM fonts)
//                                    (there are also 32 - 34, see CMD_ROMFONT)
//  options - how to align the text with the x,y co-ords
//            see FT81X_COPROC_OPTION_CENTRE... and FT81X_COPROC_OPTION_RIGHTX
//  str     - NULL terminated ascii string
ft81x_result ft81x_coproc_cmd_text(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t font_id, uint16_t options, char *str);

// Decompresses data that has been compressed with the DEFLATE algorithm
// for example using the zlib library.
// Note: This should be used in conjunction with the G_RAM manager.
//       Which for now means you need to know an upper bound on decompress size.
//  offset          - the address in g_ram to decompress to
//  compressed_data - the data
//  count           - the size of the compressed_data array
ft81x_result ft81x_coproc_cmd_inflate(FT81X_Handle *handle, uint32_t offset, uint32_t count, const uint8_t *compressed_data);

// Get the end memory address of data inflated using CMD_INFLATE
ft81x_result ft81x_coproc_cmd_getptr(FT81X_Handle *handle);

// Show the FTDI logo animation
ft81x_result ft81x_coproc_cmd_logo(FT81X_Handle *handle);

#endif

#endif
