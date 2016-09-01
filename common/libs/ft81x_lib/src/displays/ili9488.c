#include "ft81x/boards/board.h" // defines the display type

#include "ft81x/ft81x_types.h"

// only compile this file if we have the ILI9488 display defined
#if FT81X_DISPLAY == FT81X_DISPLAY_ILI9488

ft81x_result ft81x_display_initialise(void *platform_user_data)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_display_comms_initialise(void *platform_user_data)
{
    // this display needs some set up so we need to initialise the comms
    return ft81x_board_display_comms_initialise(platform_user_data);
}

#endif
