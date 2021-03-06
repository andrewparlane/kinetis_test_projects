#include "ft81x.h"
#include "ft81x_image_manager.h"
#include "ft81x_co_processor.h"
#include "ft81x/g_ram_manager.h"
#include "ft81x/bitmap_handles.h"
#include "ft81x/platforms/platform.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------
static ft81x_result load_raw_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // allocate space in g_ram, storing the offset in the image_handle
    res = ft81x_g_ram_manager_allocate(handle, image_properties->size, &(image_handle->allocation_data));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the image to g_ram
    return ft81x_g_ram_manager_write(handle, &image_handle->allocation_data, 0, image_properties->size, image_properties->data);
}

static ft81x_result load_raw_image_lut(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle)
{
    ft81x_result res;

    // allocate space in g_ram, storing the offset in the image_handle
    res = ft81x_g_ram_manager_allocate(handle, image_properties->lut_size, &(image_handle->lut_allocation_data));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write the LUT to g_ram
    return ft81x_g_ram_manager_write(handle, &image_handle->lut_allocation_data, 0, image_properties->lut_size, image_properties->lut_data);
}

#ifdef FT81X_DEBUG

static ft81x_result check_for_overflow(FT81X_Handle *handle, uint32_t start_addr, uint32_t expected_decompress_size)
{
    ft81x_result res;

    // check that we haven't overflowed the allocated space
    // shouldn't happen unless linestride, height or format are wrong

    // first we need to wait until the data has been decompressed
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

    uint32_t actual_decompress_size = end_addr - start_addr;

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
    // define FT81X_DEBUG in your config to enable checking
    // for overflows. This will slow down the execution
    // but will detect if your image decompressed to larger
    // than it should.
    uint32_t decompress_size = image_properties->linestride * image_properties->height;
    res = ft81x_g_ram_manager_allocate(handle, decompress_size, &(image_handle->allocation_data));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // get the addr we've been allocated, to pass to the co-proc
    uint32_t allocated_addr;
    res = ft81x_g_ram_manager_get_addr(handle, &(image_handle->allocation_data), &allocated_addr);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the inflate command and the data
    res = ft81x_coproc_cmd_inflate(handle, allocated_addr, image_properties->size, image_properties->data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

#ifdef FT81X_DEBUG
    res = check_for_overflow(handle, allocated_addr, decompress_size);
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

    // define FT81X_DEBUG in your config to enable checking
    // for overflows. This will slow down the execution
    // but will detect if your image decompressed to larger
    // than it should.
    res = ft81x_g_ram_manager_allocate(handle, decompress_size, &(image_handle->lut_allocation_data));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // get the addr we've been allocated, to pass to the co-proc
    uint32_t allocated_addr;
    res = ft81x_g_ram_manager_get_addr(handle, &(image_handle->lut_allocation_data), &allocated_addr);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the inflate command and the data
    res = ft81x_coproc_cmd_inflate(handle, allocated_addr, image_properties->lut_size, image_properties->lut_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

#ifdef FT81X_DEBUG
    res = check_for_overflow(handle, allocated_addr, decompress_size);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }
#endif

    return FT81X_RESULT_OK;
}

static ft81x_result setup_draw_transforms(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, const FT81X_Image_Transform *transform)
{
    ft81x_result res;

    const FT81X_Image_Properties *ip = image_handle->image_properties;

    double new_width = ip->width;
    double new_height = ip->height;

    // load the identity matrix
    res = ft81x_coproc_cmd_loadidentity(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    if (transform &&
        (transform->scale))
    {
        // apply the scale
        res = ft81x_coproc_cmd_scale_double(handle,
                                            transform->scale_x,
                                            transform->scale_y);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }
        new_width *= transform->scale_x;
        new_height *= transform->scale_y;
    }

    // use this matrix for drawing
    res = ft81x_coproc_cmd_setmatrix(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // set up the bitmap size, so we see all of it
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_BITMAP_SIZE(filter, FT81X_BITMAP_WRAP_BORDER, FT81X_BITMAP_WRAP_BORDER, (uint32_t)new_width, (uint32_t)new_height));
}

static ft81x_result cleanup_draw_transforms(FT81X_Handle *handle)
{
    ft81x_result res;

    // load the identity matrix
    res = ft81x_coproc_cmd_loadidentity(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // use this matrix for drawing
    return ft81x_coproc_cmd_setmatrix(handle);
}

static ft81x_result send_non_paletted8_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, uint32_t x, uint32_t y, uint8_t use_macro_0)
{
    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_BITMAP),
            (use_macro_0 ? FT81X_DL_CMD_MACRO(0) :
                           FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0)),
        FT81X_DL_CMD_END()
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}

