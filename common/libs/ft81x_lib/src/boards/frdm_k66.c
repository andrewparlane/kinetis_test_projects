#include "ft81x/boards/board.h" // defines the board type
#include "ft81x/platforms/platform.h"

// only compile this file if we have the nxp kinetis k66 platform defined
#if ((FT81X_BOARD) == (FT81X_BOARD_FRDM_K66F))

ft81x_result ft81x_board_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

void ft81x_board_cleanup(FT81X_Handle *handle)
{
}

#endif
