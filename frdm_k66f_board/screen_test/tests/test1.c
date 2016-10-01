#include "fsl_debug_console.h"

// ft81x_lib includes
#include "ft81x.h"
#include "ft81x_graphics_engine.h"
#include "ft81x_display_list.h"
#include "ft81x_image_manager.h"
#include "ft81x_text_manager.h"

// resources
#include "resources/cat_l8_raw.h"
#include "resources/cat_argb1555_raw.h"
#include "resources/cat_paletted8_raw.h"

static const uint32_t clear_dl_snippet[] =
{
    // set the background colour to dark grey and clear the screen
    FT81X_DL_CMD_CLEAR_COLOUR_RGB(32,32,32),
    FT81X_DL_CMD_CLEAR(1,1,1),
};

static const uint32_t test_dl_snippet[] =
{
        // draw some points
        FT81X_DL_CMD_POINT_SIZE(20 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 0, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_POINTS),
            FT81X_DL_CMD_VERTEX2II(5, 5, 0, 0),
        FT81X_DL_CMD_END(),

        FT81X_DL_CMD_POINT_SIZE(40 * 16),
        FT81X_DL_CMD_COLOUR_RGB(0, 255, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_POINTS),
            FT81X_DL_CMD_VERTEX2II(315, 5, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a line
        FT81X_DL_CMD_LINE_WIDTH(5 * 16),
        FT81X_DL_CMD_COLOUR_RGB(0, 0, 255),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_LINES),
            FT81X_DL_CMD_VERTEX2II(10, 10, 0, 0),
            FT81X_DL_CMD_VERTEX2II(310, 470, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a rect
        FT81X_DL_CMD_LINE_WIDTH(1 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 255, 0),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
            FT81X_DL_CMD_VERTEX2II(300, 199, 0, 0),
            FT81X_DL_CMD_VERTEX2II(200, 280, 0, 0),
        FT81X_DL_CMD_END(),

        // draw a rect with rounded corners
        FT81X_DL_CMD_LINE_WIDTH(10 * 16),
        FT81X_DL_CMD_COLOUR_RGB(255, 0, 255),
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_RECTS),
            FT81X_DL_CMD_VERTEX2II(50, 300, 0, 0),
            FT81X_DL_CMD_VERTEX2II(100, 450, 0, 0),
        FT81X_DL_CMD_END(),
};

ft81x_result test1(FT81X_Handle *handle)
{
    ft81x_result res;

    // ----------------------------------------------------------
    // load our resources into g RAM
    // ----------------------------------------------------------
    // first is the L8 image
    FT81X_Image_Handle cat_l8_raw_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_l8_raw_image_properties, &cat_l8_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // then the argb1555 image
    FT81X_Image_Handle cat_argb1555_raw_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_argb1555_raw_image_properties, &cat_argb1555_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // then the paletted8 image
    FT81X_Image_Handle cat_paletted8_raw_image_handle;
    res = ft81x_image_manager_load_image(handle, &cat_paletted8_raw_image_properties, &cat_paletted8_raw_image_handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_image_manager_load_image failed with %u\n", res);
        return res;
    }

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
    // start the display list
    res = ft81x_graphics_engine_start_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_start_display_list failed with %u\n", res);
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
                                                 &cat_l8_raw_image_properties,
                                                 &cat_l8_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // initialise the argb1555 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_argb1555_raw_image_properties,
                                                 &cat_argb1555_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // initialise the paletted8 image in the DL
    res = ft81x_image_manager_send_image_init_dl(handle,
                                                 &cat_paletted8_raw_image_properties,
                                                 &cat_paletted8_raw_image_handle,
                                                 FT81X_BITMAP_FILTER_NEAREST,
                                                 FT81X_BITMAP_WRAP_BORDER,
                                                 FT81X_BITMAP_WRAP_BORDER);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // draw L8 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_l8_raw_image_handle,
                                                               160-(cat_l8_raw_image_properties.width/2),
                                                               240-(cat_l8_raw_image_properties.height/2));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // draw ARGB1555 image
    res = ft81x_image_manager_send_non_paletted8_image_draw_dl(handle,
                                                               &cat_argb1555_raw_image_handle,
                                                               0, 0);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // draw paletted8 image
    // (note saves the context on the stack and restores it)
    res = ft81x_image_manager_send_paletted8_image_draw_dl(handle,
                                                           &cat_paletted8_raw_image_handle,
                                                           162, 270);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
        return res;
    }

    // draw some points, lines and rects
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(test_dl_snippet), test_dl_snippet);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_snippet failed with %u\n", res);
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

    return FT81X_RESULT_OK;
}
