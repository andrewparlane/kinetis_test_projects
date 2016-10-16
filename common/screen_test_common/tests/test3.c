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

// standard includes
#include <stdio.h>

//#define USE_HARDCODED_CALIB_VALUES

uint32_t hardcoded_calib_values[6] =
{
    0x00006D9C, 0xFFFFFEA9, 0xFFC41058,
    0x00000259, 0xFFFF695B, 0x021BA87D,
};

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
        printf("ft81x_coproc_cmd_setrotate failed with %u\n", res);
        return res;
    }

    res = ft81x_graphics_engine_flush_and_synchronise(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_flush_and_synchronise failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result test3_calibrate(FT81X_Handle *handle)
{
    ft81x_result res;

#ifdef USE_HARDCODED_CALIB_VALUES
    res = ft81x_touch_manager_write_calibration_values(handle, hardcoded_calib_values);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_touch_manager_set_calibration_values failed with %u\n", res);
        return res;
    }
#else
    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle inbuilt_26_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_26_font_handle, 26);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    FT81X_Font_Handle inbuilt_31_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_31_font_handle, 31);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    // clear the screen
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // set the text colour to red
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(255,0,0));
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // write "calibrating..." in the middle of the screen
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 160, 220, FT81X_TEXT_COORDS_CENTRE, "Calibrating...");
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // write "Tap the flashing dots." a bit below
    res = ft81x_text_manager_write_text(handle, &inbuilt_26_font_handle, 160, 260, FT81X_TEXT_COORDS_CENTRE, "Tap the flashing dots.");
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // set the calibration dots to green
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0,255,0));
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // calibrate
    // Note: This sends the DISPLAY() instruction and swaps the DL in
    //       Any DL stuff before this will be in the background of the
    //       calibration process
    res = ft81x_touch_manager_calibrate(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_touch_manager_calibrate failed with %u\n", res);
        return res;
    }

    // get the matrix values
    uint32_t values[6];
    res = ft81x_touch_manager_get_calibration_values(handle, values);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_touch_manager_get_calibration_values failed with %u\n", res);
        return res;
    }

    printf("0x%08X, 0x%08X, 0x%08X,\n", values[0], values[1], values[2]);
    printf("0x%08X, 0x%08X, 0x%08X,\n", values[3], values[4], values[5]);
#endif
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
        printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    // clear the screen
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // set the text colour to blue
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0,0,255));
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // get the matrix values
    uint32_t values[6];
    res = ft81x_touch_manager_get_calibration_values(handle, values);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_touch_manager_get_calibration_values failed with %u\n", res);
        return res;
    }

    res = ft81x_text_manager_write_text(handle, &inbuilt_28_font_handle, 240, 50, FT81X_TEXT_COORDS_CENTRE, "Calibration results");
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // set the base to 16 (hex)
    res = ft81x_coproc_cmd_setbase(handle, 16);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_coproc_cmd_setbase failed with %u\n", res);
        return res;
    }

    uint16_t x_co_ords[] = { 120, 240, 360, 120, 240, 360 };
    uint16_t y_co_ords[] = { 150, 150, 150, 200, 200, 200 };
    for (int i = 0; i < 6; i++)
    {
        res = ft81x_text_manager_write_unsigned_number(handle, &inbuilt_28_font_handle, x_co_ords[i], y_co_ords[i], FT81X_TEXT_COORDS_CENTRE, values[i]);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
            return res;
        }
    }

    // display it
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_DISPLAY());
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // write everything to the DL ram and then swap it in
    res = ft81x_graphics_engine_end_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_graphics_engine_end_display_list failed with %u\n", res);
        return res;
    }

    vTaskDelay(3000);

    return FT81X_RESULT_OK;
}

ft81x_result test3_tags(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle inbuilt_31_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_31_font_handle, 31);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
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

            // set up a third rectangle with tag 5
            // it's always red
            FT81X_DL_CMD_TAG(5),
            FT81X_DL_CMD_TAG_MASK(1),
            FT81X_DL_CMD_COLOUR_RGB(255, 0, 0),
            FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
                FT81X_DL_CMD_VERTEX2II(380, 270, 0, 0),
                FT81X_DL_CMD_VERTEX2II(480, 320, 0, 0),
            FT81X_DL_CMD_END(),

            // disable tagging
            FT81X_DL_CMD_TAG_MASK(0),

            // set the font text to white
            FT81X_DL_CMD_COLOUR_RGB(255, 255, 255),
        };

        res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
            return res;
        }

        // Label the exit button
        res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 430, 295, FT81X_TEXT_COORDS_CENTRE, "EXIT");
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_text_manager_write_text failed with %u\n", res);
            return res;
        }

        // display it
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_DISPLAY());
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
            return res;
        }

        // write everything to the DL ram and then swap it in
        res = ft81x_graphics_engine_end_display_list(handle);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_end_display_list failed with %u\n", res);
            return res;
        }

        uint8_t last_tag = touched_tag;
        while(touched_tag == last_tag)
        {
            res = ft81x_touch_manager_check_for_touched_tag(handle, &touched_tag, NULL);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_touch_manager_check_for_touched_tag failed with %u\n", res);
                return res;
            }
        }
        printf("last_tag %d, touched_tag %d\n", last_tag, touched_tag);

        if (touched_tag == 5)
        {
            // exit button pressed
            break;
        }
    }

    return FT81X_RESULT_OK;
}

