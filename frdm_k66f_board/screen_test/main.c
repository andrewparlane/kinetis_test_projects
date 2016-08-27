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
#include "fsl_dspi_edma.h"

// ----------------------------------------------------------------------------
// GPIO pins
// ----------------------------------------------------------------------------

typedef struct GPIO_Info
{
    GPIO_Type *port;
    uint32_t pin;
    gpio_pin_config_t config;
} GPIO_Info;

const GPIO_Info screen_dcx_pin      = { PTB, 19, { kGPIO_DigitalOutput, 0 } };
const GPIO_Info screen_not_pd_pin   = { PTB, 18, { kGPIO_DigitalOutput, 0 } }; // default to powered down
const GPIO_Info screen_disp_pin     = { PTC, 16, { kGPIO_DigitalOutput, 0 } };
const GPIO_Info screen_int_pin      = { PTC,  4, { kGPIO_DigitalInput,  0 } };

const GPIO_Info *pin_config[] = { &screen_dcx_pin,
                                  &screen_not_pd_pin,
                                  &screen_disp_pin,
                                  &screen_int_pin };

#define NUM_GPIO_PINS (sizeof(pin_config) / sizeof(pin_config[0]))

// ----------------------------------------------------------------------------
// SPI
// ----------------------------------------------------------------------------
#define TRANSFER_BAUDRATE 10000; // 10KHz - TODO: increase
#define MAX_SPI_TRANSFER_SIZE   32
uint8_t spi_tx_buffer[MAX_SPI_TRANSFER_SIZE];
uint8_t spi_rx_buffer[MAX_SPI_TRANSFER_SIZE];

void configure_spi_ft81x(void)
{
    dspi_master_config_t config;
    config.whichCtar = kDSPI_Ctar0;
    config.ctarConfig.baudRate = TRANSFER_BAUDRATE; // 500KHz - TODO: increase
    config.ctarConfig.bitsPerFrame = 8U;
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    config.ctarConfig.direction = kDSPI_MsbFirst;
    config.ctarConfig.pcsToSckDelayInNanoSec = 500000000U / TRANSFER_BAUDRATE; // todo this needs to be at least 4nS
    config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    config.ctarConfig.betweenTransferDelayInNanoSec = 0;
    // note we use kDSPI_Pcs0 here but in the tfer we use kDSPI_MasterPcs0
    config.whichPcs = kDSPI_Pcs0;
    config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    config.enableContinuousSCK = false;
    config.enableRxFifoOverWrite = false;
    config.enableModifiedTimingFormat = false;
    config.samplePoint = kDSPI_SckToSin0Clock;

    DSPI_MasterInit(SPI0, &config, CLOCK_GetFreq(DSPI0_CLK_SRC));
}

