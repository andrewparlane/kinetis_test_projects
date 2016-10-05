#include "ft81x/boards/board.h" // defines the platform type

#include "ft81x/platforms/platform.h"
#include "ft81x/displays/display.h"

// only compile this file if we have the nxp kinetis k66 platform defined
#if ((FT81X_PLATFORM) == (FT81X_PLATFORM_NXP_KINETIS_K66))

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// driver includes
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_dspi_edma.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// DMA callbacks
// ----------------------------------------------------------------------------
static void spi_transfer_complete(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *user_data)
{
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)user_data;

    if (k66_user_data)
    {
        k66_user_data->spi_transfer_complete = 1;
        k66_user_data->spi_transfer_status = status;
    }
}

// ----------------------------------------------------------------------------
// Initialise functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    if (handle->platform_user_data != NULL)
    {
        return FT81X_RESULT_ALREADY_INITIALISED;
    }

    handle->platform_user_data = (void *)malloc(sizeof(FT81X_NXP_kinetis_k66_user_data));

    if (handle->platform_user_data == NULL)
    {
        return FT81X_RESULT_OUT_OF_MEMORY;
    }

    // initialise the GPIO pins that connect to the GPU
    // power down - active low (defaults to powered down)
    ft81x_platform_initialise_gpio_pin(handle, FT81X_BOARD_GPU_NOT_PD_PIN_PORT, FT81X_BOARD_GPU_NOT_PD_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_spi_comms_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    // set up the SPI Rx EDMA
    memset(&k66_user_data->gpu_spi_rx_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_RX, FT81X_BOARD_GPU_DMAMUX_RX_SRC);
    DMAMUX_EnableChannel(DMAMUX, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_RX);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_rx_edma_handle, DMA0, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_RX);

    // set up the SPI Tx EDMA
    // note: I don't understand the intermediary handle
    // TODO: research more
    memset(&k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle, 0, sizeof(edma_handle_t));
    memset(&k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_TX, FT81X_BOARD_GPU_DMAMUX_TX_SRC);
    DMAMUX_EnableChannel(DMAMUX, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_TX);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle, DMA0, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_INTERMEDIARY);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle, DMA0, FT81X_NXP_K66_EDMA_CHANNEL_GPU_SPI_TX);

    // set up the SPI module
    dspi_master_config_t config;
    config.whichCtar = FT81X_BOARD_GPU_SPI_INIT_CTAR;
    config.ctarConfig.baudRate = FT81X_GPU_SPI_BAUD_RATE;
    config.ctarConfig.bitsPerFrame = 8U;
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    config.ctarConfig.direction = kDSPI_MsbFirst;
    config.ctarConfig.pcsToSckDelayInNanoSec = 500000000U / FT81X_GPU_SPI_BAUD_RATE; // todo this needs to be at least 4nS
    config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    config.ctarConfig.betweenTransferDelayInNanoSec = 0;
    config.whichPcs = FT81X_BOARD_GPU_SPI_INIT_CS;
    config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    config.enableContinuousSCK = false;
    config.enableRxFifoOverWrite = false;
    config.enableModifiedTimingFormat = false;
    config.samplePoint = kDSPI_SckToSin0Clock;

    DSPI_MasterInit(FT81X_BOARD_GPU_SPI_MODULE, &config, CLOCK_GetFreq(FT81X_BOARD_GPU_SPI_CLK_SRC));

    // create the SPI EDMA handle
    DSPI_MasterTransferCreateHandleEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), spi_transfer_complete,
                                        k66_user_data,
                                        &k66_user_data->gpu_spi_rx_edma_handle,
                                        &k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle,
                                        &k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle);

    // initialise flags
    k66_user_data->spi_transfer_complete = 0;

    return FT81X_RESULT_OK;
}

