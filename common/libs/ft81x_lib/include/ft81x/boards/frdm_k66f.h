#ifndef __FRDM_K66F_H
#define __FRDM_K66F_H

#include "fsl_common.h"

// this board requires the FT81X_PLATFORM_NXP_KINETIS_K66 platform
#define FT81X_PLATFORM FT81X_PLATFORM_NXP_KINETIS_K66

// note this board is an eval board with IO headers
// that can be used to connect to a board with a FT81X and a display
// Therefore the below pin definitions is only one of the ways that
// the frdm_k66f could be connected to an FT81X and a display.
// change this as desired

#define ME810A_HV35R_CONNECTION_TYPE_1

#ifdef ME810A_HV35R_CONNECTION_TYPE_1

// the below pinout is how I connect the FRDM_K66F
// to a ME810A-HV35R eval board, which has a ILI9488 display

#define FT81X_DISPLAY FT81X_DISPLAY_ILI9488

// ME810A-HV35R header J1      FRDM-K66F
// 1) SCK      (SPI)           J2, pin 12  (SPI0_SCK, PTD1)
// 2) CS0#     (SPI)           J2, pin 6   (SPI0_PCS0, PTD0)
// 3) MISO     (SPI)           J2, pin 10  (SPI0_SIN, PTD3)
// 4) MOSI     (SPI)           J2, pin 8   (SPI0_SOUT, PTD2)
// 5) IO3                      NC
// 6) IO2                      NC
// 7) DCX      (GPIO)          J2, pin 4   (PTB19)
// 8) CS1#     (SPI)           J1, pin 2   (SPI0_PCS1, PTC3)
// 9) 3.3V     (POW)           J3, pin 4
// 10) 5V                      NC
// 11) gnd     (POW)           J3, pin 12
// 12) gnd     (POW)           J3, pin 14
// 13) PD#     (GPIO)          J2, pin 2   (PTB18)
// 14) INT#    (GPIO)          J1, pin 4   (PTC4)
// 15) AUDL                    NC
// 16) DISP    (GPIO)          J1, pin 6   (PTC16)

#define FT81X_BOARD_GPU_NOT_PD_PIN_PORT     PTB
#define FT81X_BOARD_GPU_NOT_PD_PIN_NUM      18

#define FT81X_BOARD_GPU_SPI_MODULE          SPI0
#define FT81X_BOARD_DISPLAY_SPI_MODULE      SPI0

#define FT81X_BOARD_DMAMUX_RX_SRC           (uint8_t)kDmaRequestMux0SPI0Rx
#define FT81X_BOARD_DMAMUX_TX_SRC           (uint8_t)kDmaRequestMux0SPI0Tx

#define FT81X_BOARD_SPI_CLK_SRC             DSPI0_CLK_SRC

// note for init we use kDSPI_CtarX
// but in the tfer we use kDSPI_MasterCtarX
#define FT81X_BOARD_GPU_SPI_INIT_CTAR       kDSPI_Ctar0
#define FT81X_BOARD_DISPLAY_SPI_INIT_CTAR   kDSPI_Ctar1
#define FT81X_BOARD_GPU_SPI_TFER_CTAR       kDSPI_MasterCtar0
#define FT81X_BOARD_DISPLAY_SPI_TFER_CTAR   kDSPI_MasterCtar1

// note for init we use kDSPI_PcsX
// but in the tfer we use kDSPI_MasterPcsX
#define FT81X_BOARD_GPU_SPI_INIT_CS         kDSPI_Pcs0
#define FT81X_BOARD_DISPLAY_SPI_INIT_CS     kDSPI_Pcs1
#define FT81X_BOARD_GPU_SPI_TFER_CS         kDSPI_MasterPcs0
#define FT81X_BOARD_DISPLAY_SPI_TFER_CS     kDSPI_MasterPcs1

#else

#error FRDM_K66F No connection type defined

#endif

#endif
