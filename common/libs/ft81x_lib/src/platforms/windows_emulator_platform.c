#include "ft81x/boards/board.h" // defines the platform type

#include "ft81x/platforms/platform.h"
#include "ft81x/displays/display.h"

// only compile this file if we have the nxp kinetis k66 platform defined
#if ((FT81X_PLATFORM) == (FT81X_PLATFORM_WINDOWS_EMULATOR))

#include <windows.h>

// ----------------------------------------------------------------------------
// Initialise functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_spi_comms_initialise(FT81X_Handle *handle)
{
    // nothing to do here
    return FT81X_RESULT_OK;
}

void ft81x_platform_cleanup(FT81X_Handle *handle)
{
}

// ----------------------------------------------------------------------------
// GPU transfer functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_gpu_send_command(FT81X_Handle *handle, ft81x_command command, uint8_t param)
{
    FT8XXEMU_cs(1);
	FT8XXEMU_transfer((uint8_t) command);
	FT8XXEMU_transfer(param);
	FT8XXEMU_transfer(0);
    FT8XXEMU_cs(0);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_write_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, const uint8_t *data)
{
    if (count == 0)
    {
        return FT81X_RESULT_INVALID_ARG;
    }

    // assert the CS
    FT8XXEMU_cs(1);

    // send the address
    // the | 0x80 is because we are writing
    FT8XXEMU_transfer(((address >> 16) & 0x3F) | 0x80);
    FT8XXEMU_transfer((address >> 8)   & 0xFF);
    FT8XXEMU_transfer(address          & 0xFF);

    // now send the data
    for (uint32_t i = 0; i < count; i++)
    {
        FT8XXEMU_transfer(data[i]);
    }

    // release CS
    FT8XXEMU_cs(0);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_write_register_8(FT81X_Handle *handle, uint32_t address, uint8_t data)
{
    return ft81x_platform_gpu_write_mem(handle, address, 1, &data);
}

ft81x_result ft81x_platform_gpu_write_register_16(FT81X_Handle *handle, uint32_t address, uint16_t data)
{
    return ft81x_platform_gpu_write_mem(handle, address, 2, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_write_register_32(FT81X_Handle *handle, uint32_t address, uint32_t data)
{
    return ft81x_platform_gpu_write_mem(handle, address, 4, (uint8_t *)&data);
}

ft81x_result ft81x_platform_gpu_read_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, uint8_t *data)
{
    // assert the CS
    FT8XXEMU_cs(1);

    // send the address
    FT8XXEMU_transfer((address >> 16) & 0x3F);
    FT8XXEMU_transfer((address >> 8)  & 0xFF);
    FT8XXEMU_transfer(address         & 0xFF);

    // dummy read byte
    FT8XXEMU_transfer(0);

    // now read the data
    for (uint32_t i = 0; i < count; i++)
    {
        data[i] = FT8XXEMU_transfer(0);
    }

    // release CS
    FT8XXEMU_cs(0);

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_platform_gpu_read_register_8(FT81X_Handle *handle, uint32_t address, uint8_t *value)
{
    return ft81x_platform_gpu_read_mem(handle, address, 1, (uint8_t *)value);
}

ft81x_result ft81x_platform_gpu_read_register_16(FT81X_Handle *handle, uint32_t address, uint16_t *value)
{
    return ft81x_platform_gpu_read_mem(handle, address, 2, (uint8_t *)value);
}

ft81x_result ft81x_platform_gpu_read_register_32(FT81X_Handle *handle, uint32_t address, uint32_t *value)
{
    return ft81x_platform_gpu_read_mem(handle, address, 4, (uint8_t *)value);
}

// ----------------------------------------------------------------------------
// Power functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_set_power_down_pin(FT81X_Handle *handle, uint8_t power_down)
{
    return FT81X_RESULT_OK;
}

// ----------------------------------------------------------------------------
// GPIO functions
// note: These shouldn't be used externally for the PD pin
//       as that is managed internally
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, ft81x_gpio_direction direction, uint8_t value)
{
    return FT81X_RESULT_NOT_SUPPORTED;
}

ft81x_result ft81x_platform_set_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, uint8_t value)
{
    return FT81X_RESULT_NOT_SUPPORTED;
}

// ----------------------------------------------------------------------------
// Misc platform functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_delay(FT81X_Handle *handle, uint32_t milliseconds)
{
    Sleep(milliseconds);
    return FT81X_RESULT_OK;
}

#endif
