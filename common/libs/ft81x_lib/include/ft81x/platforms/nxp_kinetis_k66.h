#ifndef __NXP_KINETIS_K66_H
#define __NXP_KINETIS_K66_H

#include "fsl_dspi_edma.h"

#include <stdint.h>

typedef struct
{
    // init parameters
    uint32_t gpu_rx_dma_channel;
    uint32_t gpu_tx_dma_channel;
    uint32_t gpu_intermediary_dma_channel;

    // GPU spi edmai handles
    edma_handle_t               gpu_spi_tx_data_to_intermediary_edma_handle;
    edma_handle_t               gpu_spi_intermediary_to_tx_reg_edma_handle;
    edma_handle_t               gpu_spi_rx_edma_handle;
    dspi_master_edma_handle_t   gpu_spi_edma_handle;

    // display spi edma handles (pointers so we can use tha same as the gpu if needed)
    dspi_master_edma_handle_t   *display_spi_edma_handle;

    // internal flags
    volatile uint8_t            spi_transfer_complete;
    volatile status_t           spi_transfer_status;
} FT81X_NXP_kinetis_k66_user_data;

#endif
