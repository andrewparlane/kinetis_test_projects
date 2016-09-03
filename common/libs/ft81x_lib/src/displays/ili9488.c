#include "ft81x/boards/board.h" // defines the display type
#include "ft81x/platforms/platform.h"
#include "ft81x/ft81x_types.h"

// only compile this file if we have the ILI9488 display defined
#if FT81X_DISPLAY == FT81X_DISPLAY_ILI9488

// ----------------------------------------------------------------------------
// Commands
// ----------------------------------------------------------------------------
// basic commands
#define CMD_SOFTWARE_RESET              0x01
#define CMD_READ_ID                     0x04
#define CMD_SLEEP_OUT                   0x11
#define CMD_DISPLAY_ON                  0x29
#define CMD_COLUMN_ADDR_SET             0x2A
#define CMD_PAGE_ADDR_SET               0x2B
#define CMD_MEMORY_WRITE                0x2C
#define CMD_MEM_ACCESS_CTRL             0x36
#define CMD_INTERFACE_PIXEL_FORMAT      0x3A
#define     INTERFACE_PIXEL_FORMAT_18BIT_DPI    0x60
#define     INTERFACE_PIXEL_FORMAT_18BIT_DBI    0x06
#define CMD_WRITE_CONTROL_DISPLAY_VAL   0x53

// extended commands
#define CMD_INTERFACE_MODE_CONTROL      0xB0
#define CMD_FRAME_RATE_CONTROL          0xB1
#define CMD_DISPLAY_FUNCTION_CONTROL    0xB6
#define CMD_SET_IMAGE_FUNCTION          0xE9
#define CMD_ADJUST_CONTROL_3            0xF7
#define     ADJUST_CONTROL_PARAM_0              0xa9
#define     ADJUST_CONTROL_PARAM_1              0x51
#define     ADJUST_CONTROL_PARAM_2              0x2c
#define     ADJUST_CONTROL_PARAM_3              0x02
#define         ADJUST_CONTROL_PARAM_3_STREAM_PKT   0x00
#define         ADJUST_CONTROL_PARAM_3_LOOSE_PKT    0x80

// ----------------------------------------------------------------------------
// Helper macros
// ----------------------------------------------------------------------------
#define SET_DCX(val)                                                                                                                            \
{                                                                                                                                               \
    ft81x_result res = ft81x_platform_set_gpio_pin(platform_user_data, FT81X_BOARD_DISPLAY_DCX_PIN_PORT, FT81X_BOARD_DISPLAY_DCX_PIN_NUM, val); \
    if (res != FT81X_RESULT_OK)                                                                                                                 \
    {                                                                                                                                           \
        return res;                                                                                                                             \
    }                                                                                                                                           \
}

static ft81x_result ili9488_write_cmd(void *platform_user_data, uint8_t cmd, uint32_t data_count, uint8_t *data);
#define WRITE_CMD(cmd, count, data)                                             \
{                                                                               \
    ft81x_result res = ili9488_write_cmd(platform_user_data, cmd, count, data); \
    if (res != FT81X_RESULT_OK)                                                 \
    {                                                                           \
        return res;                                                             \
    }                                                                           \
}

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
static ft81x_result ili9488_write_cmd(void *platform_user_data, uint8_t cmd, uint32_t data_count, uint8_t *data)
{
    ft81x_result res;

    // send command byte
    SET_DCX(0);
    uint8_t tmp;
    res = ft81x_platform_display_spi_transfer(platform_user_data, 1, &cmd, &tmp);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    if (data_count && data != NULL)
    {
        // send the data bytes
        SET_DCX(1);
        res = ft81x_platform_display_spi_transfer(platform_user_data, data_count, data, data);

        // set DCX to default
        SET_DCX(0);
    }

    return res;
}

// ----------------------------------------------------------------------------
// Display functions
// ----------------------------------------------------------------------------
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

