#include "ft81x/boards/board.h" // defines the board type
#include "ft81x/platforms/platform.h"

#include "ft81x/ft81x_types.h"

// only compile this file if we have the nxp kinetis k66 platform defined
#if ((FT81X_BOARD) == (FT81X_BOARD_FRDM_K66F))

ft81x_result ft81x_board_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_board_gpu_comms_initialise(FT81X_Handle *handle)
{
    // nothing special board related to do here,
    // so just forward this request to the platform
    return ft81x_platform_gpu_spi_comms_initialise(handle);
}

ft81x_result ft81x_board_display_comms_initialise(FT81X_Handle *handle)
{
    // nothing special board related to do here
    // so just forward this request to the platform
    return ft81x_platform_display_spi_comms_initialise(handle);
}

void ft81x_board_cleanup(FT81X_Handle *handle)
{
}

#endif
