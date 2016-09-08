#ifndef __FT81X_H
#define __FT81X_H

#include "ft81x/ft81x_types.h"

#include <stdint.h>

typedef struct
{
    void *platform_user_data;
} FT81X_Handle;

ft81x_result ft81x_initialise(FT81X_Handle *handle);
ft81x_result ft81x_configure(FT81X_Handle *handle);

ft81x_result ft81x_reset(FT81X_Handle *handle);
ft81x_result ft81x_set_active(FT81X_Handle *handle);

#endif
