#ifndef __FT81X_H
#define __FT81X_H

#include "ft81x/g_ram_manager.h"
#include "ft81x/bitmap_handles.h"
#include "ft81x_graphics_engine.h"

#include <stdint.h>

// Common warning support
// compiler_warning.h
#define STRINGISE_IMPL(x) #x
#define STRINGISE(x) STRINGISE_IMPL(x)

// Use: #pragma message WARN("My message")
#if _MSC_VER
#   define FILE_LINE_LINK __FILE__ "(" STRINGISE(__LINE__) ") : "
#   define WARN(exp) (FILE_LINE_LINK "WARNING: " exp)
#else // __GNUC__ - may need other defines for different compilers
#   define WARN(exp) ("WARNING: " exp)
#endif

// common packing support
#if _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else // __GNUC__ - may need other defines for different compilers
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

// GPU chip types
#define FT81X_GPU_TYPE_FT810 0
#define FT81X_GPU_TYPE_FT811 1
#define FT81X_GPU_TYPE_FT812 2
#define FT81X_GPU_TYPE_FT813 3

// See the FT81x datasheet Table 4-12.
#define FT81X_COLOUR_FORMAT_RGB             0   // correct colour on each pin, MSb on MSb pin
#define FT81X_COLOUR_FORMAT_RGB_REVERSED    1   // correct colour on each pin, MSb on LSb pin
#define FT81X_COLOUR_FORMAT_BGR             2   // blue on red, and red on blue, green as normal, MSb on MSb pin
#define FT81X_COLOUR_FORMAT_BGR_REVERSED    3   // ... MSb on LSb pin
#define FT81X_COLOUR_FORMAT_BRG             8   // MSb on MSb pin
#define FT81X_COLOUR_FORMAT_BRG_REVERSED    9   // MSb on LSb pin
#define FT81X_COLOUR_FORMAT_GRB             10  // MSb on MSb pin
#define FT81X_COLOUR_FORMAT_GRB_REVERSED    11  // MSb on LSb pin
#define FT81X_COLOUR_FORMAT_GBR             12  // MSb on MSb pin
#define FT81X_COLOUR_FORMAT_GBR_REVERSED    13  // MSb on LSb pin
#define FT81X_COLOUR_FORMAT_RBG             14  // MSb on MSb pin
#define FT81X_COLOUR_FORMAT_RBG_REVERSED    15  // MSb on LSb pin

typedef enum _ft81x_result
{
    FT81X_RESULT_OK = 0,
    FT81X_RESULT_NO_HANDLE,
    FT81X_RESULT_ALREADY_INITIALISED,
    FT81X_RESULT_OUT_OF_MEMORY,
    FT81X_RESULT_OUT_OF_G_RAM,
    FT81X_RESULT_OUT_OF_DL_RAM,
    FT81X_RESULT_GPU_TFER_FAILED,
    FT81X_RESULT_INVALID_ID,
    FT81X_RESULT_INVALID_ARG,
    FT81X_RESULT_OUT_OF_BITMAP_IDS,
    FT81X_RESULT_NOT_SUPPORTED,
    FT81X_RESULT_OVERFLOW,
    FT81X_RESULT_CALIBRATION_FAILED,
} ft81x_result;

typedef struct _FT81X_Handle
{
    void                       *platform_user_data;
    FT81X_G_RAM_Manager_Data    g_ram_manager_data;
    FT81X_Bitmap_Handles        bitmap_handles;
    FT81X_Graphics_Engine_Data  graphics_engine_data;
} FT81X_Handle;

typedef enum
{
    FT81X_BACKLIGHT_LEVEL_OFF   = 0,

    FT81X_BACKLIGHT_LEVEL_MAX   = 128
} ft81x_backlight_level;


// Buffer is used to combine multiple display list commands / coproc commands
// So we only need one SPI transaction to send it to the GPU rather than
// lots of little ones. It can be whatever size you want including 0.
// If we run out of spcae then it gets flushed to the GPU and we start
// filling it again. DL ram is 8KB on the FT81x so there's no point in
// being bigger than that.
ft81x_result ft81x_initialise(FT81X_Handle *handle, uint32_t buffer_size, uint8_t *buffer);
void ft81x_cleanup(FT81X_Handle *handle);
ft81x_result ft81x_configure(FT81X_Handle *handle);

ft81x_result ft81x_reset(FT81X_Handle *handle);
ft81x_result ft81x_set_active(FT81X_Handle *handle);

ft81x_result ft81x_backlight(FT81X_Handle *handle, ft81x_backlight_level level);

ft81x_result ft81x_dump_registers(FT81X_Handle *handle);

#endif
