#ifndef __BOARD_H
#define __BOARD_H

#include "ft81x/boards/board_types.h"

// the config file defines the board
#include "ft81x_config.h"

#ifndef FT81X_BOARD
#  error FT81X_BOARD is not defined
#elif ((FT81X_BOARD) == (FT81X_BOARD_FRDM_K66F))
#  include "ft81x/boards/frdm_k66f.h"
#else
#  error FT81X_BOARD is not a defined board
#endif

#include "ft81x.h"

// functions
ft81x_result ft81x_board_initialise(FT81X_Handle *handle);
void ft81x_board_cleanup(FT81X_Handle *handle);

#endif
