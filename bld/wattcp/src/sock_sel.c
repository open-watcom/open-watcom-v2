#include <stdio.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pctcp.h"

/*
 * sock_sselect - returns one of several constants indicating
 *                SOCKESTABLISHED - tcp connection has been established
 *                SOCKDATAREAY    - tcp/udp data ready for reading
 *                SOCKCLOSED      - socket has been closed
 */

int sock_sselect (const sock_type *sk, int waitstate)
{
    /* are we connected ?
     */
    if (waitstate == SOCKDATAREADY && sk->tcp.rx_datalen)
        return (SOCKDATAREADY);

    if (sk->u.ip_type == 0)
        return (SOCKCLOSED);

    if (waitstate == SOCKESTABLISHED) {
        if (sk->u.ip_type == UDP_PROTO      ||
            sk->tcp.state == tcp_StateESTAB ||
            sk->tcp.state == tcp_StateESTCL ||
            sk->tcp.state == tcp_StateCLOSWT) {
            return (SOCKESTABLISHED);
        }
    }
    return (0);
}

