/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "dma_channels.h"

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// driver includes
#include "fsl_dmamux.h"
#include "fsl_edma.h"

// ft81x lib includes
#include "ft81x.h"
#include "ft81x_display_list.h"
#include "ft81x_image_manager.h"

// resources
#include "resources/cat_l8_raw.h"
#include "resources/cat_argb1555_raw.h"
#include "resources/cat_paletted8_raw.h"

// ----------------------------------------------------------------------------
// GPIO pins
// ----------------------------------------------------------------------------

typedef struct GPIO_Info
{
    GPIO_Type *port;
    uint32_t pin;
    gpio_pin_config_t config;
} GPIO_Info;

const GPIO_Info screen_int_pin      = { PTC,  4, { kGPIO_DigitalInput,  0 } };

const GPIO_Info *pin_config[] = { &screen_int_pin };

#define NUM_GPIO_PINS (sizeof(pin_config) / sizeof(pin_config[0]))

// ----------------------------------------------------------------------------
// Main thread
// ----------------------------------------------------------------------------

static void main_thread(void *arg)
{
    DbgConsole_Printf("Starting screen_test\n");

    // ----------------------------------------------------------
    // set up the communication pins
    // ----------------------------------------------------------

    // configure the GPIO pins
    // this puts the screen in reset
    for (int i = 0; i < NUM_GPIO_PINS; i++)
    {
        GPIO_PinInit(pin_config[i]->port, pin_config[i]->pin, &pin_config[i]->config);
    }

    // make sure the screen is properly in reset
    vTaskDelay(1);

    // configure our DMA
    if (NUM_EDMA_CHANNELS > FSL_FEATURE_DMAMUX_MODULE_CHANNEL)
    {
        DbgConsole_Printf("Trying to use too many DMA channels, aborting\n");
        while(1);
    }
    DMAMUX_Init(DMAMUX);
    edma_config_t edma_config;
    EDMA_GetDefaultConfig(&edma_config);
    EDMA_Init(DMA0, &edma_config);

    // set up the FT81X lib
    // this initialisers the spi module and the dma channels
    FT81X_Handle handle;
    memset(&handle, 0, sizeof(FT81X_Handle));
    ft81x_result res = ft81x_initialise(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_initialise failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }
    res = ft81x_configure(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_configure failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // ----------------------------------------------------------
    // load our resources into g RAM
    // ----------------------------------------------------------
    // first is the L8 image
    FT81X_Image_Handle cat_l8_raw_image_handle;
    res = ft81x_image_manager_load_image(&handle, &cat_l8_raw_image_properties, &cat_l8_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // then the argb1555 image
    FT81X_Image_Handle cat_argb1555_raw_image_handle;
    res = ft81x_image_manager_load_image(&handle, &cat_argb1555_raw_image_properties, &cat_argb1555_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // then the paletted8 image
    FT81X_Image_Handle cat_paletted8_raw_image_handle;
    res = ft81x_image_manager_load_image(&handle, &cat_paletted8_raw_image_properties, &cat_paletted8_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    const uint32_t test_dl[] =
    {
        // set the background colour to dark grey and clear the screen
        FT81X_DL_CMD_CLEAR_COLOUR_RGB(32,32,32),
        FT81X_DL_CMD_CLEAR(1,1,1),

        // set up the L8 image
        FT81X_IMAGE_MANAGER_DL_INIT_IMAGE(cat_l8_raw_image_properties,
                                          cat_l8_raw_image_handle,
                                          FT81X_BITMAP_FILTER_NEAREST,
                                          FT81X_BITMAP_WRAP_BORDER,
                                          FT81X_BITMAP_WRAP_BORDER),

        // set up the ARGB1555 image
        FT81X_IMAGE_MANAGER_DL_INIT_IMAGE(cat_argb1555_raw_image_properties,
                                          cat_argb1555_raw_image_handle,
                                          FT81X_BITMAP_FILTER_NEAREST,
                                          FT81X_BITMAP_WRAP_BORDER,
                                          FT81X_BITMAP_WRAP_BORDER),

        // set up the PALETTED8 image
        FT81X_IMAGE_MANAGER_DL_INIT_IMAGE(cat_paletted8_raw_image_properties,
                                          cat_paletted8_raw_image_handle,
                                          FT81X_BITMAP_FILTER_NEAREST,
                                          FT81X_BITMAP_WRAP_BORDER,
                                          FT81X_BITMAP_WRAP_BORDER),

        // draw L8 image
        FT81X_IMAGE_MANAGER_DL_DRAW_NON_PALETTED_IMAGE(cat_l8_raw_image_handle,
                                                       160-(cat_l8_raw_image_properties.width/2),
                                                       240-(cat_l8_raw_image_properties.height/2)),

        // draw ARGB1555 image
        FT81X_IMAGE_MANAGER_DL_DRAW_NON_PALETTED_IMAGE(cat_argb1555_raw_image_handle,
                                                       0, 0),

        // draw paletted8 image
        // (note saves the context on the stack and restores it)
        FT81X_IMAGE_MANAGER_DL_DRAW_PALETTED_IMAGE(cat_paletted8_raw_image_handle,
                                                   162, 270),

        // draw some points
        FT81X_DL_CMD_POINT_SIZE(20 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 0, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_POINTS),
            FT81X_DL_CMD_VERTEX2II(5, 5, 0, 0),
        FT81X_DL_CMD_END(),

        FT81X_DL_CMD_POINT_SIZE(40 * 16),
        FT81X_DL_CMD_COLOUR_RGB(0, 255, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_POINTS),
            FT81X_DL_CMD_VERTEX2II(315, 5, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a line
        FT81X_DL_CMD_LINE_WIDTH(5 * 16),
        FT81X_DL_CMD_COLOUR_RGB(0, 0, 255),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_LINES),
            FT81X_DL_CMD_VERTEX2II(10, 10, 0, 0),
            FT81X_DL_CMD_VERTEX2II(310, 470, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a rect
        FT81X_DL_CMD_LINE_WIDTH(1 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 255, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
            FT81X_DL_CMD_VERTEX2II(300, 199, 0, 0),
            FT81X_DL_CMD_VERTEX2II(200, 280, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a rect with rounded corners
        FT81X_DL_CMD_LINE_WIDTH(10 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 0, 255),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
            FT81X_DL_CMD_VERTEX2II(50, 300, 0, 0),
            FT81X_DL_CMD_VERTEX2II(100, 450, 0, 0),
        FT81X_DL_CMD_END(),

        // done, show it
        FT81X_DL_CMD_DISPLAY()
    };
    res = ft81x_send_display_list(&handle, sizeof(test_dl), test_dl);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_send_display_list failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // enable the backlight
    res = ft81x_backlight(&handle, FT81X_BACKLIGHT_LEVEL_MAX);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_backlight failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    DbgConsole_Printf("sreen_test completed successfully\n");

    ft81x_cleanup(&handle);
}

// ----------------------------------------------------------------------------
// Entry point
// ----------------------------------------------------------------------------

int main(void)
{
    // Init pins, clocks and default console
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    // disable the MPU ?? why
    MPU_Type *base = MPU;
    base->CESR &= ~MPU_CESR_VLD_MASK;

    // create the main thread ati start the task scheduler
    if (xTaskCreate(main_thread, "main_thread", 4096L / sizeof(portSTACK_TYPE), NULL, 4, NULL) != pdPASS)
    {
        DbgConsole_Printf("failed to start main_thread\n");
        while(1);
    }
    vTaskStartScheduler();

    // don't exit main()
    for (;;);

    return 0;
}