void configure_spi_display(void)
{
    // TODO: use correct values for the display
    dspi_master_config_t config;
    config.whichCtar = kDSPI_Ctar1;
    config.ctarConfig.baudRate = TRANSFER_BAUDRATE; // 500KHz - TODO: increase
    config.ctarConfig.bitsPerFrame = 8U;
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    config.ctarConfig.direction = kDSPI_MsbFirst;
    config.ctarConfig.pcsToSckDelayInNanoSec = 500000000U / TRANSFER_BAUDRATE; // todo this needs to be at least 4nS
    config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    config.ctarConfig.betweenTransferDelayInNanoSec = 0;
    // note we use kDSPI_Pcs0 here but in the tfer we use kDSPI_MasterPcs0
    config.whichPcs = kDSPI_Pcs1;
    config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    config.enableContinuousSCK = false;
    config.enableRxFifoOverWrite = false;
    config.enableModifiedTimingFormat = false;
    config.samplePoint = kDSPI_SckToSin0Clock;

    DSPI_MasterInit(SPI0, &config, CLOCK_GetFreq(DSPI0_CLK_SRC));
}

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
// DMA callbacks
// ----------------------------------------------------------------------------
volatile uint8_t transfer_finished = 0;
void screen_spi_transfer_complete(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success)
    {
        /*DbgConsole_Printf("Screen SPI tfer complete, data:");
        for (int i = 0; i < MAX_SPI_TRANSFER_SIZE; i++)
        {
            DbgConsole_Printf(" %02X", spi_rx_buffer[i]);
        }
        DbgConsole_Printf("\n");*/
    }
    else
    {
        DbgConsole_Printf("Screen SPI error: %u\n", (unsigned int)status);
    }

    transfer_finished = 1;
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

    // set up the SPI Rx EDMA
    edma_handle_t screen_spi_rx_edma_handle;
    memset(&screen_spi_rx_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_RX, (uint8_t)kDmaRequestMux0SPI0Rx);
    DMAMUX_EnableChannel(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_RX);
    EDMA_CreateHandle(&screen_spi_rx_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_RX);

    // set up the SPI Tx EDMA
    // note: I don't understand the intermediary handle
    // TODO: research more
    edma_handle_t screen_spi_tx_data_to_intermediary_edma_handle;
    edma_handle_t screen_spi_intermediary_to_tx_reg_edma_handle;
    memset(&screen_spi_tx_data_to_intermediary_edma_handle, 0, sizeof(edma_handle_t));
    memset(&screen_spi_intermediary_to_tx_reg_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_TX, (uint8_t)kDmaRequestMux0SPI0Tx);
    DMAMUX_EnableChannel(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_TX);
    EDMA_CreateHandle(&screen_spi_tx_data_to_intermediary_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_INTERMEDIARY);
    EDMA_CreateHandle(&screen_spi_intermediary_to_tx_reg_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_TX);

    // set up the SPI module
    configure_spi_ft81x();      // FT81x module (CS0)
    configure_spi_display();    // display (CS1)

    // ----------------------------------------------------------
    // set up the ME810A-HV35R eval board
    // ----------------------------------------------------------

    // first take the module out of reset
    GPIO_WritePinOutput(screen_not_pd_pin.port, screen_not_pd_pin.pin, 1);
    vTaskDelay(1);

    // start an EDMA transfer to get the ID out of the FT810
    dspi_master_edma_handle_t screen_spi_edma_handle;
    DSPI_MasterTransferCreateHandleEDMA(SPI0, &screen_spi_edma_handle, screen_spi_transfer_complete,
                                        NULL, &screen_spi_rx_edma_handle,
                                        &screen_spi_tx_data_to_intermediary_edma_handle,
                                        &screen_spi_intermediary_to_tx_reg_edma_handle);

    // send active command (read memory address 0)
    memset(spi_tx_buffer, 0, sizeof(spi_tx_buffer));
    memset(spi_rx_buffer, 0, sizeof(spi_rx_buffer));
    spi_tx_buffer[0] = 0x00;
    spi_tx_buffer[1] = 0x00;
    spi_tx_buffer[2] = 0x00;

    dspi_transfer_t screen_spi_transfer;
    screen_spi_transfer.txData = spi_tx_buffer;
    screen_spi_transfer.rxData = spi_rx_buffer;
    screen_spi_transfer.dataSize = 3;
    // note we use kDSPI_MasterPcs0 here but in init we use kDSPI_Pcs0
    screen_spi_transfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(SPI0, &screen_spi_edma_handle, &screen_spi_transfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to send ACTIVE command\n", (unsigned int)ret);
    }

    while (!transfer_finished);
    transfer_finished = 0;
    // wait a bit to give the screen chance to wake up
    vTaskDelay(1);

    // Send read memory address 0x0030_2000 (ID reg)
    memset(spi_tx_buffer, 0, sizeof(spi_tx_buffer));
    memset(spi_rx_buffer, 0, sizeof(spi_rx_buffer));
    spi_tx_buffer[0] = 0x30;
    spi_tx_buffer[1] = 0x20;
    spi_tx_buffer[2] = 0x00;

    screen_spi_transfer.txData = spi_tx_buffer;
    screen_spi_transfer.rxData = spi_rx_buffer;
    screen_spi_transfer.dataSize = 8;
    // note we use kDSPI_MasterPcs0 here but in init we use kDSPI_Pcs0
    screen_spi_transfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    ret = DSPI_MasterTransferEDMA(SPI0, &screen_spi_edma_handle, &screen_spi_transfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u trying to read ID register\n", (unsigned int)ret);
    }

    while (!transfer_finished);
    transfer_finished = 0;

    printf("ID: %02X\n", spi_rx_buffer[4]);

    if (spi_rx_buffer[4] != 0x7C)
    {
        printf("Invalid response for ID register, expecting 7C\n");
    }
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
