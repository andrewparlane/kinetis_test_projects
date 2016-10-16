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

// resources
#include "resources/cat_l8_compressed.h"
#include "resources/cat_argb1555_compressed.h"
#include "resources/cat_paletted8_compressed.h"
#include "resources/nyam_rgb565_compressed.h"
#include "resources/blink_font_l2_raw.h"

static const uint32_t clear_dl_snippet[] =
{
    // set the background colour to dark grey and clear the screen
    FT81X_DL_CMD_CLEAR_COLOUR_RGB(0,0,0),
    FT81X_DL_CMD_CLEAR(1,1,1),
};

ft81x_result test2_logo(FT81X_Handle *handle)
{
    ft81x_result res;

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

ft81x_result test2_compressed_images(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // load our resources into g RAM
    // ----------------------------------------------------------
    // first is the L8 image
    FT81X_Image_Handle cat_l8_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_l8_compressed_image_properties, &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // then the argb1555 image
    FT81X_Image_Handle cat_argb1555_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_argb1555_compressed_image_properties, &cat_argb1555_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // then the paletted8 image
    FT81X_Image_Handle cat_paletted8_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_paletted8_compressed_image_properties, &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
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

    // initialise the L8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the argb1555 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_argb1555_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // draw L8 image
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &cat_l8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 160-(cat_l8_compressed_image_properties.width/2),
                                                 240-(cat_l8_compressed_image_properties.height/2),
                                                 NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw ARGB1555 image
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &cat_argb1555_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 0, 0,
                                                 NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw paletted8 image
    // (note saves the context on the stack and restores it)
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &cat_paletted8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 162, 270,
                                                 NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
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

ft81x_result test2_text(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // load our custom font into g RAM
    // ----------------------------------------------------------
    FT81X_Font_Handle blink_font_handle;
    res = ft81x_text_manager_load_custom_font(handle, &blink_font_l2_raw_font_properties, &blink_font_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_load_custom_font failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Get font handles for inbuilt fonts
    // ----------------------------------------------------------
    FT81X_Font_Handle inbuilt_31_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_31_font_handle, 31);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_get_font_handle_for_inbuilt_font failed with %u\n", res);
        return res;
    }
    FT81X_Font_Handle inbuilt_34_font_handle;
    res = ft81x_text_manager_get_font_handle_for_inbuilt_font(handle, &inbuilt_34_font_handle, 34);
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

    // initialise the custom font
    // note: don't need to init inbuilt fonts
    // the API lets you, it just won't do anything
    res = ft81x_text_manager_send_font_init_dl(handle,
                                               &blink_font_handle,
                                               FT81X_BITMAP_FILTER_NEAREST,
                                               FT81X_BITMAP_WRAP_BORDER,
                                               FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_send_font_init_dl failed with %u\n", res);
        return res;
    }

    // change the colour for the folowwing text
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(255, 0, 0));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // show some text
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 10, 30, FT81X_TEXT_COORDS_TOP_LEFT, "Test 2");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // show some numbers
    res = ft81x_text_manager_write_unsigned_number(handle, &inbuilt_31_font_handle, 10, 100, FT81X_TEXT_COORDS_TOP_LEFT, 0xFFFFFFFF);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
        return res;
    }

    res = ft81x_text_manager_write_signed_number(handle, &inbuilt_31_font_handle, 10, 150, FT81X_TEXT_COORDS_TOP_LEFT, (int32_t)0xFFFFFFFF);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_signed_number failed with %u\n", res);
        return res;
    }

    // custom font text (note I replaced the data for ~ with a smiley)
    res = ft81x_text_manager_write_text(handle, &blink_font_handle, 10, 200, FT81X_TEXT_COORDS_TOP_LEFT, "Custom Font -> ~");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // numbers with custom font
    res = ft81x_text_manager_write_unsigned_number(handle, &blink_font_handle, 10, 230, FT81X_TEXT_COORDS_TOP_LEFT, (uint32_t)-1);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
        return res;
    }

    res = ft81x_text_manager_write_signed_number(handle, &blink_font_handle, 10, 260, FT81X_TEXT_COORDS_TOP_LEFT, -1);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_signed_number failed with %u\n", res);
        return res;
    }

    res = ft81x_text_manager_write_unsigned_number(handle, &blink_font_handle, 10, 290, FT81X_TEXT_COORDS_TOP_LEFT, 1337);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
        return res;
    }

    // test using inbuilt font 34 - note this has to be remapped
    // to a different bitmap_handle as only bitmap handles 0 - 31
    // are supported by the FT81X
    res = ft81x_text_manager_write_unsigned_number(handle, &inbuilt_34_font_handle, 10, 330, FT81X_TEXT_COORDS_TOP_LEFT, 34);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_unsigned_number failed with %u\n", res);
        return res;
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

