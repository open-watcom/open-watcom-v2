#include <stdlib.h>
#include <limits.h>
#include <sys/wtypes.h>
#include <netinet/tcp.h>

#include "wattcp.h"
#include "pcbuf.h"
#include "pcbufsiz.h"


static BYTE *resize_buf(int use_buf, BYTE *olddata, sock_size datalen, BYTE *buf, sock_size newsize)
{
    BYTE *newdata;

    if (use_buf) {
        /* required size can be hold by static buffer */
        newdata = buf;
        if (olddata != newdata) {
            /*
             * if old data is not in static buffer
             * copy it to static buffer and free old data
             */
            if (datalen > 0)
                memcpy (newdata, olddata, datalen);
            free (olddata);
        }
    } else {
        /* required size is greater then length of static buffer */
        if (olddata == buf) {
            /*
             * if old data is in static buffer
             * copy them to new allocated buffer
             */
            newdata = malloc (newsize);
            if (newdata == NULL)
                return (newdata);
            if (datalen > 0) {
                memcpy (newdata, olddata, datalen);
            }
        } else {
            /*
             * realloc doesn't need copy data
             */
            newdata = realloc (olddata, newsize);
            if (newdata == NULL) {
                return (newdata);
            }
        }
    }
    /* fill in rest of new buffer by zero */
    if (newsize > datalen) {
        memset (newdata + datalen, 0, newsize - datalen);
    }
    return (newdata);
}

/*
 * Set receive buffer size for TCP.
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
        datalen = min(sk->u.rx_datalen, size);
        buf = resize_buf((size <= ((is_tcp) ? tcp_MaxBufSize : udp_MaxBufSize)), sk->u.rx_data, datalen, sk->u.rx_buf, size);
        if (buf == NULL)
            return (-1);
        sk->u.rx_datalen = datalen;
        sk->u.rx_data = buf;
        sk->u.rx_maxdatalen = size;
#if (DOSX)
        if (is_tcp && size > TCP_MAXWIN) {
        	/*
        	 * it is original strange code and need change
        	 * I added only sanity check for TCP_MAX_WINSHIFT value
        	 * and correct check of buffer size for scaling
        	 */
            sk->tcp.tx_wscale = min (TCP_MAX_WINSHIFT, size >> 16);
        }
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
 * Set transmit buffer size for TCP Socket.
 * TCP max size accepted is 64k.
 * UDP doesn't use TX buffer
 */
int sock_send_buf (sock_type *sk, sock_size size)
{
    BYTE *buf;

    switch (sk->u.ip_type) {
    case TCP_PROTO:
        size = min (size, MAX_TCP_SEND_BUF);
        datalen = min(sk->tcp.tx_datalen, size);
        buf = resize_buf((size <= tcp_MaxTxBufSize), sk->tcp.tx_data, datalen, sk->tcp.tx_buf, size);
        if (buf == NULL)
            return (-1);
        sk->tcp.tx_datalen = datalen;
        sk->tcp.tx_data = buf;
        sk->tcp.tx_maxdatalen = size;
        break;
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