#if ((FT81X_DISPLAY_COMMS_TYPE) == (FT81X_DISPLAY_COMMS_TYPE_SPI))
ft81x_result ft81x_platform_display_spi_comms_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    dspi_master_config_t config;
    config.whichCtar = FT81X_BOARD_DISPLAY_SPI_INIT_CTAR;
    config.ctarConfig.baudRate = FT81X_DISPLAY_SPI_BAUD_RATE;
    config.ctarConfig.bitsPerFrame = 8U;
#if ((FT81X_DISPLAY_SPI_MODE) == (FT81X_DISPLAY_SPI_MODE_0))
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
#elif ((FT81X_DISPLAY_SPI_MODE) == (FT81X_DISPLAY_SPI_MODE_1))
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    config.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
#elif ((FT81X_DISPLAY_SPI_MODE) == (FT81X_DISPLAY_SPI_MODE_2))
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
    config.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
#elif ((FT81X_DISPLAY_SPI_MODE) == (FT81X_DISPLAY_SPI_MODE_3))
    config.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
    config.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
#else
#   error Unsupported display SPI mode
#endif

#if ((FT81X_DISPLAY_SPI_BIT_ORDER) == (FT81X_DISPLAY_SPI_BIT_ORDER_MSB))
    config.ctarConfig.direction = kDSPI_MsbFirst;
#elif ((FT81X_DISPLAY_SPI_BIT_ORDER) == (FT81X_DISPLAY_SPI_BIT_ORDER_LSB))
    config.ctarConfig.direction = kDSPI_LsbFirst;
#else
#   error Unsupported display SPI bit order
#endif

    config.ctarConfig.pcsToSckDelayInNanoSec = 500000000U / FT81X_DISPLAY_SPI_BAUD_RATE; // todo this needs to be at least 4nS
    config.ctarConfig.lastSckToPcsDelayInNanoSec = 0;
    config.ctarConfig.betweenTransferDelayInNanoSec = 0;
    config.whichPcs = FT81X_BOARD_DISPLAY_SPI_INIT_CS;

#if ((FT81X_DISPLAY_SPI_CS_POLARITY) == (FT81X_DISPLAY_SPI_CS_ACTIVE_LOW))
    config.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
#elif ((FT81X_DISPLAY_SPI_CS_POLARITY) == (FT81X_DISPLAY_SPI_CS_ACTIVE_HIGH))
    config.pcsActiveHighOrLow = kDSPI_PcsActiveHigh;
#else
#   error Unsupported Display SPI CS polarity
#endif

    config.enableContinuousSCK = false;
    config.enableRxFifoOverWrite = false;
    config.enableModifiedTimingFormat = false;
    config.samplePoint = kDSPI_SckToSin0Clock;

    DSPI_MasterInit(FT81X_BOARD_DISPLAY_SPI_MODULE, &config, CLOCK_GetFreq(FT81X_BOARD_DISPLAY_SPI_CLK_SRC));

#if ((FT81X_BOARD_GPU_SPI_BASE) != (FT81X_BOARD_DISPLAY_SPI_BASE))
#  error TODO: Set up DMA if we use a different SPI module to the GPU
#else
    k66_user_data->display_spi_edma_handle = &k66_user_data->gpu_spi_edma_handle;
#endif

    return FT81X_RESULT_OK;
}
#endif

void ft81x_platform_cleanup(FT81X_Handle *handle)
{
    if (handle != NULL)
    {
        free(handle->platform_user_data);
        handle->platform_user_data = NULL;
    }
}

