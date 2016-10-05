#ifndef __BITMAP_HANDLES_H
#define __BITMAP_HANDLES_H

#include <stdint.h>

// the FT81x supports bitmap IDs 0-14
#define FT81X_NUM_VALID_BITMAP_IDS      15
#define FT81X_VALID_BITMAP_IDS_MASK     0x7FFF

typedef struct
{
    uint16_t free_bitmap_ids;
} FT81X_Bitmap_Handles;

// forward declarations for ft81x.h
enum _ft81x_result;
typedef enum _ft81x_result ft81x_result;
struct _FT81X_Handle;
typedef struct _FT81X_Handle FT81X_Handle;

ft81x_result ft81x_bitmap_handles_initialise(FT81X_Handle *handle);
ft81x_result ft81x_bitmap_handles_allocate(FT81X_Handle *handle, uint8_t *id);

#endif
