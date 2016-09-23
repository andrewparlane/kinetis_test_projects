#ifndef __FT81X_CO_PROCESSOR_H
#define __FT81X_CO_PROCESSOR_H

#include "ft81x_config.h"

// if we write directly to DL RAM
// then we can't use any of these commands
#ifndef FT81X_DL_WRITE_DIRECTLY_TO_DL_RAM

#include "ft81x.h"

// ----------------------------------------------------------------------------
// Option flags
// ----------------------------------------------------------------------------
#define FT81X_COPROC_OPTION_CENTREX         0x0200
#define FT81X_COPROC_OPTION_CENTREY         0x0400
#define FT81X_COPROC_OPTION_CENTRE          ((FT81X_COPROC_OPTION_CENTREX) | (FT81X_COPROC_OPTION_CENTREY))
#define FT81X_COPROC_OPTION_RIGHTX          0x0800

// Starts a new display list
ft81x_result ft81x_coproc_cmd_dlstart(FT81X_Handle *handle);

// Swaps the display list (uses REG_DLSWAP)
ft81x_result ft81x_coproc_cmd_swap(FT81X_Handle *handle);

#endif

#endif
