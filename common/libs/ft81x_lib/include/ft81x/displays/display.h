#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "ft81x/displays/display_types.h"

// the board file defines the display
#include "ft81x/boards/board.h"

#ifndef FT81X_DISPLAY
#  error FT81X_DISPLAY is not defined
#elif ((FT81X_DISPLAY) == (FT81X_DISPLAY_ILI9488))
#  include "ft81x/displays/ili9488.h"
#else
#  error FT81X_DISPLAY is not a defined display
#endif


// check the display header has the required defines
// first comms type
#ifndef FT81X_DISPLAY_COMMS_TYPE
#  error Display must define FT81X_DISPLAY_COMMS_TYPE
#elif ((FT81X_DISPLAY_COMMS_TYPE) == (FT81X_DISPLAY_COMMS_TYPE_SPI))
// spi comms type specific defines
#  if !defined(FT81X_DISPLAY_SPI_MODE)
#    error Display comms type is SPI but SPI mode is not valid
#  endif
#  if !defined(FT81X_DISPLAY_SPI_BIT_ORDER)
#    error Display comms type is SPI but SPI bit order is not valid
#  endif
#  if !defined(FT81X_DISPLAY_SPI_CS_POLARITY)
#    error Display comms type is SPI but SPI CS polarity is not valid
#  endif
#else
#  error Display defined FT81X_DISPLAY_COMMS_TYPE as an unknown comms type
#endif

#include "ft81x/ft81x_types.h"

// functions
ft81x_result ft81x_display_initialise(void *platform_user_data);
ft81x_result ft81x_display_comms_initialise(void *platform_user_data);

ft81x_result ft81x_display_read_id(void *platform_user_data, uint32_t *id);

ft81x_result ft81x_display_send_configuration(void *platform_user_data);

#endif
