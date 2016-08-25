# kinetis_test_projects
A collection of test / proof of concept projects for Kinetis based chips

Requirements:
    Kinetis design studio v3.2.0 or later

To get started.
    Open KDS
    Import projects. Select the kinetis_test_projects/ folder.
    Click the search for nested projects check box.
    Import

screen_test:
    This test project is designed to evaluate the FT810 Embedded Video Engine,
    by connecting the FRDM-K66F eval board to the ME810A-HV35R development board.
    Connections:
        Eval board header J1        FRDM-K66F
        1) SCK      (SPI)           J2, pin 12  (SPI0_SCK, PTD1)
        2) CS0#     (SPI)           J2, pin 6   (SPI0_PCS0, PTD0)
        3) MISO     (SPI)           J2, pin 10  (SPI0_SIN, PTD3)
        4) MOSI     (SPI)           J2, pin 8   (SPI0_SOUT, PTD2)
        5) IO3                      NC
        6) IO2                      NC
        7) DCX      (GPIO)          J2, pin 4   (PTB19)
        8) CS1#     (SPI)           J1, pin 2    (SPI0_PCS1, PTC3)
        9) 3.3V     (POW)           J3, pin 4
        10) 5V                      NC
        11) gnd     (POW)           J3, pin 12
        12) gnd     (POW)           J3, pin 14
        13) PD#     (GPIO)          J2, pin 2   (PTB18)
        14) INT#    (GPIO)          J1, pin 4   (PTC4)
        15) AUDL                    NC
        16) DISP    (GPIO)          J1, pin 6   (PTC16)
