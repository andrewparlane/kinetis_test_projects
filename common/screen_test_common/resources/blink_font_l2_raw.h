#ifndef __BLINK_FONT_L2_RAW_H
#define __BLINK_FONT_L2_RAW_H

// Font converted from http://www.fontspace.com/cannot-into-space-fonts/blink
// using the BlinkUltraWide.otf version
// see "blink font SIL Open Font License.txt"

/*Command Line: fnt_cvt.exe -i BlinkUltraWide.otf -a -c setfont*/

#include "ft81x.h"
#include "ft81x_display_list.h"
#include "ft81x_text_manager.h"

#include <string.h>

/*('file properties ',
            'format ', 'L2',
            ' stride ', 5,
            ' width ', 20,
            'height', 20)*/

const uint8_t blink_font_l2_raw_character_widths[] =
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,6,4,7,14,10,17,14,4,
    6,6,7,9,4,7,4,8,10,7,
    8,9,10,9,10,8,10,10,4,4,
    8,8,8,9,12,12,10,10,10,8,
    8,11,10,5,8,12,8,12,11,11,
    10,12,10,11,10,10,10,19,12,10,
    9,7,8,7,7,8,6,10,10,8,
    10,8,8,10,9,5,7,10,7,14,
    9,9,10,10,8,7,8,9,9,17,
    10,9,8,7,4,7,8,20,
};

