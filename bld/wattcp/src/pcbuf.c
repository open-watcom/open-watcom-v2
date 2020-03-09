#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "language.h"
#include "misc.h"
#include "pctcp.h"
#include "pcdbug.h"
#include "pcbuf.h"


int sock_rbsize (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (sizeof(sk->raw.ip) + sizeof(sk->raw.data));
    case VALID_UDP:
    case VALID_TCP:
        return (sk->u.rx_maxdatalen);
    }
    return (0);
}

int sock_rbused (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (sk->raw.used ? intel16 (sk->raw.ip.length) : 0);
    case VALID_UDP:
    case VALID_TCP:
        return (sk->u.rx_datalen);
    }
    return (0);
}

int sock_rbleft (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (sk->raw.used ? 0 : sizeof(sk->raw.ip) + sizeof(sk->raw.data));
    case VALID_UDP:
    case VALID_TCP:
        return (sk->u.rx_maxdatalen - sk->u.rx_datalen);
    }
    return (0);
}

int sock_tbsize (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (mtu);
    case VALID_TCP:
        return (sk->tcp.tx_maxdatalen);
    case VALID_UDP:
        return (mtu - sizeof(in_Header) - sizeof(udp_Header));
    }
    return (0);
}

int sock_tbleft (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (mtu);
    case VALID_TCP:
        return (sk->tcp.tx_maxdatalen - sk->tcp.tx_datalen);
    case VALID_UDP:
        return (mtu - sizeof(in_Header) - sizeof(udp_Header));
    }
    return (0);
}

int sock_tbused (const sock_type *sk)
{
    if (_chk_socket(sk) == VALID_TCP)
        return (sk->tcp.tx_datalen);
    return (0);
}

/*
 * chk_socket - determine whether a real socket or not
 */
int _chk_socket (const sock_type *sk)
{
    if (sk != NULL) {
        switch (sk->u.ip_type) {
        case UDP_PROTO:
            return (VALID_UDP);
        case TCP_PROTO:
            if (sk->tcp.state <= tcp_StateCLOSED)
                return (VALID_TCP);
            break;
        case IP_TYPE:
            return (VALID_IP);
        }
    }
    return (0);
}

const char *sockerr (const sock_type *sk)
{
    if (sk != NULL) {
        switch (sk->u.ip_type) {
        case UDP_PROTO:
        case TCP_PROTO:
            if (sk->u.err_msg != NULL && sk->u.err_msg[0] != '\0') {
                return (sk->u.err_msg);
            }
            break;
        }
    }
    return (NULL);
}

const char *sockstate (const sock_type *sk)
{
    switch (_chk_socket(sk)) {
    case VALID_IP:
        return (_LANG("Raw IP Socket"));
    case VALID_UDP:
        return (_LANG("UDP Socket"));
    case VALID_TCP:
        return (_LANG(tcpState[sk->tcp.state]));
    default:
        return (_LANG("Not an active socket"));
    }
}