ft81x_result ft81x_display_read_id(void *platform_user_data, uint32_t *id)
{
    uint8_t tx_data[5] = { CMD_READ_ID, 0, 0, 0, 0 };
    uint8_t rx_data[5];
    ft81x_result res;

    SET_DCX(0);

    res = ft81x_platform_display_spi_transfer(platform_user_data, 5, tx_data, rx_data);

    // this command has a one bit dummy cycle between the command and the id
    // which means that the id starts in the second byte but
    // but shifted one bit to the right
    *id = (((uint32_t)rx_data[1]) << 17) |
          (((uint32_t)rx_data[2]) << 9)  |
          (((uint32_t)rx_data[3]) << 1)  |
          (((uint32_t)rx_data[4]) >> 7);

    return res;
}

ft81x_result ft81x_display_send_configuration(void *platform_user_data)
{
    ft81x_result res;
    uint8_t data[4];

    // software reset (no parameter)
    WRITE_CMD(CMD_SOFTWARE_RESET, 0, NULL);

    // requires 5ms delay after this for normal commands,
    // but 120mS for sleep commands, and since we go to sleep later
    // better do the delay here
    res = ft81x_platform_delay(platform_user_data, 120);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // Set width of screen
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = (((FT81X_SCREEN_WIDTH)-1) >> 8) & 0xFF;
    data[3] = ((FT81X_SCREEN_WIDTH)-1) & 0xFF;
    WRITE_CMD(CMD_COLUMN_ADDR_SET, 4, data);

    // set height of screen
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = (((FT81X_SCREEN_HEIGHT)-1) >> 8) & 0xFF;
    data[3] = ((FT81X_SCREEN_HEIGHT)-1) & 0xFF;
    WRITE_CMD(CMD_PAGE_ADDR_SET, 4, data);

    // configure frame rate
# warning TODO configure based on defines
    data[0] = 0xB0;
    WRITE_CMD(CMD_FRAME_RATE_CONTROL, 1, data);

    // DSI with DCS command, use loose packet ???
    data[0] = ADJUST_CONTROL_PARAM_0;
    data[1] = ADJUST_CONTROL_PARAM_1;
    data[2] = ADJUST_CONTROL_PARAM_2;
    data[3] = ADJUST_CONTROL_PARAM_3 | ADJUST_CONTROL_PARAM_3_LOOSE_PKT;
    WRITE_CMD(CMD_ADJUST_CONTROL_3, 4, data);

    // Set DOTCLK polarity for falling edge
    data[0] = 2;
    WRITE_CMD(CMD_INTERFACE_MODE_CONTROL, 1, data);

    // set the pixel format to 18 bit for both RGB and MCU interfaces
    data[0] = INTERFACE_PIXEL_FORMAT_18BIT_DPI | INTERFACE_PIXEL_FORMAT_18BIT_DBI;
    WRITE_CMD(CMD_INTERFACE_PIXEL_FORMAT, 1, data);

    // disable 24 bit data bus (think this is the default)
    data[0] = 0;
    WRITE_CMD(CMD_SET_IMAGE_FUNCTION, 1, data);

    // enable backlight and dimming control
    data[0] = 0x2C;
    WRITE_CMD(CMD_WRITE_CONTROL_DISPLAY_VAL, 1, data);

    // Set pixel format to BGR
    // and reflect column data (why?)
    data[0] = 0x48;
    WRITE_CMD(CMD_MEM_ACCESS_CTRL, 1, data);

    // not really sure on this
    data[0] = 0x30;
    data[1] = 0x02;
    data[2] = 0x3b;
    WRITE_CMD(CMD_DISPLAY_FUNCTION_CONTROL, 3, data);

    // put the ILI9488 into low power mode (but still working)
    WRITE_CMD(CMD_SLEEP_OUT, 0, NULL);

    // Might require a delay (unclear)
    res = ft81x_platform_delay(platform_user_data, 120);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // turn the display on
    WRITE_CMD(CMD_DISPLAY_ON, 0, NULL);

    // not sure what this does
    data[0] = 0xB0;
    WRITE_CMD(CMD_MEMORY_WRITE, 1, data);

    return FT81X_RESULT_OK;
}

#endif
