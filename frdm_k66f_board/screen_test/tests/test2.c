#include "fsl_debug_console.h"

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// ft81x_lib includes
#include "ft81x.h"
#include "ft81x_display_list.h"
#include "ft81x_co_processor.h"
#include "ft81x_graphics_engine.h"
#include "ft81x_image_manager.h"
#include "ft81x_text_manager.h"

static const uint32_t clear_dl_snippet[] =
{
    // set the background colour to dark grey and clear the screen
    FT81X_DL_CMD_CLEAR_COLOUR_RGB(64,0,0),
    FT81X_DL_CMD_CLEAR(1,1,1),
};

ft81x_result test2(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Display the FTDI logo animation
    // ----------------------------------------------------------
    res = ft81x_coproc_cmd_logo(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_logo failed with %u\n", res);
        return res;
    }
    // write everything to the DL ram and then swap it in
    res = ft81x_graphics_engine_end_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_end_display_list failed with %u\n", res);
        return res;
    }
    vTaskDelay(3000);

    return FT81X_RESULT_OK;
}
