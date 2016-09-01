This library is designed talk to the FT81X series of chips.
It supports multiple boards, platforms and displays.

Currently supported boards:
    FRDM-K66F (connected to a ME810A-HV35R eval board)
        note: currently only supports one method of connecting the two boards.
              see boards/frdm_k66f.h

Currently supported displays:
    ILI9488 (found on the ME810A-HV35R eval board)

Currently supported platforms:
    NXP Kinetis K66
        note: requires FreeRTOS
              requires edma and the dmamux to have been configured before initialise() is called.
              requires platform_user_data to be a struct of type FT81X_NXP_kinetis_k66_user_data (see platforms/nxp_kinetis_k66.h)


To get started
    copy the ft81x_config.h_sample to your project dir and rename to ft81x_config.h.
    Set the desired board in the config file.
    Set the desired SPI baud rates in the config file.
    Add ft81x_lib/include to your include path
    Add ft81x_lib to your sources