#include "ft81x/boards/board.h" // defines the display type
#include "ft81x/platforms/platform.h"
#include "ft81x/ft81x_types.h"

// only compile this file if we have the ILI9488 display defined
#if FT81X_DISPLAY == FT81X_DISPLAY_ILI9488

ft81x_result ft81x_display_initialise(void *platform_user_data)
{
    ft81x_result res;

    // This display uses a DCX pin as part of it's SPI comms
    res = ft81x_platform_initialise_gpio_pin(platform_user_data, FT81X_BOARD_DISPLAY_DCX_PIN_PORT, FT81X_BOARD_DISPLAY_DCX_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // We also have a disp pin that determines if the display is on or off
    res = ft81x_platform_initialise_gpio_pin(platform_user_data, FT81X_BOARD_DISPLAY_DISP_PIN_PORT, FT81X_BOARD_DISPLAY_DISP_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_display_comms_initialise(void *platform_user_data)
{
    // this display needs some set up so we need to initialise the comms
    return ft81x_board_display_comms_initialise(platform_user_data);
}

#endif
