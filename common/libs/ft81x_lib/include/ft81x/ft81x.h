#ifndef __FT81X_H
#define __FT81X_H

#include "ft81x/ft81x_types.h"

#include <stdint.h>

ft81x_result ft81x_initialise(void *platform_user_data);
ft81x_result ft81x_configure(void *platform_user_data);

ft81x_result ft81x_reset(void *platform_user_data);
ft81x_result ft81x_set_active(void *platform_user_data);

#endif
