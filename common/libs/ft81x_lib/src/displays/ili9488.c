#include "ft81x/boards/board.h" // defines the display type
#include "ft81x/platforms/platform.h"

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
#define     MEM_ACCESS_CTRL_BGR                 0x08
#define CMD_INTERFACE_PIXEL_FORMAT      0x3A
#define     INTERFACE_PIXEL_FORMAT_18BIT_DPI    0x60
#define     INTERFACE_PIXEL_FORMAT_18BIT_DBI    0x06
#define CMD_WRITE_CONTROL_DISPLAY_VAL   0x53

// extended commands
#define CMD_INTERFACE_MODE_CONTROL      0xB0
#define     INTERFACE_MODE_CONTROL_DPL(dpl)             (((dpl) & 0x01) << 1)
#define CMD_FRAME_RATE_CONTROL          0xB1
#define     FRAME_RATE_CONTROL_PARAM_0_FRS(frs)         (((frs) & 0x0F) << 4)
#define     FRAME_RATE_CONTROL_PARAM_0_DIVA_DEFAULT     0
#define     FRAME_RATE_CONTROL_PARAM_1_RTNA_DEFAULT     0x11
#define CMD_BLANKING_PORCH_CONTROL      0xB5
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
    ft81x_result res = ft81x_platform_set_gpio_pin(handle, FT81X_BOARD_DISPLAY_DCX_PIN_PORT, FT81X_BOARD_DISPLAY_DCX_PIN_NUM, val); \
    if (res != FT81X_RESULT_OK)                                                                                                                 \
    {                                                                                                                                           \
        return res;                                                                                                                             \
    }                                                                                                                                           \
}

static ft81x_result ili9488_write_cmd(FT81X_Handle *handle, uint8_t cmd, uint32_t data_count, uint8_t *data);
#define WRITE_CMD(cmd, count, data)                                             \
{                                                                               \
    ft81x_result res = ili9488_write_cmd(handle, cmd, count, data); \
    if (res != FT81X_RESULT_OK)                                                 \
    {                                                                           \
        return res;                                                             \
    }                                                                           \
}

// ----------------------------------------------------------------------------
// Local functions
// ----------------------------------------------------------------------------
static ft81x_result ili9488_write_cmd(FT81X_Handle *handle, uint8_t cmd, uint32_t data_count, uint8_t *data)
{
    ft81x_result res;

    // send command byte
    SET_DCX(0);
    uint8_t tmp;
    res = ft81x_platform_display_spi_transfer(handle, 1, &cmd, &tmp);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    if (data_count && data != NULL)
    {
        // send the data bytes
        SET_DCX(1);
        res = ft81x_platform_display_spi_transfer(handle, data_count, data, data);

        // set DCX to default
        SET_DCX(0);
    }

    return res;
}

