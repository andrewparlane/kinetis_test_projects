#include "ft81x_touch_manager.h"

#include "ft81x_display_list.h"
#include "ft81x_co_processor.h"
#include "ft81x_graphics_engine.h"

#include "ft81x/platforms/platform.h"

ft81x_result ft81x_touch_manager_calibrate(FT81X_Handle *handle)
{
    ft81x_result res;

    // we use CMD_CALIBRATE which sticks the result
    // in the 4 bytes after the command.
    // so first get the address of where that result will be stored
    uint16_t result_location;
    READ_GPU_REG_16(FT81X_REG_CMD_WRITE, result_location);
    result_location += 4;

    // send the calibration command
    res = ft81x_coproc_cmd_calibrate(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // display it
    res = ft81x_graphics_engine_write_display_list_cmd(handle, FT81X_DL_CMD_DISPLAY());
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // write everything to the DL ram and then swap it in
    res = ft81x_graphics_engine_end_display_list(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // wait until it's done
    res = ft81x_graphics_engine_flush_and_synchronise(handle);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // finally we read the result out of the command buffer
    uint32_t calibration_result;
    READ_GPU_REG_32(FT81X_COPROC_CMD_RAM + result_location, calibration_result);

    if (calibration_result == 0)
    {
        return FT81X_RESULT_CALIBRATION_FAILED;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_touch_manager_write_calibration_values(FT81X_Handle *handle, uint32_t values[6])
{
    for (int i = 0; i < 6; i++)
    {
        WRITE_GPU_REG_32(FT81X_REG_TOUCH_TRANSFORM_A + (i * 4), values[i]);
    }
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_touch_manager_get_calibration_values(FT81X_Handle *handle, uint32_t values[6])
{
    for (int i = 0; i < 6; i++)
    {
        READ_GPU_REG_32(FT81X_REG_TOUCH_TRANSFORM_A + (i * 4), values[i]);
    }
    return FT81X_RESULT_OK;
}
