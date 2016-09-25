#include "ft81x.h"
#include "ft81x_image_manager.h"
#include "ft81x_co_processor.h"
#include "ft81x/g_ram_manager.h"
#include "ft81x/platforms/platform.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------
static ft81x_result allocate_bitmap_id(FT81X_Handle *handle, uint8_t *id)
{
    uint16_t ids = handle->image_manager_data.free_bitmap_ids;

    // check each bit until we find a 1.
    // abort if ids == 0, ie. there are none free
    for (uint8_t i = 0; ids != 0; i++)
    {
        if (ids & 0x01)
        {
            *id = i;
            break;
        }

        ids >>= 1;
    }

    // did we manage to allocate one?
    if (ids == 0)
    {
        return FT81X_RESULT_OUT_OF_BITMAP_IDS;
    }

    // clear that bit, as it's not free any more
    handle->image_manager_data.free_bitmap_ids &= ~(1 << *id);

    return FT81X_RESULT_OK;
}

static ft81x_result load_raw_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // allocate space in g_ram, storing the offset in the image_handle
    res = ft81x_g_ram_manager_allocate(handle, image_properties->size, &(image_handle->load_offset));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the image to g_ram
    return ft81x_g_ram_manager_write(handle, image_handle->load_offset, image_properties->size, image_properties->data);
}

static ft81x_result load_raw_image_lut(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // allocate space in g_ram, storing the offset in the image_handle
    res = ft81x_g_ram_manager_allocate(handle, image_properties->lut_size, &(image_handle->lut_load_offset));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the LUT to g_ram
    return ft81x_g_ram_manager_write(handle, image_handle->lut_load_offset, image_properties->lut_size, image_properties->lut_data);
}

#ifdef FT81X_DEBUG

static ft81x_result check_for_overflow(FT81X_Handle *handle, uint32_t load_offset, uint32_t expected_decompress_size)
{
    ft81x_result res;

    // check that we haven't overflowed the allocated space
    // shouldn't happen unless linestride, height or format are wrong

    // first we need to wait until the data has been decompressed
#warning TODO: do we have to wait here?
    res = ft81x_graphics_engine_flush_and_synchronise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // we now use CMD_GETPTR which once written to the command bufer
    // has the 4 bytes after the command replaced with the end address
    // of the image in g_ram (next address after the end of the data).
    // We can then read that out of the command buffer.

    // Get the address to read from. The value in REG_CMD_WRITE
    // is where the next write to the cmd_buffer will go.
    // our end address will be after that
    uint16_t end_address_location;
    READ_GPU_REG_16(FT81X_REG_CMD_WRITE, end_address_location);
    end_address_location += 4;

    // then the cmd
    res = ft81x_coproc_cmd_getptr(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // wait until it's done
    res = ft81x_graphics_engine_flush_and_synchronise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // finally we read the result out of the command buffer
    uint32_t end_addr;
    READ_GPU_REG_32(FT81X_COPROC_CMD_RAM + end_address_location, end_addr);

    uint32_t actual_decompress_size = end_addr - load_offset;

    if (actual_decompress_size > expected_decompress_size)
    {
        return FT81X_RESULT_OVERFLOW;
    }

    return FT81X_RESULT_OK;
}

#endif

static ft81x_result load_compressed_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // The image will decompress to linestride*height
    // unless linestride or height have been misconfigured
    // TODO how do we deal with detecting that without
    //      blocking for ages (see the #ifdef FT81X_DEBUG below)
    //      maybe we could use CMD_INTERRUPT and then in the
    //      handler we can read the end address
#warning TODO: Set up the interrupt handler to check for g_ram overflow
    uint32_t decompress_size = image_properties->linestride * image_properties->height;
    res = ft81x_g_ram_manager_allocate(handle, decompress_size, &(image_handle->load_offset));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the inflate command and the data
    res = ft81x_coproc_cmd_inflate(handle, image_handle->load_offset, image_properties->size, image_properties->data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

#ifdef FT81X_DEBUG
    res = check_for_overflow(handle, image_handle->load_offset, decompress_size);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }
#endif

    return FT81X_RESULT_OK;
}

