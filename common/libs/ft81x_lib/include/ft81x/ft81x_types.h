#ifndef __FT81X_TYPES_H
#define __FT81X_TYPES_H

typedef enum
{
    FT81X_RESULT_OK = 0,
    FT81X_RESULT_NO_USER_DATA,
    FT81X_RESULT_GPU_TFER_FAILED,
    FT81X_RESULT_INVALID_ID,
    FT81X_RESULT_INVALID_ARG,
} ft81x_result;

enum
{
    FT81X_GPU_TYPE_FT810 = 0,
    FT81X_GPU_TYPE_FT811,
    FT81X_GPU_TYPE_FT812,
    FT81X_GPU_TYPE_FT813,
};

// See the FT81x datasheet Table 4-12.
enum
{
    FT81X_COLOUR_FORMAT_RGB = 0,        // correct colour on each pin, MSb on MSb pin
    FT81X_COLOUR_FORMAT_RGB_REVERSED,   // correct colour on each pin, MSb on LSb pin
    FT81X_COLOUR_FORMAT_BGR,            // blue on red, and red on blue, green as normal, MSb on MSb pin
    FT81X_COLOUR_FORMAT_BGR_REVERSED,   // ... MSb on LSb pin
    FT81X_COLOUR_FORMAT_BRG = 8,        // MSb on MSb pin
    FT81X_COLOUR_FORMAT_BRG_REVERSED,   // MSb on LSb pin
    FT81X_COLOUR_FORMAT_GRB,            // MSb on MSb pin
    FT81X_COLOUR_FORMAT_GRB_REVERSED,   // MSb on LSb pin
    FT81X_COLOUR_FORMAT_GBR,            // MSb on MSb pin
    FT81X_COLOUR_FORMAT_GBR_REVERSED,   // MSb on LSb pin
    FT81X_COLOUR_FORMAT_RBG,            // MSb on MSb pin
    FT81X_COLOUR_FORMAT_RBG_REVERSED,   // MSb on LSb pin
};

#endif
