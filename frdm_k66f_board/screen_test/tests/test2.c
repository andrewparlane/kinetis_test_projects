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
                                                 &cat_l8_compressed_image_properties,
                                                 &cat_l8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the argb1555 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_argb1555_compressed_image_properties,
                                                 &cat_argb1555_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_properties,
                                                 &cat_paletted8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // draw L8 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_l8_compressed_image_handle,
                                                               160-(cat_l8_compressed_image_properties.width/2),
                                                               240-(cat_l8_compressed_image_properties.height/2));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_non_paletted8_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw ARGB1555 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_argb1555_compressed_image_handle,
                                                               0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_non_paletted8_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw paletted8 image
    // (note saves the context on the stack and restores it)
    res = ft81x_image_manager_send_paletted8_image_draw_dl(handle,
                                                           &cat_paletted8_compressed_image_handle,
                                                           162, 270);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_paletted8_image_draw_dl failed with %u\n", res);
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
                                               &blink_font_l2_raw_font_properties,
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
    FT81X_Image_Handle cat_argb1555_compressed_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_argb1555_compressed_image_properties, &cat_argb1555_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
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

    // initialise the argb1555 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_argb1555_compressed_image_properties,
                                                 &cat_argb1555_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // draw ARGB1555 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_argb1555_compressed_image_handle,
                                                               0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_non_paletted8_image_draw_dl failed with %u\n", res);
        return res;
    }

    // draw some text
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 79, 230, FT81X_TEXT_COORDS_CENTRE, "Transforms");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // load the identity matrix
    res = ft81x_coproc_cmd_loadidentity(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_loadidentity failed with %u\n", res);
        return res;
    }

    // set up a translation to move the rotate and
    // scale point to the middle of the image
    res = ft81x_coproc_cmd_translate_int(handle, cat_argb1555_compressed_image_properties.width/2, cat_argb1555_compressed_image_properties.height/2);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_translate_int failed with %u\n", res);
        return res;
    }

    // scale by 3/4 about the centre
    res = ft81x_coproc_cmd_scale_double(handle, 0.75, 0.75);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_scale_double failed with %u\n", res);
        return res;
    }

    // rotate clockwise 45 degrees around the middle of the image
    res = ft81x_coproc_cmd_rotate_int(handle, 45);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_rotate_int failed with %u\n", res);
        return res;
    }

    // reverse the translation, so the
    // top left corner is where we want it
    res = ft81x_coproc_cmd_translate_int(handle, -cat_argb1555_compressed_image_properties.width/2, -cat_argb1555_compressed_image_properties.height/2);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_translate_int failed with %u\n", res);
        return res;
    }

    // use this matrix for drawing
    res = ft81x_coproc_cmd_setmatrix(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_setmatrix failed with %u\n", res);
        return res;
    }

    // draw the ARGB1555 image again
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_argb1555_compressed_image_handle,
                                                               158, 210);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_non_paletted8_image_draw_dl failed with %u\n", res);
        return res;
    }

    // load the identity matrix
    res = ft81x_coproc_cmd_loadidentity(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_loadidentity failed with %u\n", res);
        return res;
    }

    // scale to 2/5th about the centre
    res = ft81x_coproc_cmd_scale_double(handle, 0.4, 0.4);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_scale_double failed with %u\n", res);
        return res;
    }

    // use this matrix for drawing
    res = ft81x_coproc_cmd_setmatrix(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_setmatrix failed with %u\n", res);
        return res;
    }

    // show some text again
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 237, 450, FT81X_TEXT_COORDS_CENTRE, "Transforms");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
        return res;
    }

    // show some text again
    res = ft81x_text_manager_write_text(handle, &inbuilt_31_font_handle, 0, 400, FT81X_TEXT_COORDS_TOP_LEFT, "Transforms");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_text_manager_write_text failed with %u\n", res);
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
                                                 &cat_l8_compressed_image_properties,
                                                 &cat_l8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_properties,
                                                 &cat_paletted8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
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
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl_with_macro_0(handle,
                                                                            &cat_l8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_paletted8_image_draw_dl failed with %u\n", res);
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
                                                 &cat_l8_compressed_image_properties,
                                                 &cat_l8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_image_init_dl failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_compressed_image_properties,
                                                 &cat_paletted8_compressed_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
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
    res = ft81x_image_manager_send_paletted8_image_draw_dl_with_macro_0(handle,
                                                                        &cat_paletted8_compressed_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_send_paletted8_image_draw_dl failed with %u\n", res);
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
