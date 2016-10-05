#ifndef __FT81X_CO_PROCESSOR_H
#define __FT81X_CO_PROCESSOR_H

#include "ft81x_config.h"

#include "ft81x.h"

// ----------------------------------------------------------------------------
// Option flags
// ----------------------------------------------------------------------------
#define FT81X_COPROC_OPTION_FLAT            0x0100
#define FT81X_COPROC_OPTION_SIGNED          0x0100
#define FT81X_COPROC_OPTION_CENTREX         0x0200
#define FT81X_COPROC_OPTION_CENTREY         0x0400
#define FT81X_COPROC_OPTION_CENTRE          ((FT81X_COPROC_OPTION_CENTREX) | (FT81X_COPROC_OPTION_CENTREY))
#define FT81X_COPROC_OPTION_RIGHTX          0x0800
#define FT81X_COPROC_OPTION_NOBACK          0x1000
#define FT81X_COPROC_OPTION_NOTICKS         0x2000
#define FT81X_COPROC_OPTION_NOHM            0x4000
#define FT81X_COPROC_OPTION_NOPOINTER       0x4000
#define FT81X_COPROC_OPTION_NOSECS          0x8000
#define FT81X_COPROC_OPTION_NOHANDS         ((FT81X_COPROC_OPTION_NOSECS) | (FT81X_COPROC_OPTION_NOHM))

// ----------------------------------------------------------------------------
// Enums
// ----------------------------------------------------------------------------
typedef enum
{
    FT81X_COPROC_SPINNER_STYLE_CIRCLE       = 0,
    FT81X_COPROC_SPINNER_STYLE_LINE         = 1,
    FT81X_COPROC_SPINNER_STYLE_CLOCK_HAND   = 2,
    FT81X_COPROC_SPINNER_STYLE_ORBIT        = 3,
} FT81X_Coproc_Spinner_Style;

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------
// Starts a new display list
ft81x_result ft81x_coproc_cmd_dlstart(FT81X_Handle *handle);

// Swaps the display list (uses REG_DLSWAP)
ft81x_result ft81x_coproc_cmd_swap(FT81X_Handle *handle);

// Sets the background colour of widgets
ft81x_result ft81x_coproc_cmd_bgcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b);

// Sets the foreground colour of widgets
ft81x_result ft81x_coproc_cmd_fgcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b);

// Draws a gradient on the screen
// Should be used with CMD_SCISSOR_XY and CMD_SCISSOR_SIZE for the desired effect
// interpolates a gradient between fictional points x0,y0 and x1,y1
// and the colours r0,g0,b0, and r1,g1,b1
// if you want a horizontal gradient set y0 = y1.
//             a vertical gradient set x0 = x1.
//             a mostly horizontal but slight vertical gradient
//               set x0 = 0, y0 = 0, x1 = 300, y1 = 10
ft81x_result ft81x_coproc_cmd_gradient(FT81X_Handle *handle, uint16_t x0, uint16_t y0, uint8_t r0, uint8_t g0, uint8_t b0, uint16_t x1, uint16_t y1, uint8_t r1, uint8_t g1, uint8_t b1);

// Draws font to the screen
// Note: this should be used through the Text Manager and not directly
//  x       - x co-ord of text (see options)
//  y       - y co-ord of text (see options)
//  font_id - bitmap id for font (0-31)
//  options - how to align the text with the x,y co-ords
//            see FT81X_COPROC_OPTION_CENTRE... and FT81X_COPROC_OPTION_RIGHTX
//  str     - NULL terminated ascii string
ft81x_result ft81x_coproc_cmd_text(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t font_id, uint16_t options, const char *str);

// Draws a button to the screen
//  x       - x co-ord of button
//  y       - y co-ord of button
//  width   - width of button
//  height  - height of button
//  font_id - bitmap id for font (0-31)
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  str     - button label (NULL terminated ascii string)
ft81x_result ft81x_coproc_cmd_button(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font_id, uint16_t options, const char *str);

// Draws a set of keys horizontally access the screen
//  x       - x co-ord of top left of keys
//  y       - y co-ord of top left of keys
//  width   - width of keys (in total, not each)
//  height  - height of keys
//  font_id - bitmap id for font (0-31)
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//            FT81X_COPROC_OPTION_CENTRE keys are drawn at the minimum size
//                                       (based on font) in the centre of
//                                       the x,y, x+width, y+height rectangle
//            ascii code if an ascii code is specified, that key is shown as pressed.
//            IE. FT81X_COPROC_OPTION_FLAT | 'a' draws the keys flat and shows 'a' as pressed
//  str     - key labels (one per key) (NULL terminated ascii string)
ft81x_result ft81x_coproc_cmd_keys(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font_id, uint16_t options, const char *str);

// Draws a progress bar to the screen
//  x       - top left co-ord
//  y       - top left co-ord
//  width   - width in pixels
//  height  - height in pixels
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  val     - value to show between 0 - range
//  range   - maximum value
ft81x_result ft81x_coproc_cmd_progress(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t range);