// ----------------------------------------------------------------------------
// Display functions
// ----------------------------------------------------------------------------
ft81x_result ft81x_display_initialise(FT81X_Handle *handle)
{
    ft81x_result res;

    // This display uses a DCX pin as part of it's SPI comms
    res = ft81x_platform_initialise_gpio_pin(handle, FT81X_BOARD_DISPLAY_DCX_PIN_PORT, FT81X_BOARD_DISPLAY_DCX_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // We also have a disp pin that determines if the display is on or off
    res = ft81x_platform_initialise_gpio_pin(handle, FT81X_BOARD_DISPLAY_DISP_PIN_PORT, FT81X_BOARD_DISPLAY_DISP_PIN_NUM, FT81X_PLATFORM_GPIO_DIRECTION_OUTPUT, 0);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

ft81x_result ft81x_display_comms_initialise(FT81X_Handle *handle)
{
    // this display needs some set up so we need to initialise the comms
    return ft81x_platform_display_spi_comms_initialise(handle);
}

void ft81x_display_cleanup(FT81X_Handle *handle)
{
}

ft81x_result ft81x_display_read_id(FT81X_Handle *handle, uint32_t *id)
{
    uint8_t tx_data[5] = { CMD_READ_ID, 0, 0, 0, 0 };
    uint8_t rx_data[5];
    ft81x_result res;

    SET_DCX(0);

    res = ft81x_platform_display_spi_transfer(handle, 5, tx_data, rx_data);

    // this command has a one bit dummy cycle between the command and the id
    // which means that the id starts in the second byte but
    // but shifted one bit to the right
    *id = (((uint32_t)rx_data[1]) << 17) |
          (((uint32_t)rx_data[2]) << 9)  |
          (((uint32_t)rx_data[3]) << 1)  |
          (((uint32_t)rx_data[4]) >> 7);

    return res;
}

ft81x_result ft81x_display_send_configuration(FT81X_Handle *handle)
{
    ft81x_result res;
    uint8_t data[4];

    // software reset (no parameter)
    WRITE_CMD(CMD_SOFTWARE_RESET, 0, NULL);

    // requires 5ms delay after this for normal commands,
    // but 120mS for sleep commands, and since we go to sleep later
    // better do the delay here
    res = ft81x_platform_delay(handle, 120);
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

    // configure the blanking settings
    data[0] = (FT81X_SCREEN_VERT_FRONT_PORCH);
    data[1] = (FT81X_SCREEN_VERT_BACK_PORCH);
    data[2] = (FT81X_SCREEN_HORIZ_FRONT_PORCH);
    data[3] = (FT81X_SCREEN_HORIZ_BACK_PORCH);
    WRITE_CMD(CMD_BLANKING_PORCH_CONTROL, 4, data);

    // configure frame rate
    // this calulation is kind of confusing, the data sheet is not very clear
    // so we simplify things a bit.
    //   Fosc = 18MHz (I think this is always the case)
    //   DIVA = 0. IE. use Fosc as the clock, don't divide it
    //   RTNA = 0x11 (17) the default
    // using these we get the equation
    //   frame rate = Fosc / (2**DIVA * (76 - (FRS*4)) * RTNA * total_height)
    // we can rearrange this for FRS
    //   FRS = 19 - (Fosc / (frame rate * 2**DIVA * RTNA * total_height * 4))
    const uint32_t fosc = 18*1000*1000;
    const uint8_t diva = (FRAME_RATE_CONTROL_PARAM_0_DIVA_DEFAULT);
    const uint8_t rtna = (FRAME_RATE_CONTROL_PARAM_1_RTNA_DEFAULT);
    const uint8_t frs = 19 - (fosc / ((FT81X_SCREEN_FRAME_RATE) * (1 << diva) * rtna * FT81X_SCREEN_TOTAL_HEIGHT * 4));
    data[0] = (FRAME_RATE_CONTROL_PARAM_0_FRS(frs)) | diva;
    data[1] = rtna;
    WRITE_CMD(CMD_FRAME_RATE_CONTROL, 2, data);

    // DSI with DCS command, use loose packet ???
    data[0] = ADJUST_CONTROL_PARAM_0;
    data[1] = ADJUST_CONTROL_PARAM_1;
    data[2] = ADJUST_CONTROL_PARAM_2;
    data[3] = ADJUST_CONTROL_PARAM_3 | ADJUST_CONTROL_PARAM_3_LOOSE_PKT;
    WRITE_CMD(CMD_ADJUST_CONTROL_3, 4, data);

    // Set DOTCLK polarity to match the GPU.
    // if the GPU outputs on the falling edge,
    // then we read on the rising edge,
    // and vice versa
    data[0] = INTERFACE_MODE_CONTROL_DPL(FT81X_SCREEN_PCLK_POL);
    WRITE_CMD(CMD_INTERFACE_MODE_CONTROL, 1, data);

    // set the pixel format to 18 bit for both RGB and MCU interfaces
#if ((FT81X_BITS_PER_PIXEL) == 18)
    data[0] = INTERFACE_PIXEL_FORMAT_18BIT_DPI | INTERFACE_PIXEL_FORMAT_18BIT_DBI;
#else
    // todo 24 bit - look at CMD_SET_IMAGE_FUNCTION??
#error Unsupported pixel format
#endif
    WRITE_CMD(CMD_INTERFACE_PIXEL_FORMAT, 1, data);

    // enable backlight and dimming control
    data[0] = 0x2C;
    WRITE_CMD(CMD_WRITE_CONTROL_DISPLAY_VAL, 1, data);

    // Reflect column data (why?)
    data[0] = 0x40;
#if ((FT81X_SCREEN_SWIZZLE) == (FT81X_COLOUR_FORMAT_BGR))
    // set to BGR (ie blue and red pins swapped)
    data[0] |= MEM_ACCESS_CTRL_BGR;
#endif
    WRITE_CMD(CMD_MEM_ACCESS_CTRL, 1, data);

    // set some display functions
    data[0] = 0x30;     // not really sure
    data[1] = 0x02;     // default
    // how many visible lines in intervals of 8.
    // (NL[5:0] + 1) * 8 must be >= number of visible lines (FT81X_SCREEN_HEIGHT)
    data[2] = ((uint8_t)(((FT81X_SCREEN_HEIGHT) + 7) / 8)) - 1;
    WRITE_CMD(CMD_DISPLAY_FUNCTION_CONTROL, 3, data);

    // put the ILI9488 into low power mode (but still working)
    WRITE_CMD(CMD_SLEEP_OUT, 0, NULL);

    // Might require a delay (unclear)
    res = ft81x_platform_delay(handle, 120);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    // turn the display on
    WRITE_CMD(CMD_DISPLAY_ON, 0, NULL);

    // not sure what this does
    data[0] = 0xB0;
    WRITE_CMD(CMD_MEMORY_WRITE, 1, data);

    // wait a little
    res = ft81x_platform_delay(handle, 100);
    if (res != FT81X_RESULT_OK)
    {
        return res;
    }

    return FT81X_RESULT_OK;
}

#endif
