#include "ft81x_config.h"

#include "ft81x.h"
#include "ft81x_co_processor.h"
#include "ft81x_graphics_engine.h"

#include <string.h>

// ----------------------------------------------------------------------------
// CMD IDs
// ----------------------------------------------------------------------------
#define FT81X_COPROC_CMD_ID_DLSTART         0xFFFFFF00
#define FT81X_COPROC_CMD_ID_SWAP            0xFFFFFF01
#define FT81X_COPROC_CMD_ID_TEXT            0xFFFFFF0C
#define FT81X_COPROC_CMD_ID_INFLATE         0xFFFFFF22
#define FT81X_COPROC_CMD_ID_GETPTR          0xFFFFFF23
#define FT81X_COPROC_CMD_ID_LOGO            0xFFFFFF31

// ----------------------------------------------------------------------------
// CMD functions
// ----------------------------------------------------------------------------

inline ft81x_result ft81x_coproc_cmd_dlstart(FT81X_Handle *handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_COPROC_CMD_ID_DLSTART);
}

inline ft81x_result ft81x_coproc_cmd_swap(FT81X_Handle *handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_COPROC_CMD_ID_SWAP);
}

ft81x_result ft81x_coproc_cmd_text(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t font_id, uint16_t options, char *str)
{
    // first the command and the parameters
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_TEXT,
                              (y << 16) | x,
                              (options << 16) | font_id };

    ft81x_result res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then the string
    // (strlen + 1 for the NULL char
    uint32_t len = strlen(str) + 1;

    // this needs to be padded to 4 bytes.
    // so I send all blocks of 4 and then
    // finally any remaining bytes padded out
    // hence the len & ~3
    res = ft81x_graphics_engine_write_display_list_snippet(handle, len & ~3, (uint32_t *)str);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // anything remaining?
    if (len % 4)
    {
        // copy remaining data (1,2 or 3 bytes)
        uint8_t remaining_data[4];
        memcpy(remaining_data, &str[len & ~3], len % 4);

        // doesn't matter on the pad values, so just send it
        return ft81x_graphics_engine_write_display_list_snippet(handle, 4, (uint32_t *)remaining_data);
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_coproc_cmd_inflate(FT81X_Handle *handle, uint32_t offset, uint32_t count, const uint8_t *compressed_data)
{
    // first the command and the g_ram ptr
    const uint32_t cmd[] = { FT81X_COPROC_CMD_ID_INFLATE,
                             offset };

    ft81x_result res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(cmd), cmd);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then the compressed data
    // this needs to be padded to 4 bytes.
    // so I send all blocks of 4 and then finally any remaining bytes padded out
    res = ft81x_graphics_engine_write_display_list_snippet(handle, (count & ~3), (uint32_t *)compressed_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // anything remaining?
    if (count % 4)
    {
        // copy remaining data (1,2 or 3 bytes)
        uint8_t remaining_data[4];
        memcpy(remaining_data, &compressed_data[count & ~3], count % 4);

        // doesn't matter on the pad values, so just send it
        return ft81x_graphics_engine_write_display_list_snippet(handle, 4, (uint32_t *)remaining_data);
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_coproc_cmd_getptr(FT81X_Handle *handle)
{
    // first the command and a padding byte
    const uint32_t cmd[] = { FT81X_COPROC_CMD_ID_GETPTR,
                             0 };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(cmd), cmd);
}

inline ft81x_result ft81x_coproc_cmd_logo(FT81X_Handle *handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_COPROC_CMD_ID_LOGO);
}
