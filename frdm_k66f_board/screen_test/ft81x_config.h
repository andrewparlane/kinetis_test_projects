#ifndef __FT81X_CONFIG_H
#define __FT81X_CONFIG_H

// copy this file to your project directory and rename to ft81x_config.h

#include "ft81x/ft81x_types.h"
#include "ft81x/boards/board_types.h"

// ----------------------------------------------------------------------------
// Board type
// ----------------------------------------------------------------------------
#define FT81X_BOARD                             FT81X_BOARD_FRDM_K66F

// ----------------------------------------------------------------------------
// GPU parameters
// ----------------------------------------------------------------------------
#define FT81X_GPU_TYPE                          FT81X_GPU_TYPE_FT810
// for resistive touch displays only, determine by experimentation
#define FT81X_RESISTIVE_TOUCH_THRESHOLD         1800

// ----------------------------------------------------------------------------
// Comms parameters
// ----------------------------------------------------------------------------
#define FT81X_GPU_SPI_BAUD_RATE                 10000   // 10KHz
#define FT81X_DISPLAY_SPI_BAUD_RATE             10000   // 10KHz

// ----------------------------------------------------------------------------
// Screen Parameters
// ----------------------------------------------------------------------------
#define FT81X_SCREEN_WIDTH                      320     // in pixels
#define FT81X_SCREEN_HEIGHT                     480     // in pixels


// ----------------------------------------------------------------------------
// Auto calculated parameters, do not edit
// ----------------------------------------------------------------------------
#define FT81X_RESISTIVE_TOUCH                   (((FT81X_GPU_TYPE) == FT81X_GPU_TYPE_FT810) || \
                                                 ((FT81X_GPU_TYPE) == FT81X_GPU_TYPE_FT812))

#endif