static ft81x_result load_compressed_image_lut(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // The LUT will decompress to less than or equal to
    // the number of indices * size of each indice

    // if (image_properties->linestride == image_properties->width)
    // then each byte is one inex ie. 256 bytes. For now we don't support
    // more indices than that. (Pretty sure the FT81X only supports 256)
    if (image_properties->linestride != image_properties->width)
    {
        return FT81X_RESULT_NOT_SUPPORTED;
    }
    const uint32_t NUM_INDICES = 256;

    // The FT81X support 3 paletted types:
    //  565     - 16 bits per index
    //  4444    - 16 bits per index
    //  8       - 32 bits per index
    uint32_t decompress_size;
    switch (image_properties->format)
    {
        case FT81X_BITMAP_FORMAT_PALETTED565:   decompress_size = NUM_INDICES * 2;  break;
        case FT81X_BITMAP_FORMAT_PALETTED4444:  decompress_size = NUM_INDICES * 2;  break;
        case FT81X_BITMAP_FORMAT_PALETTED8:     decompress_size = NUM_INDICES * 4;  break;
        default:                                return FT81X_RESULT_NOT_SUPPORTED;
    }

    // TODO how do we deal with detecting overflow without
    //      blocking for ages (see the #ifdef FT81X_DEBUG below)
    //      maybe we could use CMD_INTERRUPT and then in the
    //      handler we can read the end address
#warning TODO: Set up the interrupt handler to check for g_ram overflow
    res = ft81x_g_ram_manager_allocate(handle, decompress_size, &(image_handle->lut_load_offset));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the inflate command and the data
    res = ft81x_coproc_cmd_inflate(handle, image_handle->lut_load_offset, image_properties->lut_size, image_properties->lut_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

#ifdef FT81X_DEBUG
    res = check_for_overflow(handle, image_handle->lut_load_offset, decompress_size);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }
#endif

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// API functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_image_manager_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    handle->image_manager_data.free_bitmap_ids = FT81X_VALID_BITMAP_IDS_MASK;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_image_manager_load_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // load the image / indices
    if (image_properties->is_compressed)
    {
        res = load_compressed_image(handle, image_properties, image_handle);
    }
    else
    {
        res = load_raw_image(handle, image_properties, image_handle);
    }

    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    if ((image_properties->format == FT81X_BITMAP_FORMAT_PALETTED565  ||
         image_properties->format == FT81X_BITMAP_FORMAT_PALETTED4444 ||
         image_properties->format == FT81X_BITMAP_FORMAT_PALETTED8) &&
         image_properties->lut_data)
    {
        // we have a LUT too
        if (image_properties->is_lut_compressed)
        {
            res = load_compressed_image_lut(handle, image_properties, image_handle);
        }
        else
        {
            res = load_raw_image_lut(handle, image_properties, image_handle);
        }

        if (res != FT81X_RESULT_OK)
        {
            // failed, free the indices data
#warning TODO: free the indices data
            return res;
        }
    }

    // assign a free bitmap ID
    res = allocate_bitmap_id(handle, &(image_handle->bitmap_handle));
    if (res != FT81X_RESULT_OK)
    {
        // failed, free the image data (LUT + indices)
#warning TODO: free the indices data + LUT data
        return res;
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// Display list functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_image_manager_send_image_init_dl(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, FT81X_Bitmap_Wrap wrapx, FT81X_Bitmap_Wrap wrapy)
{
    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BITMAP_HANDLE(image_handle->bitmap_handle),
        FT81X_DL_CMD_BITMAP_LAYOUT(image_properties->format, image_properties->linestride, image_properties->height),
        FT81X_DL_CMD_BITMAP_SIZE(filter, wrapx, wrapy, image_properties->width, image_properties->height),
        FT81X_DL_CMD_BITMAP_SOURCE(image_handle->load_offset)
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}

ft81x_result ft81x_image_manager_send_non_paletted8_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, uint32_t x, uint32_t y)
{
    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_BITMAP),
            FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0),
        FT81X_DL_CMD_END()
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}

ft81x_result ft81x_image_manager_send_paletted8_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, uint32_t x, uint32_t y)
{
    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BEGIN((FT81X_PRIMITIVE_BITMAP)),
            /* save the context so we can restore it afterwards */
            FT81X_DL_CMD_SAVE_CONTEXT(),

            /* alpha */
            FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_ONE, FT81X_BLEND_FUNC_ZERO),
            FT81X_DL_CMD_COLOUR_MASK(0, 0, 0, 1),
            FT81X_DL_CMD_PALETTE_SOURCE(image_handle->lut_load_offset+3),
            FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0),

            /* colours */
            FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_DST_ALPHA, FT81X_BLEND_FUNC_ONE_MINUS_DST_ALPHA),
            /* red */
            FT81X_DL_CMD_COLOUR_MASK(1, 0, 0, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(image_handle->lut_load_offset+2),
            FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0),
            /* green */
            FT81X_DL_CMD_COLOUR_MASK(0, 1, 0, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(image_handle->lut_load_offset+1),
            FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0),
            /* blue */
            FT81X_DL_CMD_COLOUR_MASK(0, 0, 1, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(image_handle->lut_load_offset+0),
            FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0),

            /* revert the changes to colour mask and blend func */
            FT81X_DL_CMD_RESTORE_CONTEXT(),
        FT81X_DL_CMD_END()
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}
