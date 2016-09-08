#include "ft81x.h"
#include "ft81x/commands.h"
#include "ft81x/memory_map.h"

#include "ft81x/boards/board.h"
#include "ft81x/displays/display.h"
#include "ft81x/platforms/platform.h"

// ----------------------------------------------------------------------------
// Helper macros
// ----------------------------------------------------------------------------
#define WRITE_GPU_MEM(addr, count, data)                                                    \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_write_mem(handle, addr, count, data); \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_8(addr, data)                                                         \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_write_register_8(handle, addr, data); \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_16(addr, data)                                                            \
{                                                                                               \
    ft81x_result res = ft81x_platform_gpu_write_register_16(handle, addr, data);    \
    if (res != FT81X_RESULT_OK)                                                                 \
    {                                                                                           \
        return res;                                                                             \
    }                                                                                           \
}

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
ft81x_result configure_gpu(FT81X_Handle *handle)
{
    ft81x_result res;

    // reset the GPU
    res = ft81x_reset(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // send the active command to the GPU to wake it up
    res = ft81x_set_active(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Use the external clock
#warning TODO use external clock
    /*res = ft81x_platform_gpu_send_command(handle, FT81X_COMMAND_CLKEXT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait 10mS
    res = ft81x_platform_delay(handle, 10);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // switching clk causes a system reset, so send the ACTIVE command again
    res = ft81x_set_active(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }*/

    // read the ID register to confirm the GPU is ready
    uint8_t read_id_ok = 0;
    for (int i = 0; i < 5; i++)
    {
        uint8_t id;
        res = ft81x_platform_gpu_read_register_8(handle, FT81X_REG_ID_ADDR, &id);
        if (res == FT81X_RESULT_OK)
        {
            // confirm the ID register is as expected
            if (id == FT81X_REG_ID_EXPECTED)
            {
                read_id_ok = 1;
                break;
            }
        }

        // some error, wait a bit and try again
        // Wait 100mS
        res = ft81x_platform_delay(handle, 100);
        if (res != FT81X_RESULT_OK)
        {
            return res;
        }
    }
    if (!read_id_ok)
    {
        return FT81X_RESULT_INVALID_ID;
    }

#warning TODO Clock trimming?

    // Horizontal parameters
    // total pixels per line (visible + not visible)
    WRITE_GPU_REG_16(FT81X_REG_HCYCLE, (FT81X_SCREEN_TOTAL_WIDTH));
    // horizontal resolution
    WRITE_GPU_REG_16(FT81X_REG_HSIZE, (FT81X_SCREEN_WIDTH));
    // non visible section
    WRITE_GPU_REG_16(FT81X_REG_HOFFSET, (FT81X_SCREEN_HORIZ_NON_VISIBLE));
    // front porch
    WRITE_GPU_REG_16(FT81X_REG_HSYNC0, (FT81X_SCREEN_HORIZ_FRONT_PORCH));
    // front porch + hsync pulse
    WRITE_GPU_REG_16(FT81X_REG_HSYNC1, (FT81X_SCREEN_HORIZ_FRONT_PORCH) + (FT81X_SCREEN_HSYNC));

    // vertical parameters
    // total lines per frame (visible + not visible)
    WRITE_GPU_REG_16(FT81X_REG_VCYCLE, (FT81X_SCREEN_TOTAL_HEIGHT));
    // vertical resolution
    WRITE_GPU_REG_16(FT81X_REG_VSIZE, (FT81X_SCREEN_HEIGHT));
    // non visible section
    WRITE_GPU_REG_16(FT81X_REG_VOFFSET, (FT81X_SCREEN_VERT_NON_VISIBLE));
    // front porch
    WRITE_GPU_REG_16(FT81X_REG_VSYNC0, (FT81X_SCREEN_VERT_FRONT_PORCH));
    // front porch + hsync pulse
    WRITE_GPU_REG_16(FT81X_REG_VSYNC1, (FT81X_SCREEN_VERT_FRONT_PORCH) + (FT81X_SCREEN_VSYNC));

    // misc parameters
    WRITE_GPU_REG_8(FT81X_REG_SWIZZLE, FT81X_SCREEN_SWIZZLE);
    WRITE_GPU_REG_8(FT81X_REG_PCLK_POL, FT81X_SCREEN_PCLK_POL);

#if (FT81X_RESISTIVE_TOUCH)
    // Set the touch sensitivity for resistive touch screens
    WRITE_GPU_REG_16(FT81X_REG_TOUCH_RZTHRESH, (FT81X_RESISTIVE_TOUCH_THRESHOLD));
#endif

    WRITE_GPU_REG_8(FT81X_REG_GPIO_DIR, 0xff);
    WRITE_GPU_REG_8(FT81X_REG_GPIO, 0xff);

    // set the clear colour to green, and clear the display
    uint8_t init_dl[12] =
    {
        0,255,0,2,    //GPU instruction CLEAR_COLOR_RGB
        7,0,0,38,   //GPU instruction CLEAR
        0,0,0,0,    //GPU instruction DISPLAY
    };
    WRITE_GPU_MEM(FT81X_DISPLAY_LIST_RAM, sizeof(init_dl), init_dl);
    WRITE_GPU_REG_8(FT81X_REG_DLSWAP, FT81X_REG_DLSWAP_SWAP_FRAME);

    // after this display is visible on the LCD
    WRITE_GPU_REG_8(FT81X_REG_PCLK, FT81X_SCREEN_PCLK_DIVISOR);

    // Wait 120mS
    res = ft81x_platform_delay(handle, 120);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// FT81X functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_initialise(FT81X_Handle *handle)
{
    ft81x_result res;

    // generic board initialise
    res = ft81x_board_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // generic platform initialise
    res = ft81x_platform_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // generic display initialise
    res = ft81x_display_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // initialise comms to the GPU
    res = ft81x_board_gpu_comms_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // initialise comms to the display
    res = ft81x_display_comms_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

void ft81x_cleanup(FT81X_Handle *handle)
{
    ft81x_board_cleanup(handle);
    ft81x_platform_cleanup(handle);
    ft81x_display_cleanup(handle);
}

ft81x_result ft81x_configure(FT81X_Handle *handle)
{
    ft81x_result res;

    // configure the GPU
    res = configure_gpu(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // configure the display
    return ft81x_display_send_configuration(handle);
}

ft81x_result ft81x_reset(FT81X_Handle *handle)
{
    ft81x_result res;

    // First assert power down
    res = ft81x_platform_set_power_down_pin(handle, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait a bit
    res = ft81x_platform_delay(handle, 10);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Then take out of reset using the PD pin
    res = ft81x_platform_set_power_down_pin(handle, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait a bit
    res = ft81x_platform_delay(handle, 25);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return res;
}

ft81x_result ft81x_set_active(FT81X_Handle *handle)
{
    ft81x_result res;

    // Send the ACTIVE command to wake it up
    res = ft81x_platform_gpu_send_command(handle, FT81X_COMMAND_ACTIVE, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // wait a bit more for it to wake up
    res = ft81x_platform_delay(handle, 1);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}
