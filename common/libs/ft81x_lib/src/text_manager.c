#include "ft81x.h"
#include "ft81x_text_manager.h"

#include <stdlib.h>

ft81x_result ft81x_text_manager_write_fixed_width_font_string(FT81X_Handle *handle, uint8_t font_id, uint8_t font_width, uint8_t font_height, char *str, uint32_t x, uint32_t y)
{
    ft81x_result res;

    // first we start drawing a bitmap
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_BITMAP));
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    uint32_t x_offset = x;
    for (int i = 0; str[i]; i++)
    {
        // handle special cases
        if (str[i] == '\n')
        {
            y = y + font_height;
            x_offset = x;
            continue;
        }
        else if (str[i] < 32 || str[i] >= 127)
        {
            // non displayable char
            continue;
        }

        // show the charater
        uint32_t cmd = FT81X_DL_CMD_VERTEX2II(x_offset, y, font_id, str[i]);
        res = ft81x_graphics_engine_write_display_list_cmd(handle, cmd);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }

        // update x_offset for the next char
        x_offset += font_width;
    }

    // finall end this set of primitives
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_END());
}
