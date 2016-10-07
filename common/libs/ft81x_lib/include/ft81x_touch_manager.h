#ifndef __FT81X_TOUCH_MANAGER_H
#define __FT81X_TOUCH_MANAGER_H

#include "ft81x.h"

// Note: This sends the DISPLAY() instruction and swaps the DL in
//       Any DL stuff before this will be in the background of the
//       calibration process
ft81x_result ft81x_touch_manager_calibrate(FT81X_Handle *handle);

ft81x_result ft81x_touch_manager_write_calibration_values(FT81X_Handle *handle, uint32_t values[6]);
ft81x_result ft81x_touch_manager_get_calibration_values(FT81X_Handle *handle, uint32_t values[6]);

ft81x_result ft81x_touch_manager_track_linear_region(FT81X_Handle *handle, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t tag);
ft81x_result ft81x_touch_manager_track_rotary_region(FT81X_Handle *handle, uint16_t x, uint16_t y, uint8_t tag);

// track_value can be NULL if you don't want to read the tracked value
ft81x_result ft81x_touch_manager_check_for_touched_tag(FT81X_Handle *handle, uint8_t *tag, uint16_t *track_value);

#endif
