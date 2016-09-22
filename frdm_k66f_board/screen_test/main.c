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
#include "ft81x_text_manager.h"

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
// FT81X stuff
// ----------------------------------------------------------------------------
#define DL_BUFFER_SIZE 512
uint8_t dl_buffer[DL_BUFFER_SIZE];
const uint32_t clear_dl_snippet[] =
{
    // set the background colour to dark grey and clear the screen
    FT81X_DL_CMD_CLEAR_COLOUR_RGB(32,32,32),
    FT81X_DL_CMD_CLEAR(1,1,1),
};

const uint32_t test_dl_snippet[] =
{
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
};

// ----------------------------------------------------------------------------
// Test display functions
// ----------------------------------------------------------------------------
ft81x_result test_text(FT81X_Handle *handle)
{
    ft81x_result res;

    // set the font to green
    res = ft81x_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0, 255, 0));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    res = ft81x_text_manager_write_fixed_width_font_string(handle,
                                                           24,
                                                           FT81X_TEXT_MANAGER_FONT_24_MAX_WIDTH,
                                                           FT81X_TEXT_MANAGER_FONT_24_HEIGHT,
                                                           "How much wood\nwould a wood chuck\nchuck, if a wood\nchuck could\nchuck wood?",
                                                           10,
                                                           40);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // set the font to cyan
    res = ft81x_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0, 255, 255));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    res = ft81x_text_manager_write_fixed_width_font_string(handle,
                                                           30,
                                                           FT81X_TEXT_MANAGER_FONT_30_MAX_WIDTH,
                                                           FT81X_TEXT_MANAGER_FONT_30_HEIGHT,
                                                           "Peter\nPiper\npicked...",
                                                           10,
                                                           300);


    return res;
}

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
    ft81x_result res = ft81x_initialise(&handle, DL_BUFFER_SIZE, dl_buffer);
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

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // clear the screen
    res = ft81x_write_display_list_snippet(&handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // initialise the L8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(&handle,
                                                 &cat_l8_raw_image_properties,
                                                 &cat_l8_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // initialise the argb1555 image in the DL
    res = ft81x_image_manager_send_image_init_dl(&handle,
                                                 &cat_argb1555_raw_image_properties,
                                                 &cat_argb1555_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(&handle,
                                                 &cat_paletted8_raw_image_properties,
                                                 &cat_paletted8_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // draw L8 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(&handle,
                                                               &cat_l8_raw_image_handle,
                                                               160-(cat_l8_raw_image_properties.width/2),
                                                               240-(cat_l8_raw_image_properties.height/2));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // draw ARGB1555 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(&handle,
                                                               &cat_argb1555_raw_image_handle,
                                                               0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // draw paletted8 image
    // (note saves the context on the stack and restores it)
    res = ft81x_image_manager_send_paletted8_image_draw_dl(&handle,
                                                           &cat_paletted8_raw_image_handle,
                                                           162, 270);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // draw some points, lines and rects
    res = ft81x_write_display_list_snippet(&handle, sizeof(test_dl_snippet), test_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_snippet failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // write some text strings
    res = test_text(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test_text failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // display it
    res = ft81x_write_display_list_cmd(&handle, FT81X_DL_CMD_DISPLAY());
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_write_display_list_cmd failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // write everything to the DL ram and then swap it in
    res = ft81x_end_display_list(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_end_display_list failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // ----------------------------------------------------------
    // enable the backlight
    // ----------------------------------------------------------
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
