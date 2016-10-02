#include "ft81x.h"
#include "ft81x_text_manager.h"

#include <stdlib.h>

// there are 128 chars supported by FT81X
// Ascii 0 -> 127
#define NUM_CHARS               128
#define FIRST_CHAR              ' '

typedef struct
{
    // uint8_t widths[NUM_CHARS]    // no point copying this much data, just here for reference
    uint32_t format;                // should be enum FT81X_Bitmap_Format, L1, L2, L4 or L8
    uint32_t linestride;
    uint32_t width;
    uint32_t height;
    uint32_t image_offset;          // offset into G_RAM of the bitmap data
} __attribute__((packed)) Font_Metric_Block;

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

    // copy the bitmap_handle to fontfont_id
    font_handle->font_id = font_handle->image_handle.bitmap_handle;

    // we're a custom font
    font_handle->custom = 1;

    // build the metric block
    Font_Metric_Block fmb =
    {
        .format         = ip->format,
        .linestride     = ip->linestride,
        .width          = ip->width,
        .height         = ip->height,
        .image_offset   = font_handle->image_handle.load_offset,
    };

    // allocate space in g_ram, storing the offset in the font_handle
    res = ft81x_g_ram_manager_allocate(handle, SIZEOF_FONT_METRIC_BLOCK, &(font_handle->metric_block_offset));
    if (res != FT81X_RESULT_OK)
    {
        // free the image data
#warning TODO Free the image data
        return res;
    }

    // write the character widths array
    res = ft81x_g_ram_manager_write(handle, font_handle->metric_block_offset, NUM_CHARS, font_properties->character_widths);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the metric block
    return ft81x_g_ram_manager_write(handle, font_handle->metric_block_offset + NUM_CHARS, sizeof(Font_Metric_Block), (uint8_t *)&fmb);
}

ft81x_result ft81x_text_manager_get_font_handle_for_inbuilt_font(FT81X_Handle *handle, FT81X_Font_Handle *font_handle, uint8_t font_id)
{
    font_handle->font_id = font_id;
    font_handle->custom = 0;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_text_manager_send_font_init_dl(FT81X_Handle *handle, const FT81X_Font_Properties *font_properties, const FT81X_Font_Handle *font_handle, FT81X_Bitmap_Filter filter, FT81X_Bitmap_Wrap wrapx, FT81X_Bitmap_Wrap wrapy)
{
    ft81x_result res;

    if (font_handle->custom)
    {
        // first send the image init display list stuff
        res = ft81x_image_manager_send_image_init_dl(handle, font_properties->image_properties, &font_handle->image_handle, filter, wrapx, wrapy);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }

        // then we use the setfont2 co-proc command to set this image up as a font
        return ft81x_coproc_cmd_setfont2(handle, font_handle->font_id, font_handle->metric_block_offset, FIRST_CHAR);
    }
    else
    {
        // inbuilt font, nothing to do
        return FT81X_RESULT_OK;
    }
}

inline ft81x_result ft81x_text_manager_write_text(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, const char *str)
{
    return ft81x_coproc_cmd_text(handle,
                                 x, y,
                                 font_handle->font_id,
                                 (uint16_t)options,
                                 str);
}

inline ft81x_result ft81x_text_manager_write_unsigned_number(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, uint32_t num)
{
    return ft81x_coproc_cmd_number(handle,
                                   x, y,
                                   font_handle->font_id,
                                   (uint16_t)options,
                                   num);
}

inline ft81x_result ft81x_text_manager_write_signed_number(FT81X_Handle *handle, const FT81X_Font_Handle *font_handle, uint16_t x, uint16_t y, FT81X_Text_Manager_Options options, int32_t num)
{
    return ft81x_coproc_cmd_number(handle,
                                   x, y,
                                   font_handle->font_id,
                                   ((uint16_t)options) | FT81X_COPROC_OPTION_SIGNED,
                                   num);
}
