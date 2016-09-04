#include "ft81x/ft81x.h"
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
    ft81x_result res = ft81x_platform_gpu_write_mem(platform_user_data, addr, count, data); \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_8(addr, data)                                                         \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_write_register_8(platform_user_data, addr, data); \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_16(addr, data)                                                            \
{                                                                                               \
    ft81x_result res = ft81x_platform_gpu_write_register_16(platform_user_data, addr, data);    \
    if (res != FT81X_RESULT_OK)                                                                 \
    {                                                                                           \
        return res;                                                                             \
    }                                                                                           \
}

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
ft81x_result configure_gpu(void *platform_user_data)
{
    ft81x_result res;

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

    // Use the external clock
#warning TODO use external clock
    /*res = ft81x_platform_gpu_send_command(platform_user_data, FT81X_COMMAND_CLKEXT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Wait 10mS
    res = ft81x_platform_delay(platform_user_data, 10);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // switching clk causes a system reset, so send the ACTIVE command again
    res = ft81x_set_active(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }*/

    // read the ID register to confirm the GPU is ready
    uint8_t read_id_ok = 0;
    for (int i = 0; i < 5; i++)
    {
        uint8_t id;
        res = ft81x_platform_gpu_read_register_8(platform_user_data, FT81X_REG_ID_ADDR, &id);
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
        res = ft81x_platform_delay(platform_user_data, 100);
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

#warning TODO set these based off defines
    uint16_t FT_DispWidth = 320;
    uint16_t FT_DispHeight = 480;
    uint16_t FT_DispHCycle =  400;
    uint16_t FT_DispHOffset = 40;
    uint16_t FT_DispHSync0 = 0;
    uint16_t FT_DispHSync1 = 10;
    uint16_t FT_DispVCycle = 500;
    uint16_t FT_DispVOffset = 10;
    uint16_t FT_DispVSync0 = 0;
    uint16_t FT_DispVSync1 = 5;
    uint8_t FT_DispPCLK = 5;
    uint8_t FT_DispSwizzle = 2;
    uint8_t FT_DispPCLKPol = 1;
    uint16_t FT_DispCSpread = 1;
    uint16_t FT_DispDither = 1;

    WRITE_GPU_REG_16(FT81X_REG_HCYCLE, FT_DispHCycle);
    WRITE_GPU_REG_16(FT81X_REG_HOFFSET, FT_DispHOffset);
    WRITE_GPU_REG_16(FT81X_REG_HSYNC0, FT_DispHSync0);
    WRITE_GPU_REG_16(FT81X_REG_HSYNC1, FT_DispHSync1);
    WRITE_GPU_REG_16(FT81X_REG_VCYCLE, FT_DispVCycle);
    WRITE_GPU_REG_16(FT81X_REG_VOFFSET, FT_DispVOffset);
    WRITE_GPU_REG_16(FT81X_REG_VSYNC0, FT_DispVSync0);
    WRITE_GPU_REG_16(FT81X_REG_VSYNC1, FT_DispVSync1);
    WRITE_GPU_REG_8(FT81X_REG_SWIZZLE, FT_DispSwizzle);
    WRITE_GPU_REG_8(FT81X_REG_PCLK_POL, FT_DispPCLKPol);
    WRITE_GPU_REG_16(FT81X_REG_HSIZE, FT_DispWidth);
    WRITE_GPU_REG_16(FT81X_REG_VSIZE, FT_DispHeight);
    WRITE_GPU_REG_16(FT81X_REG_CSPREAD, FT_DispCSpread);
    WRITE_GPU_REG_16(FT81X_REG_DITHER, FT_DispDither);

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
    WRITE_GPU_REG_8(FT81X_REG_PCLK, FT_DispPCLK);

    // Wait 120mS
    res = ft81x_platform_delay(platform_user_data, 120);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// FT81X functions
// ----------------------------------------------------------------------------
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

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_configure(void *platform_user_data)
{
    ft81x_result res;

    // configure the GPU
    res = configure_gpu(platform_user_data);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // configure the display
    return ft81x_display_send_configuration(platform_user_data);
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
    res = ft81x_platform_delay(platform_user_data, 5);
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
    res = ft81x_platform_delay(platform_user_data, 5);
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
