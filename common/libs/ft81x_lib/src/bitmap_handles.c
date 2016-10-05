#include "ft81x/bitmap_handles.h"

#include "ft81x.h"

#include <stdlib.h>

ft81x_result ft81x_bitmap_handles_initialise(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    handle->bitmap_handles.free_bitmap_ids = FT81X_VALID_BITMAP_IDS_MASK;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_bitmap_handles_allocate(FT81X_Handle *handle, uint8_t *id)
{
    uint16_t ids = handle->bitmap_handles.free_bitmap_ids;

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
    handle->bitmap_handles.free_bitmap_ids &= ~(1 << *id);

    return FT81X_RESULT_OK;
}
