/*
 *  Alternative socket receive handlers (see Waterloo manual)
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "pcbuf.h"
#include "pcrecv.h"


/*
 *  _recvdaemon - gets upcalled when data arrives
 */
static int _recvdaemon (sock_type *sk, BYTE *data, int len,
                        tcp_PseudoHeader *tcp_phdr, udp_Header *udp_hdr)
{
    recv_data *r;
    recv_buf  *p;
    int i;

    switch (sk->u.ip_type) {
    case UDP_PROTO:
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
#endif
        r = (recv_data *)sk->u.rx_data;
        if (r->recv_sig != RECV_USED) {
#if !defined(USE_UDP_ONLY)
            if (sk->u.ip_type == TCP_PROTO) {
                outsnl (_LANG("ERROR: tcp recv data conflict"));
                break;
            }
#endif
            outsnl (_LANG("ERROR: udp recv data conflict"));
            break;
        }
#if !defined(USE_UDP_ONLY)
        if (sk->u.ip_type == TCP_PROTO) {
            if (len > sk->tcp.max_seg)
                break;

            /* stick it on the end if you can
             */
            i = sk->tcp.rx_maxdatalen - sk->tcp.rx_datalen;
            if (i > 1) {
                /* we can accept some of this */
                if (len > i)
                    len = i;
                if (len > 0)
                    memcpy (&r->recv_bufs[sk->tcp.rx_datalen], data, len);
                sk->tcp.rx_datalen += len;
                return (len);
            }
            break;      /* didn't take none */
        }
#endif
        /* find an unused buffer
         */
        p = (recv_buf *) r->recv_bufs;
        for (i = 0; i < r->recv_bufnum; i++, p++) {
            switch (p->buf_sig) {
            case RECV_USED:
                break;
            case RECV_UNUSED:  /* take this one */
                p->buf_sig     = RECV_USED;
                p->buf_hisip   = tcp_phdr->src;
                p->buf_hisport = udp_hdr->srcPort;
                len = min (len, sizeof(p->buf_data));
                if (len > 0) {
                    memcpy (p->buf_data, data, len);
                    p->buf_len = len;
                } else {
                    p->buf_len = -1;  /* a 0-byte probe */
                }
                return (0);
            default:
                outsnl (_LANG("ERROR: sock_recv_daemon data err"));
                return (0);
            }
        }
        break;
    }
    return (0);
}


int sock_recv_used (sock_type *sk)
{
    recv_data  *r;
    recv_buf   *p;
    int i, len;

    switch (_chk_socket(sk)) {
    case VALID_UDP:
#if !defined(USE_UDP_ONLY)
    case VALID_TCP:
#endif
        r = (recv_data *) sk->u.rx_data;
        if (r->recv_sig != RECV_USED)
            return (-1);

#if !defined(USE_UDP_ONLY)
        if (sk->u.ip_type == TCP_PROTO) {
            return (sk->tcp.rx_datalen);
        }
#endif
        p = (recv_buf *) r->recv_bufs;
        for (i = len = 0; i < r->recv_bufnum; i++, p++) {
            if (p->buf_sig == RECV_USED) {
                len += abs (p->buf_len);
            }
        }
        return (len);
    }
    return (0);
}


int sock_recv_init (sock_type *sk, char *buffer, int len)
{
    recv_buf  *p;
    recv_data *r;
    int i;

    memset (buffer, 0, len);                /* clear data area */
    switch (sk->u.ip_type) {
    case UDP_PROTO:
        sk->udp.protoHandler = _recvdaemon;
        r = (recv_data *) sk->udp.rx_buf;
        memset (r, 0, udp_MaxBufSize); /* clear table */
        r->recv_sig     = RECV_USED;
        r->recv_bufs    = (BYTE *) buffer;
        r->recv_bufnum  = len / sizeof(recv_buf);
        p = (recv_buf *)buffer;
        for (i = 0; i < r->recv_bufnum; i++, p++) {
            p->buf_sig = RECV_UNUSED;
        }
        break;
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        sk->tcp.protoHandler = _recvdaemon;
        r = (recv_data *) sk->tcp.rx_buf;
        memset (r, 0, tcp_MaxBufSize); /* clear table */
        r->recv_sig     = RECV_USED;
        r->recv_bufs    = (BYTE *) buffer;
        r->recv_bufnum  = len / sizeof(recv_buf);
        break;
#endif
    }
    return (0);
}

#if defined(USE_BSD_FUNC)

int sock_recv_from (sock_type *sk, DWORD *hisip, WORD *hisport,
                    char *buffer, int len, int peek)
{
    recv_buf   *p;
    recv_data  *r;
    int i;

    switch (sk->u.ip_type) {
    case UDP_PROTO:
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
#endif
        r = (recv_data *) sk->u.rx_data;
        if (r->recv_sig != RECV_USED)
            return (-1);

#if !defined(USE_UDP_ONLY)
        if (sk->u.ip_type == TCP_PROTO) {
            if (len > sk->tcp.rx_datalen)
                len = sk->tcp.rx_datalen;
            if (len)
                memcpy (buffer, r->recv_bufs, len);
            return (len);
        }
#endif
        /* find a used buffer
         */
        p = (recv_buf *) r->recv_bufs;
        for (i = 0; i < r->recv_bufnum; i++, p++) {
            switch (p->buf_sig) {
            case RECV_UNUSED:
                break;
            case RECV_USED:
                if (p->buf_len < 0) {   /* a 0-byte probe packet */
                    len = -1;
                } else {
                    len = min (p->buf_len, len);
                    memcpy (buffer, p->buf_data, len);
                }
                if (hisip != NULL)
                    *hisip = p->buf_hisip;
                if (hisport != NULL)
                    *hisport = p->buf_hisport;
                if (!peek)
                    p->buf_sig = RECV_UNUSED;
                return (len);
            default:
                outsnl (_LANG("ERROR: sock_recv_from data err"));
                return (0);
            }
        }
        break;
    }
    return (0);
}
#endif  /* USE_BSD_FUNC */


int sock_recv (sock_type *sk, char *buffer, int len)
{
    recv_buf   *p;
    recv_data  *r;
    int i;

    switch (sk->u.ip_type) {
    case UDP_PROTO:
#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
#endif
        r = (recv_data *) sk->u.rx_data;
        if (r->recv_sig != RECV_USED)
            return (-1);

#if !defined(USE_UDP_ONLY)
        if (sk->u.ip_type == TCP_PROTO) {
            if (len > sk->tcp.rx_datalen)
                len = sk->tcp.rx_datalen;
            if (len)
                memcpy (buffer, r->recv_bufs, len);
            return (len);
        }
#endif
        /* find a used buffer
         */
        p = (recv_buf *) r->recv_bufs;
        for (i = 0; i < r->recv_bufnum; i++, p++) {
            switch (p->buf_sig) {
            case RECV_UNUSED:
                break;
            case RECV_USED:
                if (len > p->buf_len)
                    len = p->buf_len;
                if (len > 0)
                    memcpy (buffer, p->buf_data, len);
                p->buf_sig = RECV_UNUSED;
                return (len);
            default:
                outsnl (_LANG("ERROR: sock_recv data err"));
                return (0);
            }
        }
        break;
    }
    return (0);
}