// ----------------------------------------------------------------------------
// GPU transfer functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_gpu_send_command(FT81X_Handle *handle, ft81x_command command, uint8_t param)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    // commands consist of 3 bytes
    uint8_t spi_tx_buffer[3] = {(uint8_t)command, param, 0};
    uint8_t spi_rx_buffer[3];

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = spi_rx_buffer;
    tfer.dataSize = 3;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR | FT81X_BOARD_GPU_SPI_TFER_CS | kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to send GPU command\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to send GPU command\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_write_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, const uint8_t *data)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    if (count == 0)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // first send the address
    // the | 0x80 is because we are writing
    uint8_t spi_tx_buffer[3] = { ((address >> 16) & 0x3F) | 0x80,
                                  (address >> 8)  & 0xFF,
                                   address        & 0xFF };

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = NULL;
    tfer.dataSize = 3;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                       FT81X_BOARD_GPU_SPI_TFER_CS |
                       kDSPI_MasterPcsContinuous |
                       kDSPI_MasterActiveAfterTransfer;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to write to GPU memory address\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to write GPU memory address\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // now send the data
    // The EDMA peripheral doesn't allow for more than 511 bytes per transfer
    uint32_t sent = 0;
    while (sent != count)
    {
        uint32_t remaining = (count - sent);
        uint32_t toSend = (remaining > 511) ? 511 : remaining;
        tfer.txData = &data[sent];
        tfer.rxData = NULL;
        tfer.dataSize = toSend;
        tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                           FT81X_BOARD_GPU_SPI_TFER_CS |
                           kDSPI_MasterPcsContinuous |
                           ((toSend == remaining) ? 0 : kDSPI_MasterActiveAfterTransfer);

        ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
        if (ret != kStatus_Success)
        {
            DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to write GPU memory\n", (unsigned int)ret);
            return FT81X_RESULT_GPU_TFER_FAILED;
        }

        // todo add timeout
        while (!k66_user_data->spi_transfer_complete);
        k66_user_data->spi_transfer_complete = 0;

        if (k66_user_data->spi_transfer_status != kStatus_Success)
        {
            DbgConsole_Printf("SPI tfer failed with %u, trying to write GPU memory\n", (unsigned int)k66_user_data->spi_transfer_status);
            return FT81X_RESULT_GPU_TFER_FAILED;
        }

        sent += toSend;
    }

    return FT81X_RESULT_OK;
}

ft81x_result gpu_write_register(FT81X_Handle *handle, uint32_t address, uint8_t count, const uint8_t *data)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    if (count > 4)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // max size is 4 bytes data + 3 bytes address = 7 bytes
    uint8_t spi_tx_buffer[7] = { ((address >> 16) & 0x3F) | 0x80,
                                  (address >> 8)  & 0xFF,
                                   address        & 0xFF };
    memcpy(&spi_tx_buffer[3], data, count);

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = NULL;
    tfer.dataSize = 3 + count;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                       FT81X_BOARD_GPU_SPI_TFER_CS |
                       kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to write a GPU register\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to write a GPU register\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_write_register_8(FT81X_Handle *handle, uint32_t address, uint8_t data)
{
    return gpu_write_register(handle, address, 1, &data);
}

