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

ft81x_result test3_set_screen_orientation(FT81X_Handle *handle)
{
    ft81x_result res;

    res = ft81x_coproc_cmd_setrotate(handle, FT81X_COPROC_SCREEN_ORIENTATION_ROTATED);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_setrotate failed with %u\n", res);
        return res;
    }

    res = ft81x_graphics_engine_flush_and_synchronise(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_flush_and_synchronise failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}

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
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 160, 220, FT81X_TEXT_COORDS_CENTRE, "Calibrating...");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // write "Tap the flashing dots." a bit below
    res = ft81x_text_manager_write_text(handle, &inbuilt_26_font_handle, 160, 260, FT81X_TEXT_COORDS_CENTRE, "Tap the flashing dots.");
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

ft81x_result test3_display_calibration_matrix(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle inbuilt_28_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_28_font_handle, 28);
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

    // set the text colour to blue
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0,0,255));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // get the matrix values
    uint32_t values[6];
    res = ft81x_touch_manager_get_calibration_values(handle, values);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_touch_manager_get_calibration_values failed with %u\n", res);
        return res;
    }

    res = ft81x_text_manager_write_text(handle, &inbuilt_28_font_handle, 240, 50, FT81X_TEXT_COORDS_CENTRE, "Calibration results");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // set the base to 16 (hex)
    res = ft81x_coproc_cmd_setbase(handle, 16);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_setbase failed with %u\n", res);
        return res;
    }

    uint16_t x_co_ords[] = { 120, 240, 360, 120, 240, 360 };
    uint16_t y_co_ords[] = { 150, 150, 150, 200, 200, 200 };
    for (int i = 0; i < 6; i++)
    {
        res = ft81x_text_manager_write_unsigned_number(handle, &inbuilt_28_font_handle, x_co_ords[i], y_co_ords[i], FT81X_TEXT_COORDS_CENTRE, values[i]);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
            return res;
        }
    }

    // display it
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_DISPLAY());
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
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

ft81x_result test3_tags(FT81X_Handle *handle)
{
    ft81x_result res;

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

    uint8_t touched_tag = 0;
    while (1)
    {
        // draw a couple of rectangles
        // some with tags, some not
        // red = with tag, not pressed
        // green = with tag, pressed
        // blue = no tag
        const uint32_t dl[] =
        {
            // clear the tag buffer to 4s
            // when tag 4 is pressed, set the background colour to green
            // this should happen when you touch anywhere that's not another tag
            FT81X_DL_CMD_CLEAR_TAG(4),
            FT81X_DL_CMD_CLEAR_COLOUR_RGB(0,
                                          (touched_tag == 4) ? 255 : 0,
                                          0),
            FT81X_DL_CMD_CLEAR(1,1,1),

            // set up a rectangle with tag 1
            // when pressed it's red, otherwise green
            FT81X_DL_CMD_TAG(1),
            FT81X_DL_CMD_TAG_MASK(1),
            FT81X_DL_CMD_COLOUR_RGB((touched_tag == 1) ? 0 : 255,
                                    (touched_tag == 1) ? 255 : 0,
                                    0),
            FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
                FT81X_DL_CMD_VERTEX2II(50, 50, 0, 0),
                FT81X_DL_CMD_VERTEX2II(150, 150, 0, 0),
            FT81X_DL_CMD_END(),

            // set up another rectangle with no tag (ie the clear tag)
            // it's blue, we set it green if tag 2 is pressed, to prove
            // that TAG_MASK works
            FT81X_DL_CMD_TAG(2),
            FT81X_DL_CMD_TAG_MASK(0),
            FT81X_DL_CMD_COLOUR_RGB(0,
                                    (touched_tag == 2) ? 255 : 0,
                                    (touched_tag == 2) ? 0 : 255),
            FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
                FT81X_DL_CMD_VERTEX2II(200, 50, 0, 0),
                FT81X_DL_CMD_VERTEX2II(300, 150, 0, 0),
            FT81X_DL_CMD_END(),

            // set up a third rectangle with tag 3
            // it's red when it's not press,d and green when it is
            FT81X_DL_CMD_TAG(3),
            FT81X_DL_CMD_TAG_MASK(1),
            FT81X_DL_CMD_COLOUR_RGB((touched_tag == 3) ? 0 : 255,
                                    (touched_tag == 3) ? 255 : 0,
                                    0),
            FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
                FT81X_DL_CMD_VERTEX2II(50, 200, 0, 0),
                FT81X_DL_CMD_VERTEX2II(150, 300, 0, 0),
            FT81X_DL_CMD_END(),

            FT81X_DL_CMD_DISPLAY(),
        };

        res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
            return res;
        }

        // write everything to the DL ram and then swap it in
        res = ft81x_graphics_engine_end_display_list(handle);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_end_display_list failed with %u\n", res);
            return res;
        }

        uint8_t last_tag = touched_tag;
        while(touched_tag == last_tag)
        {
            res = ft81x_touch_manager_check_for_touched_tag(handle, &touched_tag);
            if (res != FT81X_RESULT_OK)
            {
                DbgConsole_Printf("ft81x_touch_manager_check_for_touched_tag failed with %u\n", res);
                return res;
            }
        }
        DbgConsole_Printf("last_tag %d, touched_tag %d\n", last_tag, touched_tag);
    }
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

    // ----------------------------------------------------------
    // Set the screen to landscape mode
    // ----------------------------------------------------------
    res = test3_set_screen_orientation(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test3_set_screen_orientation failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Show the calibration results
    // ----------------------------------------------------------
    res = test3_display_calibration_matrix(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test3_display_calibration_matrix failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Do a tag test
    // ----------------------------------------------------------
    res = test3_tags(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test3_tags failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}
