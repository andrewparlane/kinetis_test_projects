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

    // ----------------------------------------------------------
    // Build and display the display list
    // ----------------------------------------------------------
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

    // change the colour for the folowwing text
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_COLOUR_RGB(255, 0, 0));
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_graphics_engine_write_display_list_cmd failed with %u\n", res);
        return res;
    }

    // show some text
    res = ft81x_coproc_cmd_text(handle, 10, 30, 31, 0, "Test 2");
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_coproc_cmd_text failed with %u\n", res);
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
