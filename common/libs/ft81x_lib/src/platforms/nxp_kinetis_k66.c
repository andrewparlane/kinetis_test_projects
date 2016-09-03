#include "ft81x/boards/board.h" // defines the platform type
#include "ft81x/platforms/platform.h"
#include "ft81x/displays/display.h"

#include "ft81x/ft81x_types.h"

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// driver includes
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_dspi_edma.h"

// only compile this file if we have the nxp kinetis k66 platform defined
#if ((FT81X_PLATFORM) == (FT81X_PLATFORM_NXP_KINETIS_K66))

// ----------------------------------------------------------------------------
// DMA callbacks
// ----------------------------------------------------------------------------
static volatile uint8_t transfer_finished = 0;
static void spi_transfer_complete(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *user_data)
{
    if (status != kStatus_Success)
    {
        DbgConsole_Printf("SPI error: %u\n", (unsigned int)status);
    }

    transfer_finished = 1;
}

// ----------------------------------------------------------------------------
// Initialise functions
// ----------------------------------------------------------------------------

ft81x_result ft81x_platform_initialise(void *platform_user_data)
{
    // initialise the GPIO pins that connect to the GPU
    // power down - active low (defaults to powered down)
    ft81x_platform_initialise_gpio_pin(platform_user_data, FT81X_BOARD_GPU_NOT_PD_PIN_PORT, FT81X_BOARD_GPU_NOT_PD_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_spi_comms_initialise(void *platform_user_data)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;
    const uint32_t EDMA_CHANNEL_SCREEN_SPI_RX = k66_user_data->gpu_rx_dma_channel;
    const uint32_t EDMA_CHANNEL_SCREEN_SPI_TX = k66_user_data->gpu_tx_dma_channel;
    const uint32_t EDMA_CHANNEL_SCREEN_SPI_INTERMEDIARY = k66_user_data->gpu_intermediary_dma_channel;

    // set up the SPI Rx EDMA
    memset(&k66_user_data->gpu_spi_rx_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_RX, FT81X_BOARD_GPU_DMAMUX_RX_SRC);
    DMAMUX_EnableChannel(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_RX);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_rx_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_RX);

    // set up the SPI Tx EDMA
    // note: I don't understand the intermediary handle
    // TODO: research more
    memset(&k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle, 0, sizeof(edma_handle_t));
    memset(&k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle, 0, sizeof(edma_handle_t));
    DMAMUX_SetSource(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_TX, FT81X_BOARD_GPU_DMAMUX_TX_SRC);
    DMAMUX_EnableChannel(DMAMUX, EDMA_CHANNEL_SCREEN_SPI_TX);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_INTERMEDIARY);
    EDMA_CreateHandle(&k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle, DMA0, EDMA_CHANNEL_SCREEN_SPI_TX);

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
                                        NULL,
                                        &k66_user_data->gpu_spi_rx_edma_handle,
                                        &k66_user_data->gpu_spi_tx_data_to_intermediary_edma_handle,
                                        &k66_user_data->gpu_spi_intermediary_to_tx_reg_edma_handle);

    return FT81X_RESULT_OK;
}

#if ((FT81X_DISPLAY_COMMS_TYPE) == (FT81X_DISPLAY_COMMS_TYPE_SPI))
ft81x_result ft81x_platform_display_spi_comms_initialise(void *platform_user_data)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;

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

// ----------------------------------------------------------------------------
// GPU transfer functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_gpu_send_command(void *platform_user_data, ft81x_command command, uint8_t param)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;

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
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to send ACTIVE command\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!transfer_finished);
    transfer_finished = 0;

    return FT81X_RESULT_OK;
}

ft81x_result gpu_write_register(void *platform_user_data, uint32_t address, uint8_t count, uint8_t *data)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;

    if (count > 4)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // max size is 4 bytes data + 3 bytes address = 7 bytes
    uint8_t spi_tx_buffer[7] = { ((address >> 16) & 0x3F) | 0x80,
                                  (address >> 8)  & 0xFF,
                                   address        & 0xFF };
    memcpy(&spi_tx_buffer[4], data, count);

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
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to send ACTIVE command\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!transfer_finished);
    transfer_finished = 0;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_write_register_8(void *platform_user_data, uint32_t address, uint8_t data)
{
    return gpu_write_register(platform_user_data, address, 1, &data);
}

