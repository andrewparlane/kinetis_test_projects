/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

// LWIP includes
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/sockets.h"
#include "lwip/mem.h"
#include "ethernetif.h"

int main(void)
{
    // Init pins, clocks and default console
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    // disable the MPU ?? why
    MPU_Type *base = MPU;
    base->CESR &= ~MPU_CESR_VLD_MASK;

    // set the RMII clock source
    SIM->SOPT2 |= SIM_SOPT2_RMIISRC_MASK;

    // start the task scheduler
    vTaskStartScheduler();

    // initialise LWIP
    ip_addr_t our_ipaddr;
    ip_addr_t our_netmask;
    ip_addr_t our_gw;
    struct netif eth0;

    IP4_ADDR(&our_ipaddr, 192, 168, 1, 200);
    IP4_ADDR(&our_netmask, 255, 255, 255, 0);
    IP4_ADDR(&our_gw, 0, 0, 0, 0);

    tcpip_init(NULL, NULL);
    netif_add(&eth0, &our_ipaddr, &our_netmask, &our_gw, NULL, ethernetif_init, tcpip_input);
    netif_set_default(&eth0);
    netif_set_up(&eth0);

    while(1);
}
