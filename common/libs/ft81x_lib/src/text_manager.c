#include "ft81x.h"
#include "ft81x_text_manager.h"

#include <stdlib.h>

// there are 128 chars supported by FT81X
// Ascii 0 -> 127
#define NUM_CHARS               128
#define FIRST_CHAR              ' '

PACK(
typedef struct
{
    // uint8_t widths[NUM_CHARS]    // no point copying this much data, just here for reference
    uint32_t format;                // should be enum FT81X_Bitmap_Format, L1, L2, L4 or L8
    uint32_t linestride;
    uint32_t width;
    uint32_t height;
    uint32_t image_offset;          // offset into G_RAM of the bitmap data
}) Font_Metric_Block;

// + NUM_CHARS for the widths array
#define SIZEOF_FONT_METRIC_BLOCK    (NUM_CHARS + sizeof(Font_Metric_Block))

ft81x_result ft81x_text_manager_load_custom_font(FT81X_Handle *handle, const FT81X_Font_Properties *font_properties, FT81X_Font_Handle *font_handle)
{
    ft81x_result res;

    const FT81X_Image_Properties *ip = font_properties->image_properties;

    // check the format is allowed
    FT81X_Bitmap_Format format = ip->format;
    if (format != FT81X_BITMAP_FORMAT_L1 &&
        format != FT81X_BITMAP_FORMAT_L2 &&
        format != FT81X_BITMAP_FORMAT_L4 &&
        format != FT81X_BITMAP_FORMAT_L8)
    {
        return FT81X_RESULT_NOT_SUPPORTED;
    }

    // check the width data is the correct size
    if (font_properties->character_widths_size != NUM_CHARS)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // load the image into g_ram
    res = ft81x_image_manager_load_image(handle, ip, &font_handle->image_handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // we're a custom font
    font_handle->custom = 1;

    // build the metric block
    Font_Metric_Block fmb =
    {
        .format         = ip->format,
        .linestride     = ip->linestride,
        .width          = ip->width,
        .height         = ip->height,
    };

    // get the address of the image in g_ram
    res = ft81x_g_ram_manager_get_addr(handle, &font_handle->image_handle.allocation_data, &fmb.image_offset);
    if (res != FT81X_RESULT_OK)
    {
        // free the image data
        return res;
    }

    // allocate space in g_ram, storing the offset in the font_handle
    res = ft81x_g_ram_manager_allocate(handle, SIZEOF_FONT_METRIC_BLOCK, &(font_handle->metric_block_allocation_data));
    if (res != FT81X_RESULT_OK)
    {
        // free the image data
#pragma message WARN("TODO Free the image data")
        return res;
    }

    // write the character widths array
    res = ft81x_g_ram_manager_write(handle, &font_handle->metric_block_allocation_data, 0, NUM_CHARS, font_properties->character_widths);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the metric block
    return ft81x_g_ram_manager_write(handle, &font_handle->metric_block_allocation_data, NUM_CHARS, sizeof(Font_Metric_Block), (uint8_t *)&fmb);
}

ft81x_result ft81x_text_manager_get_font_handle_for_inbuilt_font(FT81X_Handle *handle, FT81X_Font_Handle *font_handle, uint8_t font_id)
{
    // Make sure we actual request an inbuilt font
    if (font_id < FT81X_TEXT_MANAGER_FIRST_INBUILT_FONT ||
        font_id > FT81X_TEXT_MANAGER_LAST_INBUILT_FONT)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // allocate a bitmap handle
    ft81x_result res = ft81x_bitmap_handles_allocate(handle, &font_handle->image_handle.bitmap_handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // this isn't a custom font
    font_handle->custom = 0;

    // Now use CMD_ROMFONT to assign this font to this bitmap_handle
    return ft81x_coproc_cmd_romfont(handle, font_handle->image_handle.bitmap_handle, font_id);
}

ft81x_result ft81x_text_manager_send_font_init_dl(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, FT81X_Bitmap_Filter filter, FT81X_Bitmap_Wrap wrapx, FT81X_Bitmap_Wrap wrapy)
{
    ft81x_result res;

    if (font_handle->custom)
    {
        const FT81X_Image_Properties *ip = font_handle->image_handle.image_properties;

        // first send the image init display list stuff
        res = ft81x_image_manager_send_image_init_dl(handle, &font_handle->image_handle);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }

        // the image manager only sends the BITMAP_SIZE field when we draw it.
        // so we have to send this here
        res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_BITMAP_SIZE(filter, wrapx, wrapy, ip->width, ip->height));
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }

        // get the addr we've been allocated, to pass to the co-proc
        uint32_t metric_allocated_addr;
        res = ft81x_g_ram_manager_get_addr(handle, &(font_handle->metric_block_allocation_data), &metric_allocated_addr);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }

        // then we use the setfont2 co-proc command to set this image up as a font
        return ft81x_coproc_cmd_setfont2(handle, font_handle->image_handle.bitmap_handle, metric_allocated_addr, FIRST_CHAR);
    }
    else
    {
        // inbuilt font, nothing to do
        return FT81X_RESULT_OK;
    }
}

ft81x_result ft81x_text_manager_write_text(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, const char *str)
{
    return ft81x_coproc_cmd_text(handle,
                                 x, y,
                                 font_handle->image_handle.bitmap_handle,
                                 (uint16_t)options,
                                 str);
}

ft81x_result ft81x_text_manager_write_unsigned_number(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, uint32_t num)
{
    return ft81x_coproc_cmd_number(handle,
                                   x, y,
                                   font_handle->image_handle.bitmap_handle,
                                   (uint16_t)options,
                                   num);
}

ft81x_result ft81x_text_manager_write_signed_number(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, int32_t num)
{
    return ft81x_coproc_cmd_number(handle,
                                   x, y,
                                   font_handle->image_handle.bitmap_handle,
                                   ((uint16_t)options) | FT81X_COPROC_OPTION_SIGNED,
                                   num);
}