// Draws a slider bar to the screen
//  x       - top left co-ord
//  y       - top left co-ord
//  width   - width in pixels
//  height  - height in pixels
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  val     - value to show between 0 - range
//  range   - maximum value
ft81x_result ft81x_coproc_cmd_slider(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t range);

// Draws a scrollbar to the screen
//  x       - top left co-ord
//  y       - top left co-ord
//  width   - width in pixels
//  height  - height in pixels
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  val     - value to show between 0 - range
//  size    - how far the bar extends
//  range   - maximum value
ft81x_result ft81x_coproc_cmd_scrollbar(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t size, uint16_t range);

// Draws a toggle to the screen
//  y       - y co-ord of top left of toggle
//  x       - x co-ord of top left of toggle
//  width   - width of toggle
//  font_id - bitmap id for font (0 - 31)
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  state   - 0 = off, anything else = on
//  str     - labels (NULL terminated ascii string)
//            the two state labels are seperated with '\xff'
//            e.g "off\xffon"
ft81x_result ft81x_coproc_cmd_toggle(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint8_t font_id, uint16_t options, uint16_t state, const char *str);

// Draws a gauge to the screen
//  x       - x co-ord of gauge
//  y       - y co-ord of gauge
//  radius  - radius of the gauge
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//            FT81X_COPROC_OPTION_NOBACK for no background
//            FT81X_COPROC_OPTION_NOTICKS for no tick marks
//            FT81X_COPROC_OPTION_NOPOINTER for no hand
//  major   - number of major subdivisions on the gauge (1-10)
//  minor   - number of minor subdivisions per major tick  (1-10)
//  val     - value indicated between 0 - range
//  range   - maximum value
ft81x_result ft81x_coproc_cmd_gauge(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range);

// Draw a clock to the screen
//  x       - x co-ord of clock
//  y       - y co-ord of clock
//  radius  - radius of the clock
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//            FT81X_COPROC_OPTION_NOBACK for no background
//            FT81X_COPROC_OPTION_NOTICKS for no tick marks
//            FT81X_COPROC_OPTION_NOHM for no hour or minute hands
//            FT81X_COPROC_OPTION_NOSECS for no second hand
//            FT81X_COPROC_OPTION_NOHANDS for no hands
//  hours   - time in hours (0 - 12)
//  minutes - time in minutes (0 - 59)
//  seconds - time in seconds (0 - 59)
//  milliseconds - time in ms (0 - 1000)
ft81x_result ft81x_coproc_cmd_clock(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t milliseconds);

// Calibrate the touch screen
// Note: Only for resistive touch.
//       but can be used with capacitive touch in compatability mode
ft81x_result ft81x_coproc_cmd_calibrate(FT81X_Handle *handle);

// Shows a "thinking" animation on the screen in one of several styles
// until CMD_STOP is sent
// Note: Can't be used with CMD_SCREENSAVER or CMD_SKETCH
//  x, y    - top left of spinner
//  style   - type of spinner to show (see enum FT81X_Coproc_Spinner_Style)
//  scale   - size of spinner, probably should be 0, 1 or 2
ft81x_result ft81x_coproc_cmd_spinner(FT81X_Handle *handle, uint16_t x, uint16_t y, FT81X_Coproc_Spinner_Style style, uint16_t scale);

// Stops the update of REG_MACRO_0 and REG_MACRO_1
// IE. It stops the current screensave, spinner or sketch
ft81x_result ft81x_coproc_cmd_stop(FT81X_Handle *handle);

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

// Sets the co-processors transformation matrix to the identity matrix
// NOTE: This doesn't affect anything until CMD_SETMATRIX is used
ft81x_result ft81x_coproc_cmd_loadidentity(FT81X_Handle *handle);

// Add a translation to the current co-processor matrix
// NOTE1: Only works for images.
// NOTE2: If the image does not fit in the rectangle defined by:
//          vertex2f / vertex2ii co-ord and
//          vertex2f / vertex2ii co-ord + original image width, height
//        Then only the section of the image that is in that rectangle
//          get's drawn.
//        Unless the BITMAP_SIZE DL instruction is used
// NOTE3: This doesn't affect anything until CMD_SETMATRIX is used
//  x,y - in pixels
ft81x_result ft81x_coproc_cmd_translate_double(FT81X_Handle *handle, double x, double y);

// Add a translation to the current co-processor matrix
// NOTE1: Only works for images.
// NOTE2: If the image does not fit in the rectangle defined by:
//          vertex2f / vertex2ii co-ord and
//          vertex2f / vertex2ii co-ord + original image width, height
//        Then only the section of the image that is in that rectangle
//          get's drawn.
//        Unless the BITMAP_SIZE DL instruction is used
// NOTE3: This doesn't affect anything until CMD_SETMATRIX is used
//  x,y - in pixels
ft81x_result ft81x_coproc_cmd_translate_int(FT81X_Handle *handle, int32_t x, int32_t y);

