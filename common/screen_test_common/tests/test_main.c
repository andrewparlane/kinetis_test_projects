// ft81x lib includes
#include "ft81x.h"

// standard includes
#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------------------
// FT81X stuff
// ----------------------------------------------------------------------------
#define DL_BUFFER_SIZE 512
uint8_t dl_buffer[DL_BUFFER_SIZE];

// ----------------------------------------------------------------------------
// Test forward defines
// ----------------------------------------------------------------------------

ft81x_result test1(FT81X_Handle *handle);
ft81x_result test2(FT81X_Handle *handle);
ft81x_result test3(FT81X_Handle *handle);

// ----------------------------------------------------------------------------
// setup the library and run all tests
// ----------------------------------------------------------------------------

void init_lib_and_run_tests()
{
    // set up the FT81X lib
    // this initialisers the spi module and the dma channels
    FT81X_Handle handle;
    memset(&handle, 0, sizeof(FT81X_Handle));
    ft81x_result res = ft81x_initialise(&handle, DL_BUFFER_SIZE, dl_buffer);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_initialise failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }
    res = ft81x_configure(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_configure failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // ----------------------------------------------------------
    // enable the backlight
    // you can do this after displaying the first screen
    // to avoid the black screen)
    // ----------------------------------------------------------
    res = ft81x_backlight(&handle, FT81X_BACKLIGHT_LEVEL_MAX);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("ft81x_backlight failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // draws some images, and primitives
    // all directly through the DL
    res = test1(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test1 failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // tests using the co-proc
    res = test2(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test2 failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    // touch stuff
    res = test3(&handle);
    if (res != FT81X_RESULT_OK)
    {
        DbgConsole_Printf("test3 failed with %u\n", res);
        ft81x_cleanup(&handle);
        return;
    }

    DbgConsole_Printf("sreen_test completed successfully\n");

    ft81x_cleanup(&handle);
}
