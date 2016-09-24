#include "ft81x.h"
#include "ft81x_graphics_engine.h"
#include "ft81x_co_processor.h"
#include "ft81x/platforms/platform.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
static ft81x_result write_to_command_buffer(FT81X_Handle *handle, uint32_t count, const uint8_t *data)
{
    uint32_t written = 0;

    while (written < count)
    {
        // wait until we have enough space or at least 1KB
        // there's no point writing 4 bytes at a time.
        uint32_t to_write = count - written;
        if (to_write > 1024*1024)
        {
            to_write = 1024*1024;
        }

        uint16_t space;
        do
        {
            READ_GPU_REG_16(FT81X_REG_CMDB_SPACE, space);
        } while (space < to_write);

        // write (either total buffer, or 1KB)
        // into the command RAM using (FT81X_REG_CMDB_WRITE)
        WRITE_GPU_MEM(FT81X_REG_CMDB_WRITE, to_write, &data[written]);

        // update the counts
        written += to_write;
    }

    return FT81X_RESULT_OK;
}

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
        ft81x_result res = write_to_command_buffer(handle, handle->graphics_engine_data.buffer_write_idx, handle->graphics_engine_data.buffer);
        handle->graphics_engine_data.buffer_write_idx = 0;
        return res;
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

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_start_display_list(FT81X_Handle *handle)
{
    // sending display list through the command buffer
    // requires each to start with the DLSTART command
    return ft81x_coproc_cmd_dlstart(handle);
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
    // then write directly to the command buffer
    if (handle->graphics_engine_data.buffer_size == 0 ||
        handle->graphics_engine_data.buffer == NULL)
    {
        ft81x_result res = write_to_command_buffer(handle, bytes, (uint8_t *)dl);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }
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
            if (space < 4)  // all writes have to be multiples of 4
            {
                // flush the buffer
                ft81x_result res = flush_display_list_buffer(handle);
                if (res != FT81X_RESULT_OK)
                {
                    // abort
                    handle->graphics_engine_data.buffer_write_idx = 0;
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
            // can only copy in 4 byte chunks
            copy_count &= ~0x03;
            memcpy(&(handle->graphics_engine_data.buffer[handle->graphics_engine_data.buffer_write_idx]), &((uint8_t *)dl)[written], copy_count);
            handle->graphics_engine_data.buffer_write_idx += copy_count;
            written += copy_count;
        }
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_graphics_engine_end_display_list(FT81X_Handle *handle)
{
    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

    ft81x_result res;

    // sending display list through the command buffer
    // requires each to end with the swap command
    res = ft81x_coproc_cmd_swap(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // flush anything in the buffer
    res = flush_display_list_buffer(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // sending display list through the command buffer
    // requires each to start with the DLSTART command
    // add it here because then it makes calling
    // ft81x_graphics_engine_start_display_list() optional
    // other than the first time (in ft81x.c)
    return ft81x_coproc_cmd_dlstart(handle);
}

ft81x_result ft81x_graphics_engine_flush_and_synchronise(FT81X_Handle *handle)
{
    // flush anything in the buffer
    ft81x_result res = flush_display_list_buffer(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // wait for read ptr to equal write ptr
    while (1)
    {
        uint16_t wp, rp;

        READ_GPU_REG_16(FT81X_REG_CMD_WRITE, wp);
        READ_GPU_REG_16(FT81X_REG_CMD_READ, rp);

        if (wp == rp)
        {
            break;
        }
    }

    return FT81X_RESULT_OK;
}
