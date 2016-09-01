#ifndef __PLATFORM_H
#define __PLATFORM_H

#include "ft81x/platforms/platform_types.h"

// the board file defines the platform
#include "ft81x/boards/board.h"

#ifndef FT81X_PLATFORM
#  error FT81X_PLATFORM is not defined
#elif ((FT81X_PLATFORM) == (FT81X_PLATFORM_NXP_KINETIS_K66))
#  include "ft81x/platforms/nxp_kinetis_k66.h"
#else
#  error FT81X_PLATFORM is not a defined platform
#endif

#include "ft81x/ft81x_types.h"
#include "ft81x/commands.h"

#include <stdint.h>

typedef enum
{
    FT81X_PLATFORM_GPIO_DIRECTION_INPUT,
    FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT
} ft81x_gpio_direction;

// functions
ft81x_result ft81x_platform_initialise(void *platform_user_data);
ft81x_result ft81x_platform_gpu_spi_comms_initialise(void *platform_user_data);
ft81x_result ft81x_platform_display_spi_comms_initialise(void *platform_user_data);

ft81x_result ft81x_platform_gpu_send_command(void *platform_user_data, ft81x_command command, uint8_t param);
ft81x_result ft81x_platform_gpu_read_register(void *platform_user_data, uint32_t address, uint32_t *value);

ft81x_result ft81x_platform_set_power_down_pin(void *platform_user_data, uint8_t power_down);

// note: These shouldn't be used externally for the PD pin
//       as that is managed internally
ft81x_result ft81x_platform_initialise_gpio_pin(void *platform_user_data, void *port, uint32_t pin, ft81x_gpio_direction direction, uint8_t value);
ft81x_result ft81x_platform_set_gpio_pin(void *platform_user_data, void *port, uint32_t pin, uint8_t value);

ft81x_result ft81x_platform_delay(void *platform_user_data, uint32_t milliseconds);

#endif