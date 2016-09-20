#include "ft81x.h"
#include "ft81x_image_manager.h"
#include "ft81x/g_ram_manager.h"

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

    // write the image to g_ram, storing the offset in the image_handle
    res = ft81x_g_ram_manager_allocate_and_write(handle, image_properties->size, image_properties->data, &(image_handle->load_offset));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    if ((image_properties->format == FT81X_BITMAP_FORMAT_PALETTED565  ||
         image_properties->format == FT81X_BITMAP_FORMAT_PALETTED4444 ||
         image_properties->format == FT81X_BITMAP_FORMAT_PALETTED8) &&
         image_properties->lut_data)
    {
        // write the LUT to g_ram, storing the offset in the image_handle
        res = ft81x_g_ram_manager_allocate_and_write(handle, image_properties->lut_size, image_properties->lut_data, &(image_handle->lut_load_offset));
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
