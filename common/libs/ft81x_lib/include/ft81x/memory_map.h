#ifndef __MEMORY_MAP_H
#define __MEMORY_MAP_H

#define FT81X_DISPLAY_LIST_RAM      0x300000

#define FT81X_REG_ID_ADDR           0x302000
#define     FT81X_REG_ID_EXPECTED           0x7C
#define FT81X_REG_HCYCLE            0x30202C
#define FT81X_REG_HOFFSET           0x302030
#define FT81X_REG_HSIZE             0x302034
#define FT81X_REG_HSYNC0            0x302038
#define FT81X_REG_HSYNC1            0x30203C
#define FT81X_REG_VCYCLE            0x302040
#define FT81X_REG_VOFFSET           0x302044
#define FT81X_REG_VSIZE             0x302048
#define FT81X_REG_VSYNC0            0x30204C
#define FT81X_REG_VSYNC1            0x302050
#define FT81X_REG_DLSWAP            0x302054
#define     FT81X_REG_DLSWAP_SWAP_FRAME     0x02
#define FT81X_REG_DITHER            0x302060
#define FT81X_REG_SWIZZLE           0x302064
#define FT81X_REG_CSPREAD           0x302068
#define FT81X_REG_PCLK_POL          0x30206C
#define FT81X_REG_PCLK              0x302070
#define FT81X_REG_GPIO_DIR          0x302090
#define FT81X_REG_GPIO              0x302094
#define     FT81X_REG_GPIO_DISP             0x80
#define FT81X_REG_TOUCH_RZTHRESH    0x302118

#endif
