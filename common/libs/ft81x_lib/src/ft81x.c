#include "ft81x.h"
#include "ft81x_display_list.h"
#include "ft81x/commands.h"
#include "ft81x/memory_map.h"
#include "ft81x/g_ram_manager.h"

#include "ft81x/boards/board.h"
#include "ft81x/displays/display.h"
#include "ft81x/platforms/platform.h"

#include <stdio.h>

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
static ft81x_result configure_gpu(FT81X_Handle *handle)
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
#pragma message WARN("TODO use external clock")
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

#pragma message WARN("TODO Clock trimming?")

    // Horizontal parameters
    // total pixels per line (visible + not visible)
    // note: HCYCLE must be > HSIZE + HOFFSET
    //       this is according to FTDI support
    //       Hence the +1 for HCYCLE
    WRITE_GPU_REG_16(FT81X_REG_HCYCLE, (FT81X_SCREEN_TOTAL_WIDTH)+1);
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
    // note: VCYCLE must be > VSIZE + VOFFSET
    //       this is according to FTDI support
    //       Hence the +1 for VCYCLE
    WRITE_GPU_REG_16(FT81X_REG_VCYCLE, (FT81X_SCREEN_TOTAL_HEIGHT)+1);
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

    // disable the backlight for now
    res = ft81x_backlight(handle, FT81X_BACKLIGHT_LEVEL_OFF);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // configure the GPIO pins
    // DISP is an output, the rest are inputs
    // DISP starts as 1 (enable)
    WRITE_GPU_REG_8(FT81X_REG_GPIO, FT81X_REG_GPIO_DISP);
    WRITE_GPU_REG_8(FT81X_REG_GPIO_DIR, FT81X_REG_GPIO_DISP);

    // clear the display to black
    res = ft81x_graphics_engine_start_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }
    const uint32_t init_dl[] =
    {
        FT81X_DL_CMD_CLEAR_COLOUR_RGB(0,0,0),
        FT81X_DL_CMD_CLEAR(1,1,1),
        FT81X_DL_CMD_DISPLAY()
    };
    res = ft81x_graphics_engine_write_display_list_snippet(handle, sizeof(init_dl), init_dl);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }
    res = ft81x_graphics_engine_end_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

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
ft81x_result ft81x_initialise(FT81X_Handle *handle, uint32_t buffer_size, uint8_t *buffer)
{
    ft81x_result res;

    if (handle == NULL)
    {
        return FT81X_RESULT_NO_HANDLE;
    }

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
    res = ft81x_platform_gpu_spi_comms_initialise(handle);
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

    // g_ram manager initialise
    res = ft81x_g_ram_manager_initialise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // graphics engine initialise
    res = ft81x_graphics_engine_initialise(handle, buffer_size, buffer);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // image manager initialise
    res = ft81x_bitmap_handles_initialise(handle);
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
    res = ft81x_display_send_configuration(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return res;
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

ft81x_result ft81x_backlight(FT81X_Handle *handle, ft81x_backlight_level level)
{
    WRITE_GPU_REG_8(FT81X_REG_PWM_DUTY, level);
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_dump_registers(FT81X_Handle *handle)
{
    DUMP_GPU_REG_32(FT81X_REG_ID_ADDR);
    DUMP_GPU_REG_32(FT81X_REG_FRAMES);
    DUMP_GPU_REG_32(FT81X_REG_CLOCK);
    DUMP_GPU_REG_32(FT81X_REG_FREQUENCY);
    DUMP_GPU_REG_32(FT81X_REG_RENDERMODE);
    DUMP_GPU_REG_32(FT81X_REG_SNAPY);
    DUMP_GPU_REG_32(FT81X_REG_SNAPSHOT);
    DUMP_GPU_REG_32(FT81X_REG_SNAPFORMAT);
    DUMP_GPU_REG_32(FT81X_REG_CPURESET);
    DUMP_GPU_REG_32(FT81X_REG_TAP_CRC);
    DUMP_GPU_REG_32(FT81X_REG_TAP_MASK);
    DUMP_GPU_REG_32(FT81X_REG_HCYCLE);
    DUMP_GPU_REG_32(FT81X_REG_HOFFSET);
    DUMP_GPU_REG_32(FT81X_REG_HSIZE);
    DUMP_GPU_REG_32(FT81X_REG_HSYNC0);
    DUMP_GPU_REG_32(FT81X_REG_HSYNC1);
    DUMP_GPU_REG_32(FT81X_REG_VCYCLE);
    DUMP_GPU_REG_32(FT81X_REG_VOFFSET);
    DUMP_GPU_REG_32(FT81X_REG_VSIZE);
    DUMP_GPU_REG_32(FT81X_REG_VSYNC0);
    DUMP_GPU_REG_32(FT81X_REG_VSYNC1);
    DUMP_GPU_REG_32(FT81X_REG_DLSWAP);
    DUMP_GPU_REG_32(FT81X_REG_ROTATE);
    DUMP_GPU_REG_32(FT81X_REG_OUTBITS);
    DUMP_GPU_REG_32(FT81X_REG_DITHER);
    DUMP_GPU_REG_32(FT81X_REG_SWIZZLE);
    DUMP_GPU_REG_32(FT81X_REG_CSPREAD);
    DUMP_GPU_REG_32(FT81X_REG_PCLK_POL);
    DUMP_GPU_REG_32(FT81X_REG_PCLK);
    DUMP_GPU_REG_32(FT81X_REG_GPIO_DIR);
    DUMP_GPU_REG_32(FT81X_REG_GPIO);
    DUMP_GPU_REG_32(FT81X_REG_INT_FLAGS);
    DUMP_GPU_REG_32(FT81X_REG_INT_EN);
    DUMP_GPU_REG_32(FT81X_REG_INT_MASK);
    DUMP_GPU_REG_32(FT81X_REG_PWM_DUTY);
    DUMP_GPU_REG_32(FT81X_REG_CMD_READ);
    DUMP_GPU_REG_32(FT81X_REG_CMD_WRITE);
    DUMP_GPU_REG_32(FT81X_REG_CMD_DL);
    DUMP_GPU_REG_32(FT81X_REG_TOUCH_RZTHRESH);

    return FT81X_RESULT_OK;
}
