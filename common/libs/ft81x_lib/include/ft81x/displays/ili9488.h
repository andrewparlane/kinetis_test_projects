#ifndef __ILI9488_H
#define __ILI9488_H

#include "ft81x_config.h"
#include "ft81x/displays/display_types.h"

// ----------------------------------------------------------------------------
// what comms type this display uses
// ----------------------------------------------------------------------------
#define FT81X_DISPLAY_COMMS_TYPE        FT81X_DISPLAY_COMMS_TYPE_SPI
#define FT81X_DISPLAY_SPI_MODE          FT81X_DISPLAY_SPI_MODE_0
#define FT81X_DISPLAY_SPI_BIT_ORDER     FT81X_DISPLAY_SPI_BIT_ORDER_MSB
#define FT81X_DISPLAY_SPI_CS_POLARITY   FT81X_DISPLAY_SPI_CS_ACTIVE_LOW

// ----------------------------------------------------------------------------
// check that the ILI9488 supports the config.
// ----------------------------------------------------------------------------
#if ((FT81X_SCREEN_SWIZZLE) != (FT81X_COLOUR_FORMAT_RGB)) && \
    ((FT81X_SCREEN_SWIZZLE) != (FT81X_COLOUR_FORMAT_BGR))
#  error Unsuported pinout
#endif

#if ((FT81X_SCREEN_HORIZ_BACK_PORCH) < 3) || \
    ((FT81X_SCREEN_HORIZ_BACK_PORCH) > 256) || \
    ((FT81X_SCREEN_HSYNC) < 3) || \
    ((FT81X_SCREEN_HORIZ_FRONT_PORCH) < 3) || \
    ((FT81X_SCREEN_HORIZ_FRONT_PORCH) > 256)
#error Horizontal blanking settings unsupported
#endif

#if ((FT81X_SCREEN_VERT_BACK_PORCH) < 2) || \
    ((FT81X_SCREEN_VERT_BACK_PORCH) > 31) || \
    ((FT81X_SCREEN_VSYNC) < 1) || \
    ((FT81X_SCREEN_VERT_FRONT_PORCH) < 2) || \
    ((FT81X_SCREEN_VERT_FRONT_PORCH) > 31)
#error Vertical blanking settings unsupported
#endif
#endif
