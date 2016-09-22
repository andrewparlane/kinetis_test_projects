#ifndef __MEMORY_MAP_H
#define __MEMORY_MAP_H

// ----------------------------------------------------------------------------
// RAM addresses
// ----------------------------------------------------------------------------
#define FT81X_G_RAM                 0x000000
#define     FT81X_G_RAM_SIZE                0x100000    // 1MB
#define FT81X_DISPLAY_LIST_RAM      0x300000
#define     FT81X_DISPLAY_LIST_RAM_SIZE     0x002000    // 8KB
#define FT81X_COPROC_CMD_RAM        0x308000
#define     FT81X_COPROC_CMD_RAM_SIZE       0x001000    // 4KB

// ----------------------------------------------------------------------------
// Registers
// ----------------------------------------------------------------------------
#define FT81X_REG_ID_ADDR           0x302000
#define     FT81X_REG_ID_EXPECTED           0x7C
#define FT81X_REG_FRAMES            0x302004
#define FT81X_REG_CLOCK             0x302008
#define FT81X_REG_FREQUENCY         0x30200C
#define FT81X_REG_RENDERMODE        0x302010
#define FT81X_REG_SNAPY             0x302014
#define FT81X_REG_SNAPSHOT          0x302018
#define FT81X_REG_SNAPFORMAT        0x30201C
#define FT81X_REG_CPURESET          0x302020
#define FT81X_REG_TAP_CRC           0x302024
#define FT81X_REG_TAP_MASK          0x302028
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
#define FT81X_REG_ROTATE            0x302058
#define FT81X_REG_OUTBITS           0x30205C
#define FT81X_REG_DITHER            0x302060
#define FT81X_REG_SWIZZLE           0x302064
#define FT81X_REG_CSPREAD           0x302068
#define FT81X_REG_PCLK_POL          0x30206C
#define FT81X_REG_PCLK              0x302070
#define FT81X_REG_GPIO_DIR          0x302090
#define FT81X_REG_GPIO              0x302094
#define     FT81X_REG_GPIO_DISP             0x80
#define FT81X_REG_INT_FLAGS         0x3020A8
#define FT81X_REG_INT_EN            0x3020AC
#define FT81X_REG_INT_MASK          0x3020B0
#define FT81X_REG_PWM_DUTY          0x3020D4
#define     FT81X_REG_PWM_DUTY_OFF          0
#define     FT81X_REG_PWM_DUTY_MAX          128
#define FT81X_REG_CMD_READ          0x3020F8
#define FT81X_REG_CMD_WRITE         0x3020FC
#define FT81X_REG_CMD_DL            0x302100
#define FT81X_REG_TOUCH_RZTHRESH    0x302118

#endif
