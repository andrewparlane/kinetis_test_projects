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

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// driver includes
#include "fsl_dmamux.h"
#include "fsl_edma.h"

// ft81x lib includes
#include "ft81x/ft81x.h"
#include "ft81x/platforms/platform.h"

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
// DMA channels
// ----------------------------------------------------------------------------
enum
{
    EDMA_CHANNEL_SCREEN_SPI_RX = 0,
    EDMA_CHANNEL_SCREEN_SPI_TX,
    EDMA_CHANNEL_SCREEN_SPI_INTERMEDIARY,   // note: no idea why we need this

    // count of how many channels we want to use
    // used to check that we aren't trying to use
    // more than are available
    NUM_EDMA_CHANNELS
};

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
    FT81X_NXP_kinetis_k66_user_data ft81x_platform_user_data;
    ft81x_platform_user_data.gpu_intermediary_dma_channel = EDMA_CHANNEL_SCREEN_SPI_INTERMEDIARY;
    ft81x_platform_user_data.gpu_tx_dma_channel = EDMA_CHANNEL_SCREEN_SPI_TX;
    ft81x_platform_user_data.gpu_rx_dma_channel = EDMA_CHANNEL_SCREEN_SPI_RX;
    ft81x_result res = ft81x_initialise((void *)&ft81x_platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_initialise failed with %u\n", res);
    }
    res = ft81x_configure(&ft81x_platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_configure failed with %u\n", res);
    }


    DbgConsole_Printf("sreen_test completed successfully\n");
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
