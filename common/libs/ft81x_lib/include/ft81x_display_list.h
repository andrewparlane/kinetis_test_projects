/*

Copyright (c) Future Technology Devices International 2014

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : FTDI

Revision History:
0.1 - date 2013.04.24 - Initial Version
0.2 - date 2013.08.19 - added few registers.
1.0 - date 2013.12.10 - Version 1.0.
2.0 - date 2014.11.24 - version 2.0 which includes support for FT81x series.


*/

#ifndef __FT81X_DISPLAY_LIST_H
#define __FT81X_DISPLAY_LIST_H

typedef enum
{
    FT81X_BITMAP_FORMAT_ARGB1555        = 0,
    FT81X_BITMAP_FORMAT_L1              = 1,
    FT81X_BITMAP_FORMAT_L4              = 2,
    FT81X_BITMAP_FORMAT_L8              = 3,
    FT81X_BITMAP_FORMAT_RGB332          = 4,
    FT81X_BITMAP_FORMAT_ARGB2           = 5,
    FT81X_BITMAP_FORMAT_ARGB4           = 6,
    FT81X_BITMAP_FORMAT_RGB565          = 7,
    FT81X_BITMAP_FORMAT_TEXT8X8         = 9,
    FT81X_BITMAP_FORMAT_TEXTVGA         = 10,
    FT81X_BITMAP_FORMAT_BARGRAPH        = 11,
    FT81X_BITMAP_FORMAT_PALETTED565     = 14,
    FT81X_BITMAP_FORMAT_PALETTED4444    = 15,
    FT81X_BITMAP_FORMAT_PALETTED8       = 16,
    FT81X_BITMAP_FORMAT_L2              = 17
} FT81X_Bitmap_Format;

typedef enum
{
    FT81X_BITMAP_FILTER_NEAREST         = 0,
    FT81X_BITMAP_FILTER_BILINEAR        = 1
} FT81X_Bitmap_Filter;

typedef enum
{
    FT81X_BITMAP_WRAP_BORDER            = 0,
    FT81X_BITMAP_WRAP_REPEAT            = 1
} FT81X_Bitmap_Wrap;

typedef enum
{
    FT81X_BLEND_FUNC_ZERO                   = 0,
    FT81X_BLEND_FUNC_ONE                    = 1,
    FT81X_BLEND_FUNC_SRC_ALPHA              = 2,
    FT81X_BLEND_FUNC_DST_ALPHA              = 3,
    FT81X_BLEND_FUNC_ONE_MINUS_SRC_ALPHA    = 4,
    FT81X_BLEND_FUNC_ONE_MINUS_DST_ALPHA    = 5
} FT81X_Blend_Func;

typedef enum
{
    FT81X_PRIMITIVE_BITMAP          = 1,
    FT81X_PRIMITIVE_POINTS          = 2,
    FT81X_PRIMITIVE_LINES           = 3,
    FT81X_PRIMITIVE_LINE_STRIP      = 4,
    FT81X_PRIMITIVE_EDGE_STRIP_R    = 5,
    FT81X_PRIMITIVE_EDGE_STRIP_L    = 6,
    FT81X_PRIMITIVE_EDGE_STRIP_A    = 7,
    FT81X_PRIMITIVE_EDGE_STRIP_B    = 8,
    FT81X_PRIMITIVE_RECTS           = 9
} FT81X_Primitive;

#define FT81X_DL_CMD_ID_DISPLAY             0x00
#define FT81X_DL_CMD_ID_BITMAP_SOURCE       0x01
#define FT81X_DL_CMD_ID_CLEAR_COLOUR_RGB    0x02
#define FT81X_DL_CMD_ID_COLOUR_RGB          0x04
#define FT81X_DL_CMD_ID_BITMAP_HANDLE       0x05
#define FT81X_DL_CMD_ID_BITMAP_LAYOUT       0x07
#define FT81X_DL_CMD_ID_BITMAP_SIZE         0x08
#define FT81X_DL_CMD_ID_BLEND_FUNC          0x0B
#define FT81X_DL_CMD_ID_POINT_SIZE          0x0D
#define FT81X_DL_CMD_ID_LINE_WIDTH          0x0E
#define FT81X_DL_CMD_ID_BEGIN               0x1F
#define FT81X_DL_CMD_ID_COLOUR_MASK         0x20
#define FT81X_DL_CMD_ID_END                 0x21
#define FT81X_DL_CMD_ID_PALETTE_SOURCE      0x2A
#define FT81X_DL_CMD_ID_CLEAR               0x26
#define FT81X_DL_CMD_ID_VERTEX2II           0x80

