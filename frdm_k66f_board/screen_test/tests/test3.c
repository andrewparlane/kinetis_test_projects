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
#include "ft81x_touch_manager.h"

static const uint32_t clear_dl_snippet[] =
{
    // set the background colour to dark grey and clear the screen
    FT81X_DL_CMD_CLEAR_COLOUR_RGB(0,0,0),
    FT81X_DL_CMD_CLEAR(1,1,1),
};

ft81x_result test3_calibrate(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle inbuilt_26_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_26_font_handle, 26);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    FT81X_Font_Handle inbuilt_31_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_31_font_handle, 31);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    // clear the screen
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // set the text colour to red
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(255,0,0));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // write "calibrating..." in the middle of the screen
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 160, 200, FT81X_TEXT_COORDS_CENTRE, "Calibrating...");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // write "Tap the flashing dots." a bit below
    res = ft81x_text_manager_write_text(handle, &inbuilt_26_font_handle, 160, 240, FT81X_TEXT_COORDS_CENTRE, "Tap the flashing dots.");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // set the calibration dots to green
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0,255,0));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // calibrate
    // Note: This sends the DISPLAY() instruction and swaps the DL in
    //       Any DL stuff before this will be in the background of the
    //       calibration process
    res = ft81x_touch_manager_calibrate(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_touch_manager_calibrate failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result test3(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Calibrate
    // ----------------------------------------------------------
    res = test3_calibrate(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test3_calibrate failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}