// Add a scale to the current co-processor matrix
// NOTE1: Only works for images.
// NOTE2: If the image does not fit in the rectangle defined by:
//          vertex2f / vertex2ii co-ord and
//          vertex2f / vertex2ii co-ord + original image width, height
//        Then only the section of the image that is in that rectangle
//          get's drawn.
//        Unless the BITMAP_SIZE DL instruction is used
// NOTE3: This doesn't affect anything until CMD_SETMATRIX is used
//  x,y - in pixels
ft81x_result ft81x_coproc_cmd_scale_double(FT81X_Handle *handle, double x, double y);

// Add a rotation to the current co-processor matrix
// NOTE1: Only works for images.
// NOTE2: If the image does not fit in the rectangle defined by:
//          vertex2f / vertex2ii co-ord and
//          vertex2f / vertex2ii co-ord + original image width, height
//        Then only the section of the image that is in that rectangle
//          get's drawn.
//        Unless the BITMAP_SIZE DL instruction is used
// NOTE3: This doesn't affect anything until CMD_SETMATRIX is used
//  degrees - clockwise rotation in degrees
ft81x_result ft81x_coproc_cmd_rotate_double(FT81X_Handle *handle, double degrees);

// Add a rotation to the current co-processor matrix
// NOTE1: Only works for images.
// NOTE2: If the image does not fit in the rectangle defined by:
//          vertex2f / vertex2ii co-ord and
//          vertex2f / vertex2ii co-ord + original image width, height
//        Then only the section of the image that is in that rectangle
//          get's drawn.
//        Unless the BITMAP_SIZE DL instruction is used
// NOTE3: This doesn't affect anything until CMD_SETMATRIX is used
//  degrees - clockwise rotation in degrees
ft81x_result ft81x_coproc_cmd_rotate_int(FT81X_Handle *handle, int32_t degrees);

// Writes the co-processor matrix to the display list
// The transformations don't affect anything before this command
ft81x_result ft81x_coproc_cmd_setmatrix(FT81X_Handle *handle);

// Set up a custom font
// Note: this should be used through the Text Manager and not directly
//  font_id                     - The bitmap id to use for font (0 - 31)
//  font_metric_block_offset    - The offset into g_ram of the font metric block
ft81x_result ft81x_coproc_cmd_setfont(FT81X_Handle *handle, uint8_t font_id, uint32_t font_metric_block_offset);

// Draw a rotary dial to the screen
//  x       - x co-ord of clock
//  y       - y co-ord of clock
//  radius  - radius of the clock
//  options - FT81X_COPROC_OPTION_FLAT for no 3D effect
//  val     - 0 = down, 0x4000 = left, 0x8000 = up, 0xC000 = right
ft81x_result ft81x_coproc_cmd_dial(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t val);

// Draw a number to the screen
// Note: this should be used through the Text Manager and not directly
//  x       - x co-ord of number
//  y       - y co-ord of number
//  font_id - bitmap id for font (0-31)
//  options - FT81X_COPROC_OPTION_SIGNED for signed number
//            FT81X_COPROC_OPTION_CENTREX
//            FT81X_COPROC_OPTION_CENTREY
//            FT81X_COPROC_OPTION_CENTRE
//            FT81X_COPROC_OPTION_RIGHTX
//  num     - the number to display 32 bit, signed or unsigned (depending on options
ft81x_result ft81x_coproc_cmd_number(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t font_id, uint16_t options, int32_t num);

// Updates REG_MACRO_0 with a VERTEX2F DL instruction
// with varying x and y co-ordinates.
// This allows you to move a bitmap around the screen with no MCU work
// until CMD_STOP is sent
// Note: Can't be used with CMD_SPINNER or CMD_SKETCH
ft81x_result ft81x_coproc_cmd_screensaver(FT81X_Handle *handle);

// Show the FTDI logo animation
ft81x_result ft81x_coproc_cmd_logo(FT81X_Handle *handle);

// Sets the co-proc to default reset states
ft81x_result ft81x_coproc_cmd_coldstart(FT81X_Handle *handle);

// Reads the transformation matrix values A-F into the command buffer
ft81x_result ft81x_coproc_cmd_getmatrix(FT81X_Handle *handle);

// Change the gradient colour for buttons and keys
// A button or key starts with the gradient colour at the top
// and fades to the fg colour at the bottom
ft81x_result ft81x_coproc_cmd_gradcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b);

// Set the base for use with CMD_NUMBER
//  base - the base to output in 2-36
ft81x_result ft81x_coproc_cmd_setbase(FT81X_Handle *handle, uint8_t base);

// Set up a custom font
// Note: this should be used through the Text Manager and not directly
//  font_id                     - The bitmap id to use for font (0 - 31)
//  font_metric_block_offset    - The offset into g_ram of the font metric block
//  first_char                  - The first character in your custom font image, probably ' '
ft81x_result ft81x_coproc_cmd_setfont2(FT81X_Handle *handle, uint8_t font_id, uint32_t font_metric_block_offset, char first_char);

// Remap a romfont to a different bitmap_handle
// Note: this should be used through the Text Manager and not directly
//  bitmap_handle - bitmap id to use for this font (0-31)
//  rom_font_id   - font id for an inbuilt font (16-34)
ft81x_result ft81x_coproc_cmd_romfont(FT81X_Handle *handle, uint8_t bitmap_handle, uint8_t rom_font_id);

#endif