// ----------------------------------------------------------------------------
// generic command macros
// ----------------------------------------------------------------------------
// 2 bit command (2 most significant bits)
#define FT81X_DL_2BIT_CMD(cmd, params) \
            ((((cmd) & 0xC0) << 24) | \
             ((params) & 0x3FFFFFFF))

// 8 bit command (MSB)
#define FT81X_DL_8BIT_CMD(cmd, params) \
            ((((cmd) & 0xFF) << 24) | \
             ((params) & 0x00FFFFFF))

// End of display lst
#define FT81X_DL_CMD_DISPLAY() \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_DISPLAY), \
                              0)

// Specify the source address in g_ram of a bitmap
//  addr - address in g_ram of the bitmap
//         (must be aligned to the bitmap format, ie RGB565 means 2 byte alignment)
#define FT81X_DL_CMD_BITMAP_SOURCE(addr) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BITMAP_SOURCE), \
                              ((addr) & 0x3FFFFF))

// Set the clear colour for the RGB channels
#define FT81X_DL_CMD_CLEAR_COLOUR_RGB(red, green, blue) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_CLEAR_COLOUR_RGB), \
                              ((((red)   & 0xFF) << 16) | \
                               (((green) & 0xFF) <<  8) | \
                               (((blue)  & 0xFF) <<  0)))

// Set the current colour
#define FT81X_DL_CMD_COLOUR_RGB(red, green, blue) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_COLOUR_RGB), \
                              ((((red)   & 0xFF) << 16) | \
                               (((green) & 0xFF) <<  8) | \
                               (((blue)  & 0xFF) <<  0)))

// Specify the bitmap handle
//  handle - (0-31) An ID associated with this bitmap
//           which can be used by vertex2i calls
//           by default 15 is for scratch used by co-proc cmds
//           and 16-31 are built in fonts.
#define FT81X_DL_CMD_BITMAP_HANDLE(handle) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BITMAP_HANDLE) , \
                              ((handle) & 0x1F))

// Specify the bitmap layout for the current handle (see BITMAP_HANDLE)
//  format      - type of bitmap (one of FT81X_Bitmap_Format)
//  linestride  - width of image in bytes
//  height      - height in lines
#define FT81X_DL_CMD_BITMAP_LAYOUT(format, linestride, height) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BITMAP_LAYOUT) , \
                              ((((format)       & 0x1F) << 19) | \
                               (((linestride)   & 0x3FF) << 9) | \
                               (((height)       & 0x1FF) << 0)))

// Specify how to draw bitmaps for the current handle (see BITMAP_HANDLE)
//  filter - one of FT81X_Bitmap_Filter
//  wrapx - one of FT81X_Bitmap_Wrap
//  wrapy - one of FT81X_Bitmap_Wrap
//  width - what width to draw in pixels
//  height - what height to draw in pixels
#define FT81X_DL_CMD_BITMAP_SIZE(filter, wrapx, wrapy, width, height) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BITMAP_SIZE) , \
                              ((((filter)  & 0x1)   << 20) | \
                               (((wrapx)   & 0x1)   << 19) | \
                               (((wrapy)   & 0x1)   << 18) | \
                               (((width)   & 0x1FF) << 9)  | \
                               (((height)  & 0x1FF) << 0)))

