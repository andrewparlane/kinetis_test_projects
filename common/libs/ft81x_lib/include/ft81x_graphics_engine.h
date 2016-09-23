#ifndef FT81X_GRAPHICS_ENGINE_H
#define FT81X_GRAPHICS_ENGINE_H

#include "ft81x_config.h"

typedef struct
{
    // buffer to store DL / coproc cmds can be NULL.
    uint8_t                    *buffer;
    uint32_t                    buffer_size;
    uint32_t                    buffer_write_idx;
#ifdef FT81X_DL_WRITE_DIRECTLY_TO_DL_RAM
    uint32_t                    dl_ram_write_idx;
#endif
} FT81X_Graphics_Engine_Data;

// Don't call initialise directly from user code
// ft81x.c calls it automatically
ft81x_result ft81x_graphics_engine_initialise(FT81X_Handle *handle, uint32_t buffer_size, uint8_t *buffer);
ft81x_result ft81x_graphics_engine_cleanup(FT81X_Handle *handle);

ft81x_result ft81x_graphics_engine_start_display_list(FT81X_Handle *handle);
ft81x_result ft81x_graphics_engine_write_display_list_cmd(FT81X_Handle *handle, uint32_t cmd);
ft81x_result ft81x_graphics_engine_write_display_list_snippet(FT81X_Handle *handle, uint32_t bytes, const uint32_t *dl);
ft81x_result ft81x_graphics_engine_end_display_list(FT81X_Handle *handle);

#endif
