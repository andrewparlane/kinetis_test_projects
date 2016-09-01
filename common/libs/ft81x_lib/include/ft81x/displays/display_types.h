#ifndef __DISPLAY_TYPES_H
#define __DISPLAY_TYPES_H

// supported displays
enum
{
    FT81X_DISPLAY_ILI9488,
};

// comms types
enum
{
    FT81X_DISPLAY_COMMS_TYPE_SPI = 0,

    // number of valid comms types
    FT81X_DISPLAY_COMMS_TYPE_COUNT
};

// spi modes
enum
{
    FT81X_DISPLAY_SPI_MODE_0 = 0,
    FT81X_DISPLAY_SPI_MODE_1,
    FT81X_DISPLAY_SPI_MODE_2,
    FT81X_DISPLAY_SPI_MODE_3,

    // number of valid spi modes
    FT81X_DISPLAY_SPI_MODE_COUNT
};


// spi bit order
enum
{
    FT81X_DISPLAY_SPI_BIT_ORDER_MSB = 0,
    FT81X_DISPLAY_SPI_BIT_ORDER_LSB,

    // number of valid bit orders
    FT81X_DISPLAY_SPI_BIT_ORDER_COUNT
};

// spi cs polarity
enum
{
    FT81X_DISPLAY_SPI_CS_ACTIVE_LOW = 0,
    FT81X_DISPLAY_SPI_CS_ACTIVE_HIGH,

    // number of valid CS polarities
    FT81X_DISPLAY_SPI_CS_POLARITY_COUNT
};

#endif
