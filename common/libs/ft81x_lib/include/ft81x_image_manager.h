#ifndef __FT81X_IMAGE_MANAGER_H
#define __FT81X_IMAGE_MANAGER_H

#include "ft81x_display_list.h"

// the FT81x supports bitmap IDs 0-14
#define FT81X_NUM_VALID_BITMAP_IDS      15
#define FT81X_VALID_BITMAP_IDS_MASK     0x7FFF

typedef struct
{
    uint16_t free_bitmap_ids;
} FT81X_Image_Manager_Data;

typedef struct
{
    FT81X_Bitmap_Format     format;

    uint32_t                width;
    uint32_t                height;
    uint32_t                linestride;

    const uint8_t          *data;
    uint32_t                size;       // needed? or linestride * height

    const uint8_t          *lut_data;   // only for format = one of FT81X_BITMAP_FORMAT_PALETTED...
    uint32_t                lut_size;
} FT81X_Image_Properties;

typedef struct
{
    uint8_t     bitmap_handle;      // pass to VERTEX2II()
    uint32_t    load_offset;        // offset of image in g_ram
    uint32_t    lut_load_offset;    // offset of LUT in g_ram (if format is paletted)
} FT81X_Image_Handle;

// ----------------------------------------------------------------------------
// Helper macros
// ----------------------------------------------------------------------------
#define FT81X_IMAGE_MANAGER_DL_INIT_IMAGE(image_properties, image_handle, filter, wrapx, wrapy)                         \
    FT81X_DL_CMD_BITMAP_HANDLE((image_handle).bitmap_handle),                                                           \
    FT81X_DL_CMD_BITMAP_LAYOUT((image_properties).format, (image_properties).linestride, (image_properties).height),    \
    FT81X_DL_CMD_BITMAP_SIZE((filter), (wrapx), (wrapy), (image_properties).width, (image_properties).height),          \
    FT81X_DL_CMD_BITMAP_SOURCE((image_handle).load_offset)


#define FT81X_IMAGE_MANAGER_DL_DRAW_NON_PALETTED_IMAGE(image_handle, x, y)  \
    FT81X_DL_CMD_BEGIN(FT81X_PRIMITIVE_BITMAP),                             \
        FT81X_DL_CMD_VERTEX2II((x), (y), (image_handle).bitmap_handle, 0),  \
    FT81X_DL_CMD_END()

#define FT81X_IMAGE_MANAGER_DL_DRAW_PALETTED_IMAGE(image_handle, x, y)                              \
    FT81X_DL_CMD_BEGIN((FT81X_PRIMITIVE_BITMAP)),                                                   \
        /* save the context so we can restore it afterwards */                                      \
        FT81X_DL_CMD_SAVE_CONTEXT(),                                                                \
                                                                                                    \
        /* alpha */                                                                                 \
        FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_ONE, FT81X_BLEND_FUNC_ZERO),                       \
        FT81X_DL_CMD_COLOUR_MASK(0, 0, 0, 1),                                                       \
        FT81X_DL_CMD_PALETTE_SOURCE((image_handle).lut_load_offset+3),                              \
        FT81X_DL_CMD_VERTEX2II((x), (y), (image_handle).bitmap_handle, 0),                          \
                                                                                                    \
        /* colours */                                                                               \
        FT81X_DL_CMD_BLEND_FUNC(FT81X_BLEND_FUNC_DST_ALPHA, FT81X_BLEND_FUNC_ONE_MINUS_DST_ALPHA),  \
        /* red */                                                                                   \
        FT81X_DL_CMD_COLOUR_MASK(1, 0, 0, 0),                                                       \
        FT81X_DL_CMD_PALETTE_SOURCE((image_handle).lut_load_offset+2),                              \
        FT81X_DL_CMD_VERTEX2II((x), (y), (image_handle).bitmap_handle, 0),                          \
        /* green */                                                                                 \
        FT81X_DL_CMD_COLOUR_MASK(0, 1, 0, 0),                                                       \
        FT81X_DL_CMD_PALETTE_SOURCE((image_handle).lut_load_offset+1),                              \
        FT81X_DL_CMD_VERTEX2II((x), (y), (image_handle).bitmap_handle, 0),                          \
        /* blue */                                                                                  \
        FT81X_DL_CMD_COLOUR_MASK(0, 0, 1, 0),                                                       \
        FT81X_DL_CMD_PALETTE_SOURCE((image_handle).lut_load_offset+0),                              \
        FT81X_DL_CMD_VERTEX2II((x), (y), (image_handle).bitmap_handle, 0),                          \
                                                                                                    \
        /* revert the changes to colour mask and blend func */                                      \
        FT81X_DL_CMD_RESTORE_CONTEXT(),                                                             \
    FT81X_DL_CMD_END()

// forward declarations for ft81x.h
enum _ft81x_result;
typedef enum _ft81x_result ft81x_result;
struct _FT81X_Handle;
typedef struct _FT81X_Handle FT81X_Handle;

// don't call in user code, auto called in ft81x.c
ft81x_result ft81x_image_manager_initialise(FT81X_Handle *handle);

ft81x_result ft81x_image_manager_load_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle);

#endif
