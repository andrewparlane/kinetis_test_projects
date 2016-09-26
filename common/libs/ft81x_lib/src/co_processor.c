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
#define FT81X_COPROC_CMD_ID_BGCOLOUR        0xFFFFFF09
#define FT81X_COPROC_CMD_ID_FGCOLOUR        0xFFFFFF0A
#define FT81X_COPROC_CMD_ID_GRADIENT        0xFFFFFF0B
#define FT81X_COPROC_CMD_ID_TEXT            0xFFFFFF0C
#define FT81X_COPROC_CMD_ID_BUTTON          0xFFFFFF0D
#define FT81X_COPROC_CMD_ID_KEYS            0xFFFFFF0E
#define FT81X_COPROC_CMD_ID_PROGRESS        0xFFFFFF0F
#define FT81X_COPROC_CMD_ID_SLIDER          0xFFFFFF10
#define FT81X_COPROC_CMD_ID_SCROLLBAR       0xFFFFFF11
#define FT81X_COPROC_CMD_ID_TOGGLE          0xFFFFFF12
#define FT81X_COPROC_CMD_ID_GAUGE           0xFFFFFF13
#define FT81X_COPROC_CMD_ID_CLOCK           0xFFFFFF14
#define FT81X_COPROC_CMD_ID_INFLATE         0xFFFFFF22
#define FT81X_COPROC_CMD_ID_GETPTR          0xFFFFFF23
#define FT81X_COPROC_CMD_ID_DIAL            0xFFFFFF2D
#define FT81X_COPROC_CMD_ID_NUMBER          0xFFFFFF2E
#define FT81X_COPROC_CMD_ID_LOGO            0xFFFFFF31
#define FT81X_COPROC_CMD_ID_COLDSTART       0xFFFFFF32
#define FT81X_COPROC_CMD_ID_GRADCOLOUR      0xFFFFFF34

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

ft81x_result ft81x_coproc_cmd_bgcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_BGCOLOUR,
                              (r << 16) | (g << 8) | b };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_fgcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_FGCOLOUR,
                              (r << 16) | (g << 8) | b };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_gradient(FT81X_Handle *handle, uint16_t x0, uint16_t y0, uint8_t r0, uint8_t g0, uint8_t b0, uint16_t x1, uint16_t y1, uint8_t r1, uint8_t g1, uint8_t b1)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_GRADIENT,
                              (y0 << 16) | x0,
                              (r0 << 16) | (g0 << 8) | b0,
                              (y1 << 16) | x1,
                              (r1 << 16) | (g1 << 8) | b1 };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
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
    return ft81x_graphics_engine_write_display_list_data(handle, len, (uint8_t *)str);
}

ft81x_result ft81x_coproc_cmd_button(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font_id, uint16_t options, char *str)
{
    // first the command and the parameters
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_BUTTON,
                              (y << 16) | x,
                              (height << 16) | width,
                              (options << 16) | font_id };

    ft81x_result res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then the string
    // (strlen + 1 for the NULL char
    uint32_t len = strlen(str) + 1;
    return ft81x_graphics_engine_write_display_list_data(handle, len, (uint8_t *)str);
}

ft81x_result ft81x_coproc_cmd_keys(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font_id, uint16_t options, char *str)
{
    // first the command and the parameters
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_KEYS,
                              (y << 16) | x,
                              (height << 16) | width,
                              (options << 16) | font_id };

    ft81x_result res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then the string
    // (strlen + 1 for the NULL char
    uint32_t len = strlen(str) + 1;
    return ft81x_graphics_engine_write_display_list_data(handle, len, (uint8_t *)str);
}

ft81x_result ft81x_coproc_cmd_progress(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t range)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_PROGRESS,
                              (y << 16) | x,
                              (height << 16) | width,
                              (val << 16) | options,
                              range };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_slider(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t range)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_SLIDER,
                              (y << 16) | x,
                              (height << 16) | width,
                              (val << 16) | options,
                              range };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_scrollbar(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_SCROLLBAR,
                              (y << 16) | x,
                              (height << 16) | width,
                              (val << 16) | options,
                              (range << 16) | size};

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_toggle(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint8_t font_id, uint16_t options, uint16_t state, char *str)
{
    // state 0 is off, 65535 is on
    if (state)
    {
        state = 65535;
    }

    // first the command and the parameters
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_TOGGLE,
                              (y << 16) | x,
                              (font_id << 16) | width,
                              (state << 16) | options };

    ft81x_result res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // then the string
    // (strlen + 1 for the NULL char
    uint32_t len = strlen(str) + 1;
    return ft81x_graphics_engine_write_display_list_data(handle, len, (uint8_t *)str);
}


ft81x_result ft81x_coproc_cmd_gauge(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_GAUGE,
                              (y << 16) | x,
                              (options << 16) | radius,
                              (minor << 16) | major,
                              (range << 16) | val };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_clock(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t hours, uint16_t minutes, uint16_t seconds, uint16_t milliseconds)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_CLOCK,
                              (y << 16) | x,
                              (options << 16) | radius,
                              (minutes << 16) | hours,
                              (milliseconds << 16) | seconds };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
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
    return ft81x_graphics_engine_write_display_list_data(handle, count, compressed_data);
}

ft81x_result ft81x_coproc_cmd_getptr(FT81X_Handle *handle)
{
    // first the command and a padding byte
    const uint32_t cmd[] = { FT81X_COPROC_CMD_ID_GETPTR,
                             0 };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(cmd), cmd);
}

ft81x_result ft81x_coproc_cmd_dial(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t radius, uint16_t options, uint16_t val)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_DIAL,
                              (y << 16) | x,
                              (options << 16) | radius,
                              val };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

ft81x_result ft81x_coproc_cmd_number(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t font_id, uint16_t options, int32_t num)
{
    // first the command and the parameters
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_NUMBER,
                              (y << 16) | x,
                              (options << 16) | font_id,
                              num };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}

inline ft81x_result ft81x_coproc_cmd_logo(FT81X_Handle *handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_COPROC_CMD_ID_LOGO);
}

inline ft81x_result ft81x_coproc_cmd_coldstart(FT81X_Handle *handle)
{
    return ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_COPROC_CMD_ID_COLDSTART);
}

ft81x_result ft81x_coproc_cmd_gradcolour(FT81X_Handle *handle, uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t data[] = { FT81X_COPROC_CMD_ID_GRADCOLOUR,
                              (r << 16) | (g << 8) | b };

    return ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(data), data);
}