ft81x_result test2_buttons(FT81X_Handle *handle)
{
    ft81x_result res;

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

    // draw a default button
    res = ft81x_coproc_cmd_button(handle, 10, 10, 200, 100, 24, 0, "button1");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_button failed with %u\n", res);
        return res;
    }

    // change the fg colour to red
    res = ft81x_coproc_cmd_fgcolour(handle, 255, 0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_fgcolour failed with %u\n", res);
        return res;
    }

    // draw a flat button
    res = ft81x_coproc_cmd_button(handle, 10, 120, 200, 100, 24, FT81X_COPROC_OPTION_FLAT, "flat button");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_button failed with %u\n", res);
        return res;
    }

    // change the fg colour to dark yellow
    res = ft81x_coproc_cmd_fgcolour(handle, 32, 32, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_fgcolour failed with %u\n", res);
        return res;
    }

    // set the gradient colour to blue
    res = ft81x_coproc_cmd_gradcolour(handle, 0, 0, 255);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_gradcolour failed with %u\n", res);
        return res;
    }

    // draw another button
    res = ft81x_coproc_cmd_button(handle, 10, 220, 250, 200, 28, 0, "button 3 with gradient");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_button failed with %u\n", res);
        return res;
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

ft81x_result test2_clocks_and_guages(FT81X_Handle *handle)
{
    ft81x_result res;

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

    // draw a default clock
    res = ft81x_coproc_cmd_clock(handle, 50, 50, 50, 0, 6, 30, 15, 500);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a default gauge
    res = ft81x_coproc_cmd_gauge(handle, 250, 50, 50, 0, 5, 2, 75, 100);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // change the bg colour to green
    res = ft81x_coproc_cmd_bgcolour(handle, 0, 255, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_bgcolour failed with %u\n", res);
        return res;
    }

    // draw a flat clock with no ticks
    res = ft81x_coproc_cmd_clock(handle, 50, 170, 60, FT81X_COPROC_OPTION_FLAT | FT81X_COPROC_OPTION_NOTICKS, 3, 45, 35, 100);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a flat gauge with no ticks
    res = ft81x_coproc_cmd_gauge(handle, 250, 170, 60, FT81X_COPROC_OPTION_FLAT | FT81X_COPROC_OPTION_NOTICKS, 5, 1, 25, 100);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a clock with no hands and no background
    res = ft81x_coproc_cmd_clock(handle, 100, 300, 90, FT81X_COPROC_OPTION_NOBACK | FT81X_COPROC_OPTION_NOHANDS, 3, 45, 35, 100);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a gauge with no pointer and no background
    res = ft81x_coproc_cmd_gauge(handle, 250, 300, 90, FT81X_COPROC_OPTION_NOPOINTER | FT81X_COPROC_OPTION_NOBACK, 2, 10, 25, 100);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
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

ft81x_result test2_keys_dials_toggles(FT81X_Handle *handle)
{
    ft81x_result res;

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

    // draw some keys
    res = ft81x_coproc_cmd_keys(handle, 50, 50, 270, 50, 23, 'a', "aaAbBcC");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a dial
    res = ft81x_coproc_cmd_dial(handle, 80, 240, 80, 0, 0x2000);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_dial failed with %u\n", res);
        return res;
    }

    // draw a toggle
    res = ft81x_coproc_cmd_toggle(handle, 50, 330, 80, 23, 0, 0, "off\xffon");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_toggle failed with %u\n", res);
        return res;
    }

    // change the fg colour to red
    res = ft81x_coproc_cmd_fgcolour(handle, 255, 0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_fgcolour failed with %u\n", res);
        return res;
    }

    // change the bg colour to white
    res = ft81x_coproc_cmd_bgcolour(handle, 255, 255, 255);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_bgcolour failed with %u\n", res);
        return res;
    }

    // set the text colour to black
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(0,0,0));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // change the grad colour to blue
    res = ft81x_coproc_cmd_gradcolour(handle, 0, 0, 255);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_gradcolour failed with %u\n", res);
        return res;
    }

    // draw some keys
    res = ft81x_coproc_cmd_keys(handle, 50, 110, 250, 50, 23, FT81X_COPROC_OPTION_CENTRE | 'B', "aAbBcC123");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_clock failed with %u\n", res);
        return res;
    }

    // draw a flat dial
    res = ft81x_coproc_cmd_dial(handle, 240, 240, 80, FT81X_COPROC_OPTION_FLAT, 6000);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_dial failed with %u\n", res);
        return res;
    }

    // draw a flat toggle
    res = ft81x_coproc_cmd_toggle(handle, 190, 330, 80, 23, 0, 1, "off\xffon");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_toggle failed with %u\n", res);
        return res;
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

