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

#include "ft81x/commands.h"

#include <stdint.h>

typedef enum
{
    FT81X_PLATFORM_GPIO_DIRECTION_INPUT,
    FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT
} ft81x_gpio_direction;

// ----------------------------------------------------------------------------
// Helper macros
// ----------------------------------------------------------------------------
#define WRITE_GPU_MEM(addr, count, data)                                                    \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_write_mem((handle), (addr), (count), (data));     \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_8(addr, data)                                                         \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_write_register_8((handle), (addr), (data));       \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define WRITE_GPU_REG_16(addr, data)                                                            \
{                                                                                               \
    ft81x_result res = ft81x_platform_gpu_write_register_16((handle), (addr), (data));          \
    if (res != FT81X_RESULT_OK)                                                                 \
    {                                                                                           \
        return res;                                                                             \
    }                                                                                           \
}

#define WRITE_GPU_REG_32(addr, data)                                                            \
{                                                                                               \
    ft81x_result res = ft81x_platform_gpu_write_register_32((handle), (addr), (data));          \
    if (res != FT81X_RESULT_OK)                                                                 \
    {                                                                                           \
        return res;                                                                             \
    }                                                                                           \
}

#define READ_GPU_MEM(addr, count, data)                                                     \
{                                                                                           \
    ft81x_result res = ft81x_platform_gpu_read_mem((handle), (addr), (count), (data));      \
    if (res != FT81X_RESULT_OK)                                                             \
    {                                                                                       \
        return res;                                                                         \
    }                                                                                       \
}

#define READ_GPU_REG_32(addr, data)                                                             \
{                                                                                               \
    ft81x_result res = ft81x_platform_gpu_read_register_32((handle), (addr), &(data));          \
    if (res != FT81X_RESULT_OK)                                                                 \
    {                                                                                           \
        return res;                                                                             \
    }                                                                                           \
}

#define DUMP_GPU_REG_32(addr)                                           \
{                                                                       \
    uint32_t data;                                                      \
    READ_GPU_REG_32((addr), data);                                      \
    printf("%s (%08X) = %08X\n", #addr, (addr), (unsigned int)data);    \
}

// ----------------------------------------------------------------------------
// Platform API functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_platform_initialise(FT81X_Handle *handle);
ft81x_result ft81x_platform_gpu_spi_comms_initialise(FT81X_Handle *handle);
ft81x_result ft81x_platform_display_spi_comms_initialise(FT81X_Handle *handle);
void ft81x_platform_cleanup(FT81X_Handle *handle);

ft81x_result ft81x_platform_gpu_send_command(FT81X_Handle *handle, ft81x_command command, uint8_t param);
ft81x_result ft81x_platform_gpu_write_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, const uint8_t *data);
ft81x_result ft81x_platform_gpu_write_register_8(FT81X_Handle *handle, uint32_t address, uint8_t data);
ft81x_result ft81x_platform_gpu_write_register_16(FT81X_Handle *handle, uint32_t address, uint16_t data);
ft81x_result ft81x_platform_gpu_write_register_32(FT81X_Handle *handle, uint32_t address, uint32_t data);
ft81x_result ft81x_platform_gpu_read_mem(FT81X_Handle *handle, uint32_t address, uint32_t count, uint8_t *data);
ft81x_result ft81x_platform_gpu_read_register_8(FT81X_Handle *handle, uint32_t address, uint8_t *value);
ft81x_result ft81x_platform_gpu_read_register_16(FT81X_Handle *handle, uint32_t address, uint16_t *value);
ft81x_result ft81x_platform_gpu_read_register_32(FT81X_Handle *handle, uint32_t address, uint32_t *value);

ft81x_result ft81x_platform_display_spi_transfer(FT81X_Handle *handle, uint32_t count, const uint8_t *tx_data, uint8_t *rx_data);

ft81x_result ft81x_platform_set_power_down_pin(FT81X_Handle *handle, uint8_t power_down);

// note: These shouldn't be used externally for the PD pin
//       as that is managed internally
ft81x_result ft81x_platform_initialise_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, ft81x_gpio_direction direction, uint8_t value);
ft81x_result ft81x_platform_set_gpio_pin(FT81X_Handle *handle, void *port, uint32_t pin, uint8_t value);

ft81x_result ft81x_platform_delay(FT81X_Handle *handle, uint32_t milliseconds);

#endif