ft81x_result test3_tracking(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &font_handle, 29);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    typedef struct
    {
        char *name;
        uint16_t value;
    } Control;

    Control controls[] =
    {
        { "Volume",     0x8000 },
        { "Low",        0x1000 },
        { "Med",        0x1000 },
        { "High",       0x1000 },
        { "WOMWOMWOM",  0xC000 },
        { "WOBWOBWOB",  0xC000 },
    };

    const uint32_t NUM_CONTROLS = sizeof(controls) / sizeof(Control);

    const uint32_t CONTROLS_OFFSET = 180;

    uint8_t touched_tag = 0;
    uint16_t touched_tag_track = 0;
    uint16_t scroll_offset = 0;
    while (1)
    {
        // clear the screen
        res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
            return res;
        }

        // draw the controls
        uint16_t start_y = 0;
        for (uint32_t i = 0; i < NUM_CONTROLS; i++)
        {
            // disable tags for the text
            res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG_MASK(0));
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
                return res;
            }

            // write the control's name
            res = ft81x_text_manager_write_text(handle, &font_handle, 325, start_y + 30 - scroll_offset, FT81X_TEXT_COORDS_CENTRE_X, controls[i].name);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_text_manager_write_text failed with %u\n", res);
                return res;
            }

            // enable tags for the rest
            res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG_MASK(1));
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
                return res;
            }

            // draw the dial as tag ((i * 2) + 1)
            res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG((i * 2) + 1));
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
                return res;
            }

            // draw a dial
            uint16_t dial_x = 100;
            uint16_t dial_y = start_y + 100 - scroll_offset;
            res = ft81x_coproc_cmd_dial(handle, dial_x, dial_y, 70, 0, controls[i].value);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_coproc_cmd_dial failed with %u\n", res);
                return res;
            }

            // track the dial
            res = ft81x_touch_manager_track_rotary_region(handle, dial_x, dial_y, (i * 2) + 1);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_touch_manager_track_rotary_region failed with %u\n", res);
                return res;
            }

            // draw the slider as tag (i * 2) + 2
            res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG((i * 2) + 2));
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
                return res;
            }

            // draw a slider
            uint16_t slider_x = 250;
            uint16_t slider_y = start_y + 95 - scroll_offset;
            res = ft81x_coproc_cmd_slider(handle, slider_x, slider_y, 150, 10, 0, controls[i].value, 65535);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_coproc_cmd_slider failed with %u\n", res);
                return res;
            }

            // track the slider (support twice the height, to make it easier)
            res = ft81x_touch_manager_track_linear_region(handle, slider_x, slider_y-5, 150, 20, (i * 2) + 2);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_touch_manager_track_linear_region failed with %u\n", res);
                return res;
            }

            start_y += CONTROLS_OFFSET;
        }

        // tag the exit button as (NUM_CONTROLS * 2) + 1
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG((NUM_CONTROLS * 2) + 1));
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
            return res;
        }

        // add an exit button to the bottom
        res = ft81x_coproc_cmd_button(handle, 190, start_y - scroll_offset, 100, 50, 31, 0, "EXIT");
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_coproc_cmd_button failed with %u\n", res);
            return res;
        }

        // tag the scrollbar as (NUM_CONTROLS * 2) + 2
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_TAG((NUM_CONTROLS * 2) + 2));
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
            return res;
        }

        // draw a scrollbar
        // don't quite go to the edges, as it makes it easier to touch
        uint16_t total_height = start_y + 200;
        res = ft81x_coproc_cmd_scrollbar(handle, 450, 20, 30, 280, 0, scroll_offset, 320, total_height);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_coproc_cmd_scrollbar failed with %u\n", res);
            return res;
        }

        // track the scrollbar
        res = ft81x_touch_manager_track_linear_region(handle, 420, 20, 60, 280, (NUM_CONTROLS * 2) + 2);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_touch_manager_track_linear_region failed with %u\n", res);
            return res;
        }

        // display it
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_DISPLAY());
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
            return res;
        }

        // write everything to the DL ram and then swap it in
        res = ft81x_graphics_engine_end_display_list(handle);
        if (res != FT81X_RESULT_OK)
        {
            printf("ft81x_graphics_engine_end_display_list failed with %u\n", res);
            return res;
        }

        // wait for a touch event
        uint8_t last_tag = touched_tag;
        uint16_t last_tracked = touched_tag_track;
        while(touched_tag == last_tag &&
              touched_tag_track == last_tracked)
        {
            res = ft81x_touch_manager_check_for_touched_tag(handle, &touched_tag, &touched_tag_track);
            if (res != FT81X_RESULT_OK)
            {
                printf("ft81x_touch_manager_check_for_touched_tag failed with %u\n", res);
                return res;
            }
        }

        if (touched_tag > 0)
        {
            // touched something
            if (touched_tag <= (NUM_CONTROLS * 2))
            {
                // it's a control.
                // update
                uint8_t control_idx = (touched_tag - 1) / 2;
                controls[control_idx].value = touched_tag_track;
            }
            else if (touched_tag == ((NUM_CONTROLS * 2) + 1))
            {
                // exit button
                break;
            }
            else if (touched_tag == ((NUM_CONTROLS * 2) + 2))
            {
                // scrollbar
                scroll_offset = (touched_tag_track * total_height) / 65536;
                if (scroll_offset > (total_height - 320))
                {
                    scroll_offset = (total_height - 320);
                }
            }
        }
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
        printf("test3_calibrate failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Set the screen to landscape mode
    // ----------------------------------------------------------
    res = test3_set_screen_orientation(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("test3_set_screen_orientation failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Show the calibration results
    // ----------------------------------------------------------
    res = test3_display_calibration_matrix(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("test3_display_calibration_matrix failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Do a tag test
    // ----------------------------------------------------------
    res = test3_tags(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("test3_tags failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Do a tracking test
    // ----------------------------------------------------------
    res = test3_tracking(handle);
    if (res != FT81X_RESULT_OK)
    {
        printf("test3_tracking failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}
