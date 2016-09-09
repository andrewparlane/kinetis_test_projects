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

#define FT81X_DL_CMD_ID_DISPLAY             0x00
#define FT81X_DL_CMD_ID_CLEAR_COLOUR_RGB    0x02
#define FT81X_DL_CMD_ID_COLOUR_RGB          0x04
#define FT81X_DL_CMD_ID_POINT_SIZE          0x0D
#define FT81X_DL_CMD_ID_LINE_WIDTH          0x0E
#define FT81X_DL_CMD_ID_BEGIN               0x1F
#define     FT81X_DL_PRIM_BITMAP                    1
#define     FT81X_DL_PRIM_POINTS                    2
#define     FT81X_DL_PRIM_LINES                     3
#define     FT81X_DL_PRIM_LINE_STRIP                4
#define     FT81X_DL_PRIM_EDGE_STRIP_R              5
#define     FT81X_DL_PRIM_EDGE_STRIP_L              6
#define     FT81X_DL_PRIM_EDGE_STRIP_A              7
#define     FT81X_DL_PRIM_EDGE_STRIP_B              8
#define     FT81X_DL_PRIM_RECTS                     9
#define FT81X_DL_CMD_ID_END                 0x21
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
//  prim - should be one of FT81X_DL_PRIM_...
#define FT81X_DL_CMD_BEGIN(prim) \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_BEGIN), \
                              (((prim) & 0x0F) << 0))

// End drawing a graphics primitive
#define FT81X_DL_CMD_END() \
            FT81X_DL_8BIT_CMD((FT81X_DL_CMD_ID_END), \
                              0)
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
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&4194303UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define BITMAP_HANDLE(handle) ((5UL<<24)|(((handle)&31UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
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
#define COLOUR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define VERTEX_FORMAT(frac) ((39UL<<24)|(((frac)&7UL)<<0))
#define BITMAP_LAYOUT_H(linestride,height) ((40UL<<24)|(((linestride)&3UL)<<2)|(((height)&3UL)<<0))
#define BITMAP_SIZE_H(width,height) ((41UL<<24)|(((width)&3UL)<<2)|(((height)&3UL)<<0))
#define PALETTE_SOURCE(addr) ((42UL<<24)|(((addr)&4194303UL)<<0))
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