// Specify how to blend pixels
//  src - specifies how the source blending factor is computed
//  dst - specifies how the destination blending factor is computed
//   src and dst should be one of FT81X_Blend_Func
//   given a pixel value: source
//   and a previous value in the colour beffer: destination
//   new colour = source * src + destination * dst
#define FT81X_DL_CMD_BLEND_FUNC(src, dst) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BLEND_FUNC) , \
                              ((((src)  & 0x07)     << 3) | \
                               (((dst)  & 0x07)     << 0)))

// Specify the radius of a point
//  size - radius in 1/16ths of pixels
#define FT81X_DL_CMD_POINT_SIZE(size) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_POINT_SIZE), \
                              (((size) & 0x1FFF) << 0))

// Specify the width of a line
//  size - width in 1/16ths of pixels
#define FT81X_DL_CMD_LINE_WIDTH(size) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_LINE_WIDTH), \
                              (((size) & 0xFFF) << 0))

// Begin drawing a graphics primitive
//  prim - should be one of FT81X_Primitive
#define FT81X_DL_CMD_BEGIN(prim) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BEGIN), \
                              (((prim) & 0x0F) << 0))

// Enables or disables drawing of colour components
//  r - 1 = enable red component
//  g - 1 = enable green component
//  b - 1 = enable blue component
//  a - 1 = enable alpha component
#define FT81X_DL_CMD_COLOUR_MASK(r, g, b, a) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_COLOUR_MASK), \
                              ((((r) & 0x1) << 3) | \
                               (((g) & 0x1) << 2) | \
                               (((b) & 0x1) << 1) | \
                               (((a) & 0x1) << 0)))

// End drawing a graphics primitive
#define FT81X_DL_CMD_END() \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_END), \
                              0)

// Specifies the base address of the palette
//  addr - specify the address of the palette in g_ram
#define FT81X_DL_CMD_PALETTE_SOURCE(addr) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_PALETTE_SOURCE), \
                              (((addr) & 0x3FFFFF) << 0))

// clear buffers
//  c - colour buffer
//  s - stencil buffer
//  t - tag buffer
#define FT81X_DL_CMD_CLEAR(c,s,t) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_CLEAR), \
                              ((((c) & 0x01) << 2) | \
                               (((s) & 0x01) << 1) | \
                               (((t) & 0x01) << 0)))

// Place a vertex of a graphics primitive
//  x      - x-cord in pixels
//  y      - y-cord in pixels
//  handle - bitmap handle (ignored for anything other than bitmap primitives)
//  cell   - cell number (ignored for anything other than bitmap primitives)
#define FT81X_DL_CMD_VERTEX2II(x, y, handle, cell) \
            FT81X_DL_2BIT_CMD((FT81X_DL_CMD_ID_VERTEX2II), \
                              ((((x)      & 0x1FF) << 21) | \
                               (((y)      & 0x1FF) << 12) | \
                               (((handle) &  0x1F) <<  7) | \
                               (((cell)   &  0x7F) << 0)))

/*
#define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define CLEAR_COLOUR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
#define COLOUR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&2047UL)<<11)|(((y)&2047UL)<<0))
#define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&4095UL)<<12)|(((height)&4095UL)<<0))
#define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
#define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
#define VERTEX_FORMAT(frac) ((39UL<<24)|(((frac)&7UL)<<0))
#define BITMAP_LAYOUT_H(linestride,height) ((40UL<<24)|(((linestride)&3UL)<<2)|(((height)&3UL)<<0))
#define BITMAP_SIZE_H(width,height) ((41UL<<24)|(((width)&3UL)<<2)|(((height)&3UL)<<0))
#define VERTEX_TRANSLATE_X(x) ((43UL<<24)|(((x)&131071UL)<<0))
#define VERTEX_TRANSLATE_Y(y) ((44UL<<24)|(((y)&131071UL)<<0))
#define NOP() ((45UL<<24))
#define END() ((33UL<<24))
#define SAVE_CONTEXT() ((34UL<<24))
#define RESTORE_CONTEXT() ((35UL<<24))
#define RETURN() ((36UL<<24))
#define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
*/

#endif
