#ifndef __DMA_CHANNELS_H
#define __DMA_CHANNELS_H

// ----------------------------------------------------------------------------
// DMA channels
// ----------------------------------------------------------------------------

enum
{
    EDMA_CHANNEL_GPU_SPI_RX = 0,
    EDMA_CHANNEL_GPU_SPI_TX,
    EDMA_CHANNEL_GPU_SPI_INTERMEDIARY,   // note: no idea why we need this

    // count of how many channels we want to use
    // used to check that we aren't trying to use
    // more than are available
    NUM_EDMA_CHANNELS
};


#endif