static ft81x_result send_paletted8_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, uint32_t x, uint32_t y, uint8_t use_macro_0)
{
    ft81x_result res;

    // get the addr we've been allocated, to use in the DL
    uint32_t lut_allocated_addr;
    res = ft81x_g_ram_manager_get_addr(handle, &(image_handle->lut_allocation_data), &lut_allocated_addr);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BEGIN((FT81X_PRIMITIVE_BITMAP)),
            /* save the context so we can restore it afterwards */
            FT81X_DL_CMD_SAVE_CONTEXT(),

            /* alpha */
            FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_ONE, FT81X_BLEND_FUNC_ZERO),
            FT81X_DL_CMD_COLOUR_MASK(0, 0, 0, 1),
            FT81X_DL_CMD_PALETTE_SOURCE(lut_allocated_addr + 3),
            (use_macro_0 ? FT81X_DL_CMD_MACRO(0) :
                           FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0)),

            /* colours */
            FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_DST_ALPHA, FT81X_BLEND_FUNC_ONE_MINUS_DST_ALPHA),
            /* red */
            FT81X_DL_CMD_COLOUR_MASK(1, 0, 0, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(lut_allocated_addr + 2),
            (use_macro_0 ? FT81X_DL_CMD_MACRO(0) :
                           FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0)),
            /* green */
            FT81X_DL_CMD_COLOUR_MASK(0, 1, 0, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(lut_allocated_addr + 1),
            (use_macro_0 ? FT81X_DL_CMD_MACRO(0) :
                           FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0)),
            /* blue */
            FT81X_DL_CMD_COLOUR_MASK(0, 0, 1, 0),
            FT81X_DL_CMD_PALETTE_SOURCE(lut_allocated_addr + 0),
            (use_macro_0 ? FT81X_DL_CMD_MACRO(0) :
                           FT81X_DL_CMD_VERTEX2II(x, y, image_handle->bitmap_handle, 0)),

            /* revert the changes to colour mask and blend func */
            FT81X_DL_CMD_RESTORE_CONTEXT(),
        FT81X_DL_CMD_END()
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}

// ----------------------------------------------------------------------------
// API functions
// ----------------------------------------------------------------------------
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
#pragma message WARN("TODO: free the indices data")
            return res;
        }
    }

    // store a pointer to the properties in the handle
    image_handle->image_properties = image_properties;

    // assign a free bitmap ID
    res = ft81x_bitmap_handles_allocate(handle, &(image_handle->bitmap_handle));
    if (res != FT81X_RESULT_OK)
    {
        // failed, free the image data (LUT + indices)
#pragma message WARN("TODO: free the indices data + LUT data")
        return res;
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// Display list functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_image_manager_send_image_init_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle)
{
    ft81x_result res;
    const FT81X_Image_Properties *ip = image_handle->image_properties;

    // get the addr we've been allocated, to use in the DL
    uint32_t allocated_addr;
    res = ft81x_g_ram_manager_get_addr(handle, &(image_handle->allocation_data), &allocated_addr);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    const uint32_t dl[] =
    {
        FT81X_DL_CMD_BITMAP_HANDLE(image_handle->bitmap_handle),
        FT81X_DL_CMD_BITMAP_LAYOUT(ip->format, ip->linestride, ip->height),
        FT81X_DL_CMD_BITMAP_SOURCE(allocated_addr)
    };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(dl), dl);
}

ft81x_result ft81x_image_manager_send_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, uint32_t x, uint32_t y, const FT81X_Image_Transform *transform)
{
    // first we set up the matrix for any transforms
    ft81x_result res = setup_draw_transforms(handle, image_handle, filter, transform);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // set the image as active
    // this doesn't seem to be needed by the actual hardware
    // however it is needed by the windows emulator
    // doesn't hurt much to put it in here
    res = ft81x_image_manager_set_active_bitmap(handle, image_handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then draw the image
    if (image_handle->image_properties->format != FT81X_BITMAP_FORMAT_PALETTED8)
    {
        res = send_non_paletted8_image_draw_dl(handle, image_handle, x, y, 0);
    }
    else
    {
        res = send_paletted8_image_draw_dl(handle, image_handle, x, y, 0);
    }

    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // finally cleanup the trantransforms (load the identity matrix)
    // only needed if we transformed the image
    if (transform &&
        (transform->scale))
    {
        return cleanup_draw_transforms(handle);
    }
    else
    {
        return FT81X_RESULT_OK;
    }
}

ft81x_result ft81x_image_manager_send_image_draw_dl_with_macro_0(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, const FT81X_Image_Transform *transform)
{
    // first we set up the matrix for any transforms
    ft81x_result res = setup_draw_transforms(handle, image_handle, filter, transform);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // set the image as active
    // this doesn't seem to be needed by the actual hardware
    // however it is needed by the windows emulator
    // doesn't hurt much to put it in here
    res = ft81x_image_manager_set_active_bitmap(handle, image_handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then draw the image
    if (image_handle->image_properties->format != FT81X_BITMAP_FORMAT_PALETTED8)
    {
        res = send_non_paletted8_image_draw_dl(handle, image_handle, 0, 0, 1);
    }
    else
    {
        res = send_paletted8_image_draw_dl(handle, image_handle, 0, 0, 1);
    }

    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // finally cleanup the trantransforms (load the identity matrix)
    // only needed if we transformed the image
    if (transform &&
        (transform->scale))
    {
        return cleanup_draw_transforms(handle);
    }
    else
    {
        return FT81X_RESULT_OK;
    }
}

ft81x_result ft81x_image_manager_set_active_bitmap(FT81X_Handle *handle, FT81X_Image_Handle *image_handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_BITMAP_HANDLE(image_handle->bitmap_handle));
}