ft81x_result test2_gradient(FT81X_Handle *handle)
{
    ft81x_result res;

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

    // show a gradient
    res = ft81x_coproc_cmd_gradient(handle, 200, 100, 255, 0, 0, 300, 400, 0, 0, 255);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_gradient failed with %u\n", res);
        return res;
    }

#warning TODO: add support for scissor commands then improve test2_gradient()

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

ft81x_result test2_progress_scrollbars_sliders(FT81X_Handle *handle)
{
    ft81x_result res;

    for (int i = 0; i <= 100; i++)
    {
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

        // show a progress bar
        res = ft81x_coproc_cmd_progress(handle, 50, 50, 220, 30, 0, i, 100);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_progress failed with %u\n", res);
            return res;
        }

        // show a horizontal scrollbar
        res = ft81x_coproc_cmd_scrollbar(handle, 0, 460, 320, 20, 0, i, 10, 110);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_scrollbar failed with %u\n", res);
            return res;
        }

        // show a vertical scrollbar
        res = ft81x_coproc_cmd_scrollbar(handle, 300, 0, 20, 480, 0, i, 30, 130);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_scrollbar failed with %u\n", res);
            return res;
        }

        // show a slider
        res = ft81x_coproc_cmd_slider(handle, 50, 150, 100, 40, 0, i, 100);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_slider failed with %u\n", res);
            return res;
        }

        // show the current value of i
        res = ft81x_coproc_cmd_number(handle, 100, 300, 31, FT81X_COPROC_OPTION_RIGHTX, i);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_number failed with %u\n", res);
            return res;
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

        vTaskDelay(100);
    }
    vTaskDelay(3000);

    return FT81X_RESULT_OK;
}

ft81x_result test2_transformations(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // load our resources into g RAM
    // ----------------------------------------------------------
    FT81X_Image_Handle nyam_rgb565_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &nyam_rgb565_compressed_image_properties, &nyam_rgb565_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
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

    // initialise the rgb565 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &nyam_rgb565_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // draw rgb565 image
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &nyam_rgb565_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 0, 0,
                                                 NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw the rgb565 image again this time twice as big
    FT81X_Image_Transform tform =
    {
        .scale          = 1,        // scale by 3/4 in both dimensions
        .scale_x        = 2,
        .scale_y        = 2,
    };
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &nyam_rgb565_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 100, 0,
                                                 &tform);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
    }

    // again buut half the original size
    tform.scale_x = 0.5;
    tform.scale_y = 0.5;
    res = ft81x_image_manager_send_image_draw_dl(handle,
                                                 &nyam_rgb565_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 0, 50,
                                                 &tform);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl failed with %u\n", res);
        return res;
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

ft81x_result test2_spinners(FT81X_Handle *handle)
{
    ft81x_result res;

    uint8_t colours[4][3] =
    {
        { 255, 0, 0 },
        { 0, 255, 0 },
        { 0, 0, 255 },
        { 255, 255, 255 },
    };

    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    for (int i = 0; i < 4; i++)
    {
        // clear the screen
        res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
            return res;
        }

        // put something on the screen
        uint32_t draw_lines[] =
        {
            FT81X_DL_CMD_COLOUR_RGB(255, 255, 255),
            FT81X_DL_CMD_LINE_WIDTH(5 * 16),
            FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_LINES),
                FT81X_DL_CMD_VERTEX2II(0, 0, 0, 0),
                FT81X_DL_CMD_VERTEX2II(320, 480, 0, 0),
                FT81X_DL_CMD_VERTEX2II(320, 0, 0, 0),
                FT81X_DL_CMD_VERTEX2II(0, 480, 0, 0),
                FT81X_DL_CMD_VERTEX2II(0, 0, 0, 0),
            FT81X_DL_CMD_END(),
        };
        res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(draw_lines), draw_lines);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
            return res;
        }

        // set the spinner colour
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(colours[i][0],colours[i][1],colours[i][2]));
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
            return res;
        }

        // start a spinner
        res = ft81x_coproc_cmd_spinner(handle, 160, 240, i, 1);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_spinner failed with %u\n", res);
            return res;
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

        vTaskDelay(2000);

        // stop it
        res = ft81x_coproc_cmd_stop(handle);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_coproc_cmd_stop failed with %u\n", res);
            return res;
        }

        res = ft81x_graphics_engine_flush_and_synchronise(handle);
        if (res != FT81X_RESULT_OK)
        {
            DbgConsole_Printf("ft81x_graphics_engine_flush_and_synchronise failed with %u\n", res);
            return res;
        }

        vTaskDelay(1000);
    }

    return FT81X_RESULT_OK;
}