ft81x_result ft81x_platform_gpu_write_register_16(void *platform_user_data, uint32_t address, uint16_t data)
{
    return gpu_write_register(platform_user_data, address, 2, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_write_register_32(void *platform_user_data, uint32_t address, uint32_t data)
{
    return gpu_write_register(platform_user_data, address, 4, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_read_register(void *platform_user_data, uint32_t address, uint32_t *value)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;

    // register reads consist of:
    //  3 bytes of address (write)
    //  1 byte of dummy
    //  4 bytes of data (read)

    uint8_t spi_tx_buffer[8] = {(uint8_t)((address >> 16) & 0x3F),
                                (uint8_t)(address >> 8),
                                (uint8_t)(address),
                                0,
                                0, 0, 0, 0};
    uint8_t spi_rx_buffer[8];

    dspi_transfer_t tfer;
    tfer.txData = spi_tx_buffer;
    tfer.rxData = spi_rx_buffer;
    tfer.dataSize = 8;
    tfer.configFlags = FT81X_BOARD_GPU_SPI_TFER_CTAR | FT81X_BOARD_GPU_SPI_TFER_CS | kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, &(k66_user_data->gpu_spi_edma_handle), &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to read a GPU register\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!transfer_finished);
    transfer_finished = 0;

    if (value != NULL)
    {
        memcpy(value, &spi_rx_buffer[4], 4);
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// Display transfer functions
// ----------------------------------------------------------------------------
#if ((FT81X_DISPLAY_COMMS_TYPE) == (FT81X_DISPLAY_COMMS_TYPE_SPI))
ft81x_result ft81x_platform_display_spi_transfer(void *platform_user_data, uint32_t count, uint8_t *tx_data, uint8_t *rx_data)
{
    if (platform_user_data == NULL)
    {
        return FT81X_RESULT_NO_USER_DATA;
    }
    FT81X_NXP_kinetis_k66_user_data *k66_user_data = (FT81X_NXP_kinetis_k66_user_data *)platform_user_data;

    dspi_transfer_t tfer;
    tfer.txData = tx_data;
    tfer.rxData = rx_data;
    tfer.dataSize = count;
    tfer.configFlags = FT81X_BOARD_DISPLAY_SPI_TFER_CTAR | FT81X_BOARD_DISPLAY_SPI_TFER_CS | kDSPI_MasterPcsContinuous;

    status_t ret = DSPI_MasterTransferEDMA(FT81X_BOARD_GPU_SPI_MODULE, k66_user_data->display_spi_edma_handle, &tfer);
    if (ret != kStatus_Success)
    {
        DbgConsole_Printf("DSPI_MasterTransferEDMA() returned %u, trying to write to the display\n", (unsigned int)ret);
        return FT81X_RESULT_GPU_TFER_FAILED;
    }

    // todo add timeout
    while (!transfer_finished);
    transfer_finished = 0;

    return FT81X_RESULT_OK;
}
#endif

// ----------------------------------------------------------------------------
// Power functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_set_power_down_pin(void *platform_user_data, uint8_t power_down)
{
    return ft81x_platform_set_gpio_pin(platform_user_data, FT81X_BOARD_GPU_NOT_PD_PIN_PORT, FT81X_BOARD_GPU_NOT_PD_PIN_NUM, !power_down);
}

// ----------------------------------------------------------------------------
// GPIO functions
// note: These shouldn't be used externally for the PD pin
//       as that is managed internally
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise_gpio_pin(void *platform_user_data, void *port, uint32_t pin, ft81x_gpio_direction direction, uint8_t value)
{
    gpio_pin_config_t config = { (direction == FT81X_PLATFORM_GPIO_DIRECTION_INPUT) ? kGPIO_DigitalInput : kGPIO_DigitalOutput, value };
    GPIO_PinInit((GPIO_Type *)port, pin, &config);
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_set_gpio_pin(void *platform_user_data, void *port, uint32_t pin, uint8_t value)
{
    GPIO_WritePinOutput((GPIO_Type *)port, pin, value);
    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// Misc platform functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_delay(void *platform_user_data, uint32_t milliseconds)
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
