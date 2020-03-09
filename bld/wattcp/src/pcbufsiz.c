#include <stdlib.h>
#include <limits.h>
#include <sys/wtypes.h>
#include <netinet/tcp.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "pcbuf.h"
#include "pcbufsiz.h"



/*
 * Set receive buffer size for TCP/UDP.
 * TCP max size accepted is 1MByte = 64k * (2 << TCP_MAX_WINSHIFT) for 32-bit
 * or 64kB for 16-bit.
 * UDP max size accepted is 64k.
 */
int sock_recv_buf (sock_type *sk, sock_size size)
{
    BYTE *buf;
    sock_size datalen;
    int is_tcp;

    is_tcp = 0;
    switch (sk->u.ip_type) {
    case TCP_PROTO:
        is_tcp = 1;
        /* fall through */
    case UDP_PROTO:
        size = min (size, (is_tcp) ? MAX_TCP_RECV_BUF : MAX_UDP_RECV_BUF);  /* TCP-64kB/1MB, UDP-64kB */
        buf  = realloc (sk->u.rx_data, size);
        if (buf == NULL) {
            return (-1);
        }

        /* Copy the data to new buffer. Data might be overlapping
         * hence using movmem(). Also clear rest of buffer.
         */
        datalen = sk->u.rx_datalen;
        if (datalen > 0) {
            datalen = min (size, datalen);
            movmem (sk->u.rx_data, buf, datalen);
            if (size > datalen) {
                memset (sk->u.rx_data + datalen, 0, size - datalen);
            }
            sk->u.rx_datalen = datalen;
        }
        sk->u.rx_data       = buf;
        sk->u.rx_maxdatalen = size;
#if (DOSX)
        if (is_tcp && size > 64*1024)
            sk->tcp.tx_wscale = size >> 16;
#endif
        break;
    default:
        /* raw to-do !! */
        break;
    }
    return (0);
}

/*
 *  Sets new buffer for Rx-data (for udp/tcp).
 *  Should be used only when current buffer is empty,
 *  i.e. sock_rbused() returns 0
 */
int sock_setbuf (sock_type *sk, BYTE *rx_buf, sock_size rx_len)
{
    int is_tcp;

    is_tcp = 0;
    switch (_chk_socket(sk)) {
    case VALID_TCP:
        is_tcp = 1;
        /* fall through */
    case VALID_UDP:
        if (rx_len == 0 || rx_buf == NULL) {
            sk->u.rx_data = sk->u.rx_buf;
            sk->u.rx_maxdatalen = (is_tcp) ? tcp_MaxBufSize : udp_MaxBufSize;
        } else {
            sk->u.rx_data = rx_buf;
            sk->u.rx_maxdatalen = min (rx_len, USHRT_MAX);
            memset (rx_buf, 0, sk->u.rx_maxdatalen);
        }
        return (sk->u.rx_maxdatalen);
    }
    return (0);  /* Raw-sockets use fixed buffer */
}

#ifdef NOT_YET
/*
 * Set transmit buffer size for Socket.
 * TCP max size accepted is 64k.
 * UDP doesn't use TX buffer
 */
int sock_send_buf (sock_type *sk, sock_size size)
{
    BYTE *buf;
    sock_size datalen;

    switch (sk->u.ip_type) {
    case TCP_PROTO:
      {
        size = min (size, MAX_TCP_SEND_BUF);
        buf  = realloc (sk->tcp.tx_data, size);
        if (buf == NULL) {
            return (-1);
        }

        /* Copy current data to new buffer. Data might be overlapping
         * hence using movmem().
         */
        datalen = sk->tcp.tx_datalen;
        if (datalen > 0) {
            datalen = min (size, datalen);
            movmem (sk->tcp.tx_data, buf, datalen);
            sk->tcp.tx_datalen = datalen;
        }
        sk->tcp.tx_data       = buf;
        sk->tcp.tx_maxdatalen = size;
        break;
      }
    case UDP_PROTO:
        /* UDP doesn't use TX-buffer */
        break;
    default:
        /* raw to-do !! */
        break;
    }
    return (0);
}
#endif