ft81x_result test2_screensaver(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // load our resources into g RAM
    // ----------------------------------------------------------
    // The l8 image
    FT81X_Image_Handle cat_l8_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_l8_compressed_image_properties, &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // The paletted8 image
    FT81X_Image_Handle cat_paletted8_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_paletted8_compressed_image_properties, &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
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

    res = ft81x_coproc_cmd_screensaver(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_screensaver failed with %u\n", res);
        return res;
    }

    // clear the screen
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // initialise the l8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // because we are using vertex2f (see CMD_SCREENSAVER)
    // we have to specify the bitmap source first
    res = ft81x_image_manager_set_active_bitmap(handle, &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_set_active_bitmap failed with %u\n", res);
        return res;
    }

    // draw l8 image using macro_0
    // the co-proc replaces macro_0 with vertex2f
    // due to the CMD_SCREENSAVER above
    res = ft81x_image_manager_send_image_draw_dl_with_macro_0(handle,
                                                              &cat_l8_compressed_image_handle,
                                                              FT81X_BITMAP_FILTER_NEAREST,
                                                              NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl_with_macro_0 failed with %u\n", res);
        return res;
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

    vTaskDelay(5000);

    // stop it
    res = ft81x_coproc_cmd_stop(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_stop failed with %u\n", res);
        return res;
    }

    // reset the co-proc to it's default state
    res = ft81x_coproc_cmd_coldstart(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_coldstart failed with %u\n", res);
        return res;
    }

    res = ft81x_coproc_cmd_screensaver(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_screensaver failed with %u\n", res);
        return res;
    }

    // clear the screen
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(clear_dl_snippet), clear_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // initialise the l8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // because we are using vertex2f (see CMD_SCREENSAVER)
    // we have to specify the bitmap source first
    res = ft81x_image_manager_set_active_bitmap(handle, &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_set_active_bitmap failed with %u\n", res);
        return res;
    }

    // draw paletted8 image using macro_0
    // the co-proc replaces macro_0 with vertex2f
    // due to the CMD_SCREENSAVER above
    res = ft81x_image_manager_send_image_draw_dl_with_macro_0(handle,
                                                              &cat_paletted8_compressed_image_handle,
                                                              FT81X_BITMAP_FILTER_NEAREST,
                                                              NULL);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_draw_dl_with_macro_0 failed with %u\n", res);
        return res;
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

    vTaskDelay(5000);

    // stop it
    res = ft81x_coproc_cmd_stop(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_stop failed with %u\n", res);
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

ft81x_result test2(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // Display some text
    // ----------------------------------------------------------
    res = test2_text(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_text failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display the FTDI logo animation
    // ----------------------------------------------------------
    res = test2_logo(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_logo failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display some compressed images
    // ----------------------------------------------------------
    res = test2_compressed_images(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_compressed_images failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display some buttons
    // ----------------------------------------------------------
    res = test2_buttons(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_buttons failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display some clocks and guages
    // ----------------------------------------------------------
    res = test2_clocks_and_guages(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_clocks_and_guages failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display some keys, dials and toggles
    // ----------------------------------------------------------
    res = test2_keys_dials_toggles(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_keys_dials_toggles failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display a gradient
    // ----------------------------------------------------------
    res = test2_gradient(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_gradient failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Display a progress bar, some scrollbars and sliders
    // ----------------------------------------------------------
    res = test2_progress_scrollbars_sliders(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_progress_scrollbars_sliders failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Demonstrtate transformations
    // ----------------------------------------------------------
    res = test2_transformations(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_transformations failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Demonstrtate spinners
    // ----------------------------------------------------------
    res = test2_spinners(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_spinners failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Demonstrtate screensaver
    // ----------------------------------------------------------
    res = test2_screensaver(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2_screensaver failed with %u\n", res);
        return res;
    }

    return FT81X_RESULT_OK;
}
