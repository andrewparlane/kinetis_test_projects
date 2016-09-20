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

// forward declarations for ft81x.h
enum _ft81x_result;
typedef enum _ft81x_result ft81x_result;
struct _FT81X_Handle;
typedef struct _FT81X_Handle FT81X_Handle;

// don't call in user code, auto called in ft81x.c
ft81x_result ft81x_image_manager_initialise(FT81X_Handle *handle);

ft81x_result ft81x_image_manager_load_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle);

#endif
