#ifndef __FT81X_CONFIG_H
#define __FT81X_CONFIG_H

// copy this file to your project directory and rename to ft81x_config.h

#include "ft81x/boards/board_types.h"

// ----------------------------------------------------------------------------
// Board type
// ----------------------------------------------------------------------------
#define FT81X_BOARD                             FT81X_BOARD_FRDM_K66F

// ----------------------------------------------------------------------------
// Platform parameters for K66 platform
// ----------------------------------------------------------------------------
#include "dma_channels.h"
#define FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_RX               EDMA_CHANNEL_GPU_SPI_RX
#define FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_TX               EDMA_CHANNEL_GPU_SPI_TX
#define FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_INTERMEDIARY     EDMA_CHANNEL_GPU_SPI_INTERMEDIARY

// ----------------------------------------------------------------------------
// GPU parameters
// ----------------------------------------------------------------------------
#define FT81X_GPU_TYPE                          (FT81X_GPU_TYPE_FT810)
// for resistive touch displays only, determine by experimentation
#define FT81X_RESISTIVE_TOUCH_THRESHOLD         1800

// ----------------------------------------------------------------------------
// Comms parameters
// ----------------------------------------------------------------------------
#define FT81X_GPU_SPI_BAUD_RATE                 1000000   // 1MHz
#define FT81X_DISPLAY_SPI_BAUD_RATE             1000000   // 1MHz

// ----------------------------------------------------------------------------
// Screen Parameters
// ----------------------------------------------------------------------------
#define FT81X_SCREEN_WIDTH                      320     // in pixels
#define FT81X_SCREEN_HEIGHT                     480     // in pixels
#define FT81X_SCREEN_HORIZ_FRONT_PORCH          3       // REG_HSYNC0 (number of pixel cycles after image data and before hsync pulse)
#define FT81X_SCREEN_HSYNC                      10      // REG_HSYNC1 = this + FT81X_SCREEN_HORIZ_FRONT_PORCH (number of pixel cycles the hsync signal is low)
#define FT81X_SCREEN_HORIZ_BACK_PORCH           30      // REG_HOFFSET = this + FT81X_SCREEN_HSYNC (number of pixel cycles after the hsync pulse and before image data)
#define FT81X_SCREEN_VERT_FRONT_PORCH           2       // REG_VSYNC0 (number of lines after image data and before vsync pulse)
#define FT81X_SCREEN_VSYNC                      5       // REG_VSYNC1 = this + FT81X_SCREEN_VERT_FRONT_PORCH (number of lines the vsync signal is low)
#define FT81X_SCREEN_VERT_BACK_PORCH            5       // REG_VOFFSET = this + FT81X_SCREEN_VSYNC (number of lines after the hsync pulse and before image data)

#define FT81X_SCREEN_PCLK_POL                   1       // 0 = output data on rising edge, 1 = output data on falling edge
#define FT81X_SCREEN_SWIZZLE                    (FT81X_COLOUR_FORMAT_RGB) // change what colour / bit is outputed on each pin.
#define FT81X_SCREEN_PCLK_DIVISOR               5       // frame rate = (internalClk (default 60MHz) / this) / (TOTAL_WIDTH * TOTAL_HEIGHT)

// ----------------------------------------------------------------------------
// Auto calculated parameters, do not edit
// ----------------------------------------------------------------------------
#define FT81X_RESISTIVE_TOUCH                   (((FT81X_GPU_TYPE) == (FT81X_GPU_TYPE_FT810)) || \
                                                 ((FT81X_GPU_TYPE) == (FT81X_GPU_TYPE_FT812)))

#if ((FT81X_GPU_TYPE) == (FT81X_GPU_TYPE_FT810)) || \
    ((FT81X_GPU_TYPE) == (FT81X_GPU_TYPE_FT811))
#define FT81X_BITS_PER_PIXEL                    18
#else
#define FT81X_BITS_PER_PIXEL                    24
#endif

#define FT81X_SCREEN_HORIZ_NON_VISIBLE          ((FT81X_SCREEN_HORIZ_FRONT_PORCH) + \
                                                 (FT81X_SCREEN_HSYNC) + \
                                                 (FT81X_SCREEN_HORIZ_BACK_PORCH))

#define FT81X_SCREEN_TOTAL_WIDTH                ((FT81X_SCREEN_WIDTH) + \
                                                 (FT81X_SCREEN_HORIZ_NON_VISIBLE))

#define FT81X_SCREEN_VERT_NON_VISIBLE           ((FT81X_SCREEN_VERT_FRONT_PORCH) + \
                                                 (FT81X_SCREEN_VSYNC) + \
                                                 (FT81X_SCREEN_VERT_BACK_PORCH))

#define FT81X_SCREEN_TOTAL_HEIGHT               ((FT81X_SCREEN_HEIGHT) + \
                                                 (FT81X_SCREEN_VERT_NON_VISIBLE))

#define FT81X_SCREEN_TOTAL_PIXELS               ((FT81X_SCREEN_TOTAL_WIDTH) * \
                                                 (FT81X_SCREEN_TOTAL_HEIGHT))

#define FT81X_SCREEN_FRAME_RATE                 ((60000000 / (FT81X_SCREEN_PCLK_DIVISOR)) / (FT81X_SCREEN_TOTAL_PIXELS))

#endif