// For demonstration purposes I replaced the ~ with a smiley
const uint8_t blink_font_l2_raw[] =
{
/*   */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ! */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,62,0,0,0,0,62,0,0,0,0,62,0,0,0,0,61,0,0,0,0,61,0,0,0,0,61,0,0,0,0,20,0,0,0,0,62,0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* " */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,125,240,0,0,0,121,224,0,0,0,56,224,0,0,0,20,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* # */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,61,31,0,0,0,124,46,0,0,0,184,61,0,0,47,255,255,192,0,47,255,255,192,0,127,255,255,64,0,127,255,255,64,0,11,131,208,0,0,15,71,192,0,0,31,11,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* $ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,0,0,0,1,254,64,0,0,11,255,192,0,0,15,209,64,0,0,15,224,0,0,0,7,253,0,0,0,0,127,128,0,0,0,31,208,0,0,29,47,208,0,0,127,255,128,0,0,27,253,0,0,0,0,168,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* % */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,248,2,128,0,46,110,7,0,0,60,15,29,0,0,62,111,52,0,0,11,248,160,0,0,0,2,192,0,0,0,7,95,253,0,0,29,61,31,0,0,56,62,111,0,0,240,27,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* & */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,254,0,0,0,11,235,128,0,0,15,67,192,0,0,11,155,128,0,0,7,254,0,0,0,47,254,5,0,0,191,127,253,0,0,255,111,248,0,0,191,255,253,0,0,27,249,127,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ' */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,120,0,0,0,0,56,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ( */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,128,0,0,0,15,128,0,0,0,47,0,0,0,0,62,0,0,0,0,189,0,0,0,0,188,0,0,0,0,252,0,0,0,0,188,0,0,0,0,188,0,0,0,0,125,0,0,0,0,62,0,0,0,0,47,0,0,0,0,11,128,0,0,0,2,64,0,0,0,0,0,0,0,0,
/* ) */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,0,0,0,0,47,0,0,0,0,15,128,0,0,0,11,208,0,0,0,7,224,0,0,0,3,224,0,0,0,3,240,0,0,0,3,224,0,0,0,7,224,0,0,0,7,208,0,0,0,15,128,0,0,0,31,64,0,0,0,45,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* * */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,43,160,0,0,0,7,64,0,0,0,9,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* + */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,192,0,0,0,3,192,0,0,0,191,254,0,0,0,191,254,0,0,0,3,192,0,0,0,3,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* , */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,63,0,0,0,0,13,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* - */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,240,0,0,0,127,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* . */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* / */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,248,0,0,0,1,240,0,0,0,2,224,0,0,0,3,208,0,0,0,7,192,0,0,0,11,128,0,0,0,15,64,0,0,0,31,0,0,0,0,46,0,0,0,0,61,0,0,0,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,248,0,0,0,11,254,0,0,0,31,95,64,0,0,47,15,128,0,0,63,15,192,0,0,63,15,192,0,0,47,15,128,0,0,31,95,64,0,0,11,254,0,0,0,2,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 1 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,240,0,0,0,47,240,0,0,0,127,240,0,0,0,103,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 2 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,224,0,0,0,47,248,0,0,0,125,124,0,0,0,24,124,0,0,0,0,120,0,0,0,0,244,0,0,0,2,224,0,0,0,11,128,0,0,0,47,252,0,0,0,127,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 3 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,228,0,0,0,31,254,0,0,0,25,62,0,0,0,0,249,0,0,0,0,253,0,0,0,0,126,0,0,0,0,63,0,0,0,46,126,0,0,0,47,253,0,0,0,11,228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 4 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,0,0,0,1,255,0,0,0,2,255,0,0,0,7,191,0,0,0,11,127,0,0,0,31,63,0,0,0,47,255,224,0,0,127,255,224,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 5 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,252,0,0,0,7,252,0,0,0,11,0,0,0,0,15,144,0,0,0,15,253,0,0,0,0,126,0,0,0,0,63,0,0,0,25,127,0,0,0,47,253,0,0,0,11,228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 6 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,125,0,0,0,0,244,0,0,0,2,224,0,0,0,11,232,0,0,0,31,255,0,0,0,63,95,128,0,0,63,15,192,0,0,47,95,192,0,0,31,255,64,0,0,2,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 7 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,254,0,0,0,63,253,0,0,0,0,180,0,0,0,1,224,0,0,0,2,208,0,0,0,7,128,0,0,0,11,64,0,0,0,15,0,0,0,0,46,0,0,0,0,125,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 8 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,249,0,0,0,47,255,128,0,0,63,15,192,0,0,47,95,128,0,0,11,254,0,0,0,47,255,128,0,0,191,15,224,0,0,255,95,240,0,0,127,255,208,0,0,10,250,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 9 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,249,0,0,0,127,255,192,0,0,189,7,224,0,0,252,3,240,0,0,190,11,240,0,0,127,255,208,0,0,11,255,128,0,0,0,190,0,0,0,2,252,0,0,0,15,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* : */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,46,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ; */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,46,0,0,0,0,0,0,0,0,0,4,0,0,0,0,63,0,0,0,0,30,0,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* < */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,108,0,0,0,7,248,0,0,0,127,64,0,0,0,127,64,0,0,0,11,248,0,0,0,0,108,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* = */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,252,0,0,0,127,252,0,0,0,127,252,0,0,0,127,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* > */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,121,0,0,0,0,111,144,0,0,0,6,252,0,0,0,2,252,0,0,0,111,144,0,0,0,121,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ? */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,244,0,0,0,63,254,0,0,0,109,31,0,0,0,0,31,0,0,0,1,190,0,0,0,3,244,0,0,0,3,240,0,0,0,1,80,0,0,0,2,224,0,0,0,2,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* @ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,191,64,0,0,11,234,224,0,0,47,107,184,0,0,61,255,60,0,0,60,243,60,0,0,61,247,248,0,0,46,191,224,0,0,31,168,0,0,0,7,251,128,0,0,1,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* A */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,0,0,0,0,188,0,0,0,0,253,0,0,0,2,239,0,0,0,3,223,64,0,0,11,203,128,0,0,15,255,208,0,0,31,255,224,0,0,63,67,240,0,0,127,3,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* B */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,248,0,0,0,63,254,0,0,0,63,63,0,0,0,63,126,0,0,0,63,248,0,0,0,63,255,128,0,0,63,63,192,0,0,63,63,192,0,0,63,191,128,0,0,63,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* C */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,111,208,0,0,2,255,208,0,0,15,245,128,0,0,47,208,0,0,0,63,192,0,0,0,63,192,0,0,0,47,208,0,0,0,15,245,128,0,0,2,255,208,0,0,0,111,208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* D */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,228,0,0,0,63,255,64,0,0,63,47,192,0,0,63,31,224,0,0,63,15,240,0,0,63,15,240,0,0,63,31,224,0,0,63,47,192,0,0,63,255,64,0,0,63,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* E */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,253,0,0,0,63,253,0,0,0,63,0,0,0,0,63,0,0,0,0,63,252,0,0,0,63,252,0,0,0,63,0,0,0,0,63,0,0,0,0,63,253,0,0,0,63,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* F */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,253,0,0,0,63,253,0,0,0,63,0,0,0,0,63,0,0,0,0,63,252,0,0,0,63,252,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* G */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,190,64,0,0,7,255,208,0,0,31,225,128,0,0,47,208,0,0,0,63,203,252,0,0,63,203,252,0,0,47,209,252,0,0,15,226,244,0,0,2,255,224,0,0,0,126,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* H */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,255,192,0,0,63,255,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* I */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* J */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,252,0,0,0,0,252,0,0,0,0,252,0,0,0,0,252,0,0,0,0,252,0,0,0,0,252,0,0,0,0,252,0,0,0,21,252,0,0,0,63,248,0,0,0,31,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* K */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,15,208,0,0,63,47,64,0,0,63,125,0,0,0,63,184,0,0,0,63,224,0,0,0,63,244,0,0,0,63,189,0,0,0,63,63,64,0,0,63,47,208,0,0,63,15,244,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* L */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,253,0,0,0,63,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* M */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,130,252,0,0,63,195,252,0,0,63,215,252,0,0,63,215,252,0,0,63,170,252,0,0,63,190,252,0,0,63,125,252,0,0,63,60,252,0,0,63,40,252,0,0,63,40,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* N */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,67,240,0,0,63,131,240,0,0,63,211,240,0,0,63,227,240,0,0,63,179,240,0,0,63,123,240,0,0,63,47,240,0,0,63,31,240,0,0,63,11,240,0,0,63,7,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* O */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,185,0,0,0,7,255,64,0,0,31,223,208,0,0,47,71,224,0,0,63,3,240,0,0,63,3,240,0,0,47,71,224,0,0,31,223,208,0,0,7,255,64,0,0,1,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* P */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,249,0,0,0,63,255,128,0,0,63,63,192,0,0,63,63,192,0,0,63,255,64,0,0,63,249,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* Q */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,185,0,0,0,7,255,64,0,0,31,223,208,0,0,47,71,224,0,0,63,3,240,0,0,63,7,240,0,0,47,111,224,0,0,31,223,192,0,0,7,255,208,0,0,1,187,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* R */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,249,0,0,0,63,255,64,0,0,63,63,192,0,0,63,63,192,0,0,63,191,64,0,0,63,249,0,0,0,63,184,0,0,0,63,126,0,0,0,63,63,64,0,0,63,47,208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* S */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,191,144,0,0,7,255,224,0,0,15,209,128,0,0,15,224,0,0,0,7,254,0,0,0,0,127,208,0,0,0,15,240,0,0,29,31,240,0,0,127,255,208,0,0,27,250,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* T */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,255,128,0,0,191,255,128,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,3,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* U */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,63,15,192,0,0,47,95,128,0,0,31,255,64,0,0,6,249,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* V */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,125,11,192,0,0,62,11,128,0,0,47,15,128,0,0,31,79,64,0,0,15,223,0,0,0,7,239,0,0,0,3,254,0,0,0,2,253,0,0,0,1,253,0,0,0,0,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* W */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,64,112,11,240,63,128,180,15,208,47,192,248,47,192,15,209,248,63,128,11,226,252,127,64,7,243,237,190,0,3,251,222,253,0,1,255,159,252,0,0,255,143,248,0,0,191,79,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* X */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,195,244,0,0,7,219,224,0,0,2,255,128,0,0,0,190,0,0,0,0,60,0,0,0,0,190,0,0,0,2,255,128,0,0,7,235,208,0,0,31,215,244,0,0,127,130,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* Y */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,15,192,0,0,47,31,192,0,0,15,111,128,0,0,7,191,128,0,0,3,255,64,0,0,1,255,0,0,0,0,255,0,0,0,0,255,0,0,0,0,255,0,0,0,0,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* Z */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,255,0,0,0,15,254,0,0,0,0,188,0,0,0,0,248,0,0,0,2,240,0,0,0,7,208,0,0,0,11,192,0,0,0,31,64,0,0,0,47,253,0,0,0,127,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* [ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,224,0,0,0,61,80,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,61,80,0,0,0,63,224,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* \ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0,61,0,0,0,0,46,0,0,0,0,31,0,0,0,0,15,64,0,0,0,11,128,0,0,0,7,192,0,0,0,3,224,0,0,0,2,240,0,0,0,0,244,0,0,0,0,184,0,0,0,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ] */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,192,0,0,0,87,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,3,192,0,0,0,87,192,0,0,0,191,192,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ^ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,64,0,0,0,31,208,0,0,0,40,164,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* _ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ` */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,0,47,128,0,0,0,2,208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* a */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,239,192,0,0,31,255,192,0,0,63,15,192,0,0,63,15,192,0,0,31,255,192,0,0,7,239,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* b */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,189,0,0,0,63,255,64,0,0,63,15,192,0,0,63,15,192,0,0,63,255,64,0,0,63,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* c */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,224,0,0,0,47,244,0,0,0,60,0,0,0,0,60,16,0,0,0,47,244,0,0,0,11,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* d */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,7,239,192,0,0,31,255,192,0,0,63,15,192,0,0,63,15,192,0,0,31,255,192,0,0,7,239,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* e */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,228,0,0,0,47,124,0,0,0,127,253,0,0,0,126,20,0,0,0,47,188,0,0,0,7,228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* f */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,249,0,0,0,11,248,0,0,0,15,192,0,0,0,127,248,0,0,0,127,248,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* g */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,239,192,0,0,31,255,192,0,0,63,15,192,0,0,63,15,192,0,0,31,255,192,0,0,7,239,192,0,0,5,31,192,0,0,47,255,64,0,0,6,249,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* h */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,248,0,0,0,63,254,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* i */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,0,0,0,0,46,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* j */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,128,0,0,0,11,128,0,0,0,0,0,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,31,192,0,0,0,191,64,0,0,0,105,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* k */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,63,64,0,0,63,189,0,0,0,63,224,0,0,0,63,244,0,0,0,63,125,0,0,0,63,47,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* l */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,64,0,0,0,31,224,0,0,0,6,144,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* m */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,246,254,0,0,63,255,255,128,0,63,63,15,192,0,63,63,15,192,0,63,63,15,192,0,63,63,15,192,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* n */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,248,0,0,0,63,254,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* o */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,208,0,0,0,47,248,0,0,0,60,60,0,0,0,60,60,0,0,0,47,248,0,0,0,7,208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* p */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,189,0,0,0,63,255,64,0,0,63,15,192,0,0,63,15,192,0,0,63,255,64,0,0,63,189,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* q */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,239,192,0,0,31,255,192,0,0,63,15,192,0,0,63,15,192,0,0,31,255,192,0,0,7,239,192,0,0,0,15,192,0,0,0,15,192,0,0,0,15,192,0,0,0,0,0,0,0,0,0,0,0,0,
/* r */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,184,0,0,0,63,244,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* s */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,228,0,0,0,47,160,0,0,0,47,144,0,0,0,6,244,0,0,0,62,244,0,0,0,47,224,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* t */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,64,0,0,0,15,192,0,0,0,15,192,0,0,0,127,248,0,0,0,111,232,0,0,0,15,192,0,0,0,15,192,0,0,0,15,248,0,0,0,6,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* u */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,63,63,0,0,0,47,255,0,0,0,11,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* v */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,125,62,0,0,0,46,125,0,0,0,15,188,0,0,0,11,248,0,0,0,3,244,0,0,0,1,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* w */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,127,65,192,190,0,31,195,225,252,0,11,231,247,244,0,2,255,191,208,0,0,255,127,128,0,0,62,62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* x */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,175,128,0,0,7,253,0,0,0,1,244,0,0,0,2,248,0,0,0,11,254,0,0,0,47,95,208,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* y */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,125,62,0,0,0,46,125,0,0,0,15,184,0,0,0,7,244,0,0,0,2,224,0,0,0,2,208,0,0,0,7,192,0,0,0,11,64,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* z */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,248,0,0,0,26,240,0,0,0,3,208,0,0,0,11,128,0,0,0,31,164,0,0,0,127,248,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* { */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,224,0,0,0,15,144,0,0,0,15,0,0,0,0,15,0,0,0,0,15,0,0,0,0,31,0,0,0,0,126,0,0,0,0,31,0,0,0,0,15,0,0,0,0,15,0,0,0,0,15,0,0,0,0,15,144,0,0,0,7,224,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* | */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* } */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,189,0,0,0,0,111,0,0,0,0,15,0,0,0,0,15,0,0,0,0,15,0,0,0,0,15,64,0,0,0,11,208,0,0,0,15,64,0,0,0,15,0,0,0,0,15,0,0,0,0,15,0,0,0,0,111,0,0,0,0,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* ~ */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,40,0,0,190,0,190,0,0,190,0,190,0,0,40,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,0,0,0,60,60,0,0,0,60,63,192,0,3,252,63,252,0,63,252,15,255,255,255,240,0,255,255,255,0,0,15,255,240,0,0,0,0,0,0,0,0,0,0,0,
};

const FT81X_Image_Properties blink_font_l2_raw_image_properties =
{
    .format             = FT81X_BITMAP_FORMAT_L2,
    .width              = 20,
    .height             = 20,
    .linestride         = 5,

    .is_compressed      = 0,
    .data               = blink_font_l2_raw,
    .size               = sizeof(blink_font_l2_raw),

    .is_lut_compressed  = 0,
    .lut_data           = NULL,
    .lut_size           = 0,
};

const FT81X_Font_Properties blink_font_l2_raw_font_properties =
{
    .image_properties       = &blink_font_l2_raw_image_properties,
    .character_widths       = blink_font_l2_raw_character_widths,
    .character_widths_size  = sizeof(blink_font_l2_raw_character_widths),
};

#endif