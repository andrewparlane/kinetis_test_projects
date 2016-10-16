#if _MSC_VER
#include <windows.h>
#else // __GNUC__ - may need other defines for different compilers

// freeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

#define Sleep(ms)    vTaskDelay(ms)

#endif

