#ifndef __FT81X_IMAGE_MANAGER_H
#define __FT81X_IMAGE_MANAGER_H

#include "ft81x.h"
#include "ft81x_display_list.h"

typedef struct
{
    FT81X_Bitmap_Format     format;

    uint32_t                width;
    uint32_t                height;
    uint32_t                linestride;

    uint8_t                 is_compressed;
    const uint8_t          *data;
    uint32_t                size;

    // only for format = one of FT81X_BITMAP_FORMAT_PALETTED...
    uint8_t                 is_lut_compressed;
    const uint8_t          *lut_data;
    uint32_t                lut_size;
} FT81X_Image_Properties;

typedef struct
{
    const FT81X_Image_Properties       *image_properties;
    uint8_t                             bitmap_handle;          // pass to VERTEX2II()
    FT81X_G_RAM_Manager_Allocation_Data allocation_data;        // offset of image in g_ram
    FT81X_G_RAM_Manager_Allocation_Data lut_allocation_data;    // offset of LUT in g_ram (if format is paletted)
} FT81X_Image_Handle;

typedef struct
{
    uint8_t scale;          // scale this image?
    double scale_x;         // how much x?
    double scale_y;         // how much y?
} FT81X_Image_Transform;

ft81x_result ft81x_image_manager_load_image(FT81X_Handle *handle, const FT81X_Image_Properties *image_properties, FT81X_Image_Handle *image_handle);

ft81x_result ft81x_image_manager_send_image_init_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle);
ft81x_result ft81x_image_manager_send_image_draw_dl(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, uint32_t x, uint32_t y, const FT81X_Image_Transform *transform);
ft81x_result ft81x_image_manager_send_image_draw_dl_with_macro_0(FT81X_Handle *handle, const FT81X_Image_Handle *image_handle, FT81X_Bitmap_Filter filter, const FT81X_Image_Transform *transform);
ft81x_result ft81x_image_manager_set_active_bitmap(FT81X_Handle *handle, FT81X_Image_Handle *image_handle);

#endif
