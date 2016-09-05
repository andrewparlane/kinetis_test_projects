#ifndef __DISPLAY_TYPES_H
#define __DISPLAY_TYPES_H

// supported displays - for use with #define FT81X_DISPLAY
#define FT81X_DISPLAY_ILI9488               0

// comms types - for use with #define FT81X_DISPLAY_COMMS_TYPE
#define FT81X_DISPLAY_COMMS_TYPE_SPI        0

// spi modes - for use with #define FT81X_DISPLAY_SPI_MODE
#define FT81X_DISPLAY_SPI_MODE_0            0
#define FT81X_DISPLAY_SPI_MODE_1            1
#define FT81X_DISPLAY_SPI_MODE_2            2
#define FT81X_DISPLAY_SPI_MODE_3            3

// spi bit order - for use with #define FT81X_DISPLAY_SPI_BIT_ORDER
#define FT81X_DISPLAY_SPI_BIT_ORDER_MSB     0
#define FT81X_DISPLAY_SPI_BIT_ORDER_LSB     1

// spi cs polarity - for use with #define FT81X_DISPLAY_SPI_CS_POLARITY
#define FT81X_DISPLAY_SPI_CS_ACTIVE_LOW     0
#define FT81X_DISPLAY_SPI_CS_ACTIVE_HIGH    1

#endif
