#include "ft81x/boards/board.h" // defines the display type
#include "ft81x/platforms/platform.h"

// only compile this file if we have the ILI9488 display defined
#if ((FT81X_DISPLAY) == (FT81X_DISPLAY_WINDOWS_EMULATOR))

ft81x_result ft81x_display_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_display_comms_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

void ft81x_display_cleanup(FT81X_Handle *handle)
{
}

ft81x_result ft81x_display_read_id(FT81X_Handle *handle, uint32_t *id)
{
    // use a fake ID
    *id = 0xDEADBEEF;
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_display_send_configuration(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

#endif