ft81x_result ft81x_platform_gpu_write_register_16(FT81X_Handle *handle, uint32_t address, uint16_t data)
{
    return gpu_write_register(handle, address, 2, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_write_register_32(FT81X_Handle *handle, uint32_t address, uint32_t data)
{
    return gpu_write_register(handle, address, 4, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_read_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, uint8_t *data)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    // first send the address and a dummy byte
    uint8_t spi_tx_buffer[4] = { ((address >> 16) & 0x3F),
                                  (address >> 8)  & 0xFF,
                                   address        & 0xFF,
                                   0 };

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = NULL;
    tfer.dataSize = 4;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                       FT81X_BOARD_GPU_SPI_TFER_CS |
                       kDSPI_MasterPcsContinuous |
                       kDSPI_MasterActiveAfterTransfer;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to read GPU memory (sending addr)\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to read GPU memory (sending addr)\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // now read the data
    tfer.txData = NULL;
    tfer.rxData = data;
    tfer.dataSize = count;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                       FT81X_BOARD_GPU_SPI_TFER_CS |
                       kDSPI_MasterPcsContinuous;

    ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to read GPU memory\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to read GPU memory\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    return FT81X_RESULT_OK;
}

ft81x_result gpu_read_register(FT81X_Handle *handle, uint32_t address, uint8_t count, uint8_t *data)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    // register reads consist of:
    //  3 bytes of address (write)
    //  1 byte of dummy
    //  up to 4 bytes of data (read)
    uint8_t spi_tx_buffer[8] = {(uint8_t)((address >> 16) & 0x3F),
                                (uint8_t)(address >> 8),
                                (uint8_t)(address),
                                0,
                                0, 0, 0, 0};
    uint8_t spi_rx_buffer[8];

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = spi_rx_buffer;
    tfer.dataSize = 4 + count;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR |
                       FT81X_BOARD_GPU_SPI_TFER_CS |
                       kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to read a GPU register\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to read a GPU register\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    memcpy(data, &spi_rx_buffer[4], count);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_read_register_8(FT81X_Handle *handle, uint32_t address, uint8_t *value)
{
    return gpu_read_register(handle, address, 1, (uint8_t *)value);
}

ft81x_result ft81x_platform_gpu_read_register_16(FT81X_Handle *handle, uint32_t address, uint16_t *value)
{
    return gpu_read_register(handle, address, 2, (uint8_t *)value);
}

ft81x_result ft81x_platform_gpu_read_register_32(FT81X_Handle *handle, uint32_t address, uint32_t *value)
{
    return gpu_read_register(handle, address, 4, (uint8_t *)value);
}

// ----------------------------------------------------------------------------
// Display transfer functions
// ----------------------------------------------------------------------------
#if ((FT81X_DISPLAY_COMMS_TYPE) == (FT81X_DISPLAY_COMMS_TYPE_SPI))
ft81x_result ft81x_platform_display_spi_transfer(FT81X_Handle *handle, uint32_t count, const uint8_t *tx_data, uint8_t *rx_data)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)handle->platform_user_data;

    dspi_transfer_t tfer;
    tfer.txData = tx_data;
    tfer.rxData = rx_data;
    tfer.dataSize = count;
    tfer.configFlags = FT81X_BOARD_DISPLAY_SPI_TFER_CTAR | FT81X_BOARD_DISPLAY_SPI_TFER_CS | kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, k66_user_data->display_spi_edma_handle, &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to transfer to the display\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!k66_user_data->spi_transfer_complete);
    k66_user_data->spi_transfer_complete = 0;

    if (k66_user_data->spi_transfer_status != kStatus_Success)
    {
        DbgConsole_Printf("SPI tfer failed with %u, trying to transfer to the display\n", (unsigned int)k66_user_data->spi_transfer_status);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    return FT81X_RESULT_OK;
}
#endif

// ----------------------------------------------------------------------------
// Power functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_set_power_down_pin(FT81X_Handle *handle, uint8_t power_down)
{
    return ft81x_platform_set_gpio_pin(handle, FT81X_BOARD_GPU_NOT_PD_PIN_PORT, FT81X_BOARD_GPU_NOT_PD_PIN_NUM, !power_down);
}

// ----------------------------------------------------------------------------
// GPIO functions
// note: These shouldn't be used externally for the PD pin
//       as that is managed internally
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, ft81x_gpio_direction direction, uint8_t value)
{
    gpio_pin_config_t config = { (direction == FT81X_PLATFORM_GPIO_DIRECTION_INPUT) ? kGPIO_DigitalInput : kGPIO_DigitalOutput, value };
    GPIO_PinInit((GPIO_Type *)port, pin, &config);
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_set_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, uint8_t value)
{
    GPIO_WritePinOutput((GPIO_Type *)port, pin, value);
    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// Misc platform functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_delay(FT81X_Handle *handle, uint32_t milliseconds)
{
    // todo use a better delay function
    uint8_t plusOne = 0;
    if (milliseconds % portTICK_PERIOD_MS)
    {
        plusOne = 1;
    }

    // delays for X ticks. Turn milliseconds into ticks, rounding up
    vTaskDelay((milliseconds / portTICK_PERIOD_MS) + plusOne);

    return FT81X_RESULT_OK;
}

#endif
