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

// structs and macros copied from icmp.h
PACK_STRUCT_BEGIN
struct icmp_echo_hdr {
  PACK_STRUCT_FIELD(u8_t type);
  PACK_STRUCT_FIELD(u8_t code);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u16_t id);
  PACK_STRUCT_FIELD(u16_t seqno);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

#define ICMPH_TYPE(hdr) ((hdr)->type)
#define ICMPH_CODE(hdr) ((hdr)->code)

#define ICMPH_TYPE_SET(hdr, t) ((hdr)->type = (t))
#define ICMPH_CODE_SET(hdr, c) ((hdr)->code = (c))

#define ICMP_ECHO 8    /* echo */

// configuration options
#define PING_DATA_SIZE  32
#define PING_ID         0xAFAF

/* Prepare a echo ICMP request */
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len, u16_t seq)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id = PING_ID;
    iecho->seqno = htons(seq);

    /* fill the additional data buffer with some data */
    for (i = 0; i < data_len; i++)
    {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}

/* Ping using the socket ip */
static err_t ping_send(int s, ip_addr_t *addr, u16_t seq)
{
    int err;
    struct icmp_echo_hdr *iecho;
    struct sockaddr_in to;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
    LWIP_ASSERT("ping_size is too big", ping_size <= 0xffff);

    iecho = (struct icmp_echo_hdr *)mem_malloc((mem_size_t)ping_size);
    if (!iecho)
    {
        return ERR_MEM;
    }

    ping_prepare_echo(iecho, (u16_t)ping_size, seq);

    to.sin_len = sizeof(to);
    to.sin_family = AF_INET;
    inet_addr_from_ipaddr(&to.sin_addr, addr);

    err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr *)&to, sizeof(to));

    mem_free(iecho);

    return (err ? ERR_OK : ERR_VAL);
}

static void ping_recv(int s, u16_t seq, u32_t send_time)
{
    char buf[64];
    int fromlen, len;
    struct sockaddr_in from;
    struct ip_hdr *iphdr;
    struct icmp_echo_hdr *iecho;
    fromlen = sizeof(from);

    while ((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen)) > 0)
    {
        if (len >= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)))
        {
            ip_addr_t fromaddr;
            ip_addr_t *fromaddr_ptr = &fromaddr;
            inet_addr_to_ipaddr(fromaddr_ptr, &from.sin_addr);
            printf("ping: recv %u ms\n", (sys_now() - send_time));

            iphdr = (struct ip_hdr *)buf;
            iecho = (struct icmp_echo_hdr *)(buf + (IPH_HL(iphdr) * 4));
            if ((iecho->id == PING_ID) && (iecho->seqno == htons(seq)))
            {
                printf("ping: received OK\n");
                return;
            }
            else
            {
                printf("ping: drop\n");
            }
        }
    }

    if (len == 0)
    {
        printf("ping: recv - %u ms - timeout\n", (sys_now() - send_time));
    }

    printf("ping: failed?\n");
}

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

    // start the echo test using raw sockets
    int s;
    int timeout = 1000; // 1s
    ip_addr_t ping_target = our_gw;
    ip_addr_t *ping_target_ptr = &ping_target;
    u16_t seq = 0;

    if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0)
    {
        printf("lwip_socket returned %d\n", s);
        for (;;);
    }

    lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (1)
    {
        //IP4_ADDR(ping_target_ptr, PING_TARGET0, PING_TARGET1, PING_TARGET2, PING_TARGET3);

        if (ping_send(s, ping_target_ptr, seq) == ERR_OK)
        {
            printf("ping: send\n");
            ping_recv(s, seq, sys_now());
        }
        else
        {
            printf("ping: send - error\n");
        }

        seq++;
        sys_msleep(1000);   // 1s
    }
}
