#include "ft81x.h"
#include "ft81x_graphics_engine.h"
#include "ft81x/platforms/platform.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
static ft81x_result flush_display_list_buffer(FT81X_Handle *handle)
{
    if (handle->graphics_engine_data.buffer_size == 0    ||
        handle->graphics_engine_data.buffer == NULL      ||
        handle->graphics_engine_data.buffer_write_idx == 0)
    {
        // nothing to do
    }
    else
    {
        // check we won't overflow DL ram
        if ((handle->graphics_engine_data.dl_ram_write_idx + handle->graphics_engine_data.buffer_write_idx) > FT81X_DISPLAY_LIST_RAM_SIZE)
        {
            handle->graphics_engine_data.dl_ram_write_idx = 0;
            handle->graphics_engine_data.buffer_write_idx = 0;
            return FT81X_RESULT_OUT_OF_DL_RAM;
        }

        // write the buffer into DL ram
        // and update our indexes
        WRITE_GPU_MEM(FT81X_DISPLAY_LIST_RAM + handle->graphics_engine_data.dl_ram_write_idx, handle->graphics_engine_data.buffer_write_idx, handle->graphics_engine_data.buffer);
        handle->graphics_engine_data.dl_ram_write_idx += handle->graphics_engine_data.buffer_write_idx;
        handle->graphics_engine_data.buffer_write_idx = 0;
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// FT81X Grahpics engine functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_graphics_engine_initialise(FT81X_Handle *handle, uint32_t buffer_size, uint8_t *buffer)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    handle->graphics_engine_data.buffer              = buffer;
    handle->graphics_engine_data.buffer_size         = buffer_size;
    handle->graphics_engine_data.buffer_write_idx    = 0;
    handle->graphics_engine_data.dl_ram_write_idx    = 0;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_cleanup(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    handle->graphics_engine_data.buffer              = NULL;
    handle->graphics_engine_data.buffer_size         = 0;
    handle->graphics_engine_data.buffer_write_idx    = 0;
    handle->graphics_engine_data.dl_ram_write_idx    = 0;

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_write_display_list_cmd(FT81X_Handle *handle, uint32_t cmd)
{
    return ft81x_graphics_engine_write_display_list_snippet(handle, 4, &cmd);
}

ft81x_result ft81x_graphics_engine_write_display_list_snippet(FT81X_Handle *handle, uint32_t bytes, const uint32_t *dl)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    // special case, if the buffer is 0 bytes or NULL
    // then write directly to DL ram
    if (handle->graphics_engine_data.buffer_size == 0 ||
        handle->graphics_engine_data.buffer == NULL)
    {
        // check it's not bigger than DL ram
        if (bytes > FT81X_DISPLAY_LIST_RAM_SIZE)
        {
            handle->graphics_engine_data.dl_ram_write_idx = 0;
            return FT81X_RESULT_OUT_OF_DL_RAM;
        }
        WRITE_GPU_MEM(FT81X_DISPLAY_LIST_RAM + handle->graphics_engine_data.dl_ram_write_idx, bytes, (uint8_t *)dl);
        handle->graphics_engine_data.dl_ram_write_idx += bytes;
    }
    else
    {
        // write this dl to our buffer
        // if the buffer fills up, then flush the buffer
        // and continue writting
        uint32_t written = 0;
        while (written != bytes)
        {
            // how much space is left in our buffer?
            uint32_t space = handle->graphics_engine_data.buffer_size - handle->graphics_engine_data.buffer_write_idx;
            if (space == 0)
            {
                // flush the buffer
                ft81x_result res = flush_display_list_buffer(handle);
                if (res != FT81X_RESULT_OK)
                {
                    // abort
                    handle->graphics_engine_data.buffer_write_idx = 0;
                    handle->graphics_engine_data.dl_ram_write_idx = 0;
                    return res;
                }

                // recalculate space
                // can't be 0 since buffer_size != 0, and buffer_write_idx == 0
                space = handle->graphics_engine_data.buffer_size - handle->graphics_engine_data.buffer_write_idx;
            }

            // copy up to space bytes into the buffer
            uint32_t copy_count = (bytes - written);
            if (copy_count > space)
            {
                copy_count = space;
            }
            memcpy(&(handle->graphics_engine_data.buffer[handle->graphics_engine_data.buffer_write_idx]), &((uint8_t *)dl)[written], copy_count);
            handle->graphics_engine_data.buffer_write_idx += copy_count;
            written += copy_count;
        }
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_send_display_list_to_coproc(FT81X_Handle *handle, uint32_t bytes, const uint32_t *dl)
{
    uint32_t dlstart = 0xFFFFFF00;
    uint32_t dlswap = 0xFFFFFF01;

#warning TODO: Use handle->graphics_engine_data.buffer like for DL ram

    uint32_t wp;
    READ_GPU_REG_32(FT81X_REG_CMD_WRITE, wp);

    WRITE_GPU_MEM(FT81X_COPROC_CMD_RAM + wp, 4, (uint8_t *)&dlstart);
    WRITE_GPU_MEM(FT81X_COPROC_CMD_RAM + wp + 4, bytes, (uint8_t *)dl);
    WRITE_GPU_MEM(FT81X_COPROC_CMD_RAM + wp + bytes + 4, 4, (uint8_t *)&dlswap);

    WRITE_GPU_REG_32(FT81X_REG_CMD_WRITE, (wp + bytes + 8) % 4096);

    uint32_t rp;
    do
    {
        READ_GPU_REG_32(FT81X_REG_CMD_READ, rp);
    } while(rp != (wp + bytes + 8) % 4096);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_end_display_list(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    // flush anything in the buffer
    ft81x_result res = flush_display_list_buffer(handle);

    // set the DL offset back to 0 for the next DL
    handle->graphics_engine_data.dl_ram_write_idx = 0;

    // swap the buffers
    WRITE_GPU_REG_8(FT81X_REG_DLSWAP, FT81X_REG_DLSWAP_SWAP_FRAME);

    return res;
}
