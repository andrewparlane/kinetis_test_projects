#ifndef __FT81X_H
#define __FT81X_H

#include <stdint.h>

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

typedef enum
{
    FT81X_RESULT_OK = 0,
    FT81X_RESULT_NO_HANDLE,
    FT81X_RESULT_AREADY_INITIALISED,
    FT81X_RESULT_OUT_OF_MEMORY,
    FT81X_RESULT_GPU_TFER_FAILED,
    FT81X_RESULT_INVALID_ID,
    FT81X_RESULT_INVALID_ARG,
} ft81x_result;

typedef struct
{
    void *platform_user_data;
} FT81X_Handle;

ft81x_result ft81x_initialise(FT81X_Handle *handle);
void ft81x_cleanup(FT81X_Handle *handle);
ft81x_result ft81x_configure(FT81X_Handle *handle);

ft81x_result ft81x_reset(FT81X_Handle *handle);
ft81x_result ft81x_set_active(FT81X_Handle *handle);

#endif
