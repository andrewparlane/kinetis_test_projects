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
// SPI buffers
// ----------------------------------------------------------------------------
#define MAX_SPI_TRANSFER_SIZE   32
uint8_t spi_tx_buffer[MAX_SPI_TRANSFER_SIZE];
uint8_t spi_rx_buffer[MAX_SPI_TRANSFER_SIZE];


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
    printf("screen_spi_transfer_complete: ");

    if (status == kStatus_Success)
    {
        printf("OK");
    }
    else
    {
        printf("Error: %u", (unsigned int)status);
    }
    printf("\ndata:");
    for (int i = 0; i < MAX_SPI_TRANSFER_SIZE; i++)
    {
        printf(" %02X", spi_rx_buffer[i]);
    }
    printf("\n");
    transfer_finished = 1;
}
// ----------------------------------------------------------------------------
// misc functions
// ----------------------------------------------------------------------------

void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}


int main(void)
{
    // Init pins, clocks and default console
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    // disable the MPU ?? why
    MPU_Type *base = MPU;
    base->CESR &= ~MPU_CESR_VLD_MASK;

    // start the task scheduler
    //vTaskStartScheduler();

    printf("Starting screen_test\n");
    delay();
    printf("1\n");
    delay();
    printf("2\n");
    delay();
    printf("3\n");
    delay();

    // ----------------------------------------------------------
    // set up the communication pins
    // ----------------------------------------------------------

    // configure the GPIO pins
    for (int i = 0; i < NUM_GPIO_PINS; i++)
    {
        GPIO_PinInit(pin_config[i]->port, pin_config[i]->pin, &pin_config[i]->config);
    }

    // configure our DMA
    if (NUM_EDMA_CHANNELS > FSL_FEATURE_DMAMUX_MODULE_CHANNEL)
    {
        printf("Trying to use too many DMA channels, aborting\n");
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
    dspi_master_config_t screen_spi_config;
    screen_spi_config.whichCtar = kDSPI_Ctar0;
    screen_spi_config.ctarConfig.baudRate = 500000; // 500KHz - TODO: increase
    screen_spi_config.ctarConfig.bitsPerFrame = 8U;
    screen_spi_config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    screen_spi_config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    screen_spi_config.ctarConfig.direction = kDSPI_MsbFirst;
    screen_spi_config.ctarConfig.pcsToSckDelayInNanoSec = 4;
    screen_spi_config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    screen_spi_config.ctarConfig.betweenTransferDelayInNanoSec = 0;
    // note we use kDSPI_Pcs0 here but in the tfer we use kDSPI_MasterPcs0
    screen_spi_config.whichPcs = kDSPI_Pcs0;
    screen_spi_config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    screen_spi_config.enableContinuousSCK = false;
    screen_spi_config.enableRxFifoOverWrite = false;
    screen_spi_config.enableModifiedTimingFormat = false;
    screen_spi_config.samplePoint = kDSPI_SckToSin0Clock;

    DSPI_MasterInit(SPI0, &screen_spi_config, CLOCK_GetFreq(DSPI0_CLK_SRC));

    // ----------------------------------------------------------
    // set up the ME810A-HV35R eval board
    // ----------------------------------------------------------

    // first take the module out of reset
    delay();
    GPIO_WritePinOutput(screen_not_pd_pin.port, screen_not_pd_pin.pin, 1);
    delay();

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
    screen_spi_transfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0;

    status_t ret = DSPI_MasterTransferEDMA(SPI0, &screen_spi_edma_handle, &screen_spi_transfer);
    if (ret != kStatus_Success)
    {
        printf("DSPI_MasterTransferEDMA() returned %u, trying to send ACTIVE command\n", (unsigned int)ret);
    }

    while (!transfer_finished);
    delay();
    delay();

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
    screen_spi_transfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0;

    ret = DSPI_MasterTransferEDMA(SPI0, &screen_spi_edma_handle, &screen_spi_transfer);
    if (ret != kStatus_Success)
    {
        printf("DSPI_MasterTransferEDMA() returned %u trying to read ID register\n", (unsigned int)ret);
    }

    // don't exit main()
    for (;;);

    return 0;
}
