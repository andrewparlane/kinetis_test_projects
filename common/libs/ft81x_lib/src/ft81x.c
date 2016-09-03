#include "ft81x/ft81x.h"
#include "ft81x/commands.h"
#include "ft81x/memory_map.h"

#include "ft81x/boards/board.h"
#include "ft81x/displays/display.h"
#include "ft81x/platforms/platform.h"

ft81x_result ft81x_initialise(void *platform_user_data)
{
    ft81x_result res;

    // generic board initialise
    res = ft81x_board_initialise(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // generic platform initialise
    res = ft81x_platform_initialise(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // generic display initialise
    res = ft81x_display_initialise(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // initialise comms to the GPU
    res = ft81x_board_gpu_comms_initialise(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // initialise comms to the display
    res = ft81x_display_comms_initialise(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // reset the GPU
    res = ft81x_reset(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the active command to the GPU to wake it up
    res = ft81x_set_active(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // read the ID register
    uint32_t id;
    res = ft81x_read_gpu_register(platform_user_data, FT81X_REG_ID_ADDR, &id);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // confirm the ID register is as expected
    if (id != FT81X_REG_ID_EXPECTED)
    {
        return FT81X_RESULT_INVALID_ID;
    }

    // configure the GPU
#warning TODO

    // configure the display
    res = ft81x_display_send_configuration(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_reset(void *platform_user_data)
{
    ft81x_result res;

    // First assert power down
    res = ft81x_platform_set_power_down_pin(platform_user_data, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait a bit
    res = ft81x_platform_delay(platform_user_data, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Then take out of reset using the PD pin
    res = ft81x_platform_set_power_down_pin(platform_user_data, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait a bit
    res = ft81x_platform_delay(platform_user_data, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return res;
}

ft81x_result ft81x_set_active(void *platform_user_data)
{
    ft81x_result res;

    // Send the ACTIVE command to wake it up
    res = ft81x_platform_gpu_send_command(platform_user_data, FT81X_COMMAND_ACTIVE, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // wait a bit more for it to wake up
    res = ft81x_platform_delay(platform_user_data, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_read_gpu_register(void *platform_user_data, uint32_t addr, uint32_t *val)
{
    return ft81x_platform_gpu_read_register_32(platform_user_data, addr, val);
}
