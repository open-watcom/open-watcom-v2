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
static int _recvdaemon (sock_type *s, BYTE *data, int len,
                        tcp_PseudoHeader *ph, udp_Header *udp)
{
    recv_data *r;
    recv_buf  *p;
    int i;

    switch (s->u.ip_type) {
    case UDP_PROTO:
        r = (recv_data*)s->udp.rdata;
        if (r->recv_sig != RECV_USED) {
            outsnl (_LANG("ERROR: udp recv data conflict"));
            break;
        }
        /* find an unused buffer
         */
        p = (recv_buf*) r->recv_bufs;
        for (i = 0; i < r->recv_bufnum; i++, p++) {
            switch (p->buf_sig) {
            case RECV_USED:
                break;
            case RECV_UNUSED:  /* take this one */
                p->buf_sig     = RECV_USED;
                p->buf_hisip   = ph->src;
                p->buf_hisport = udp->srcPort;
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

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        r = (recv_data*) s->tcp.rdata;
        if (len > s->tcp.max_seg)
            break;

        if (r->recv_sig != RECV_USED) {
            outsnl (_LANG("ERROR: tcp recv data conflict"));
            break;
        }
        /* stick it on the end if you can
         */
        i = s->tcp.maxrdatalen - s->tcp.rdatalen;
        if (i > 1) {
            /* we can accept some of this */
            if (len > i)
                len = i;
            if (len > 0)
                memcpy (&r->recv_bufs[s->tcp.rdatalen], data, len);
            s->tcp.rdatalen += len;
            return (len);
        }
        break;      /* didn't take none */
#endif
    }
    return (0);
}


int sock_recv_used (sock_type *s)
{
    recv_data  *r;
    recv_buf   *p;
    int i, len;

    switch (_chk_socket(s)) {
    case VALID_UDP:
        r = (recv_data *) s->udp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);

        p = (recv_buf *) r->recv_bufs;
        for (i = len = 0; i < r->recv_bufnum; i++, p++) {
            if (p->buf_sig == RECV_USED) {
                len += abs (p->buf_len);
            }
        }
        return (len);

#if !defined(USE_UDP_ONLY)
    case VALID_TCP:
        r = (recv_data *) s->tcp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);
        return (s->tcp.rdatalen);
#endif
    }
    return (0);
}


int sock_recv_init (sock_type *s, char *buffer, int len)
{
    recv_buf  *p;
    recv_data *r;
    int i;

    memset (buffer, 0, len);                /* clear data area */
    switch (s->u.ip_type) {
    case UDP_PROTO:
    case TCP_PROTO:
        s->u.protoHandler = _recvdaemon;

        r = (recv_data*) s->u.rddata;
        memset (r, 0, sizeof(s->u.rddata)); /* clear table */
        r->recv_sig     = RECV_USED;
        r->recv_bufs    = (BYTE*) buffer;
        r->recv_bufnum  = len / sizeof(recv_buf);
        if (s->u.ip_type == UDP_PROTO) {
            p = (recv_buf *)buffer;
            for (i = 0; i < r->recv_bufnum; i++, p++) {
                p->buf_sig = RECV_UNUSED;
            }
        }
        break;
    }
    return (0);
}

#if defined(USE_BSD_FUNC)

int sock_recv_from (sock_type *s, DWORD *hisip, WORD *hisport,
                    char *buffer, int len, int peek)
{
    recv_buf   *p;
    recv_data  *r;
    int i;

    switch (s->u.ip_type) {
    case UDP_PROTO:
        r = (recv_data *) s->udp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);

        /* find a used buffer
         */
        p = (recv_buf*) r->recv_bufs;
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
                if (hisip)
                    *hisip = p->buf_hisip;
                if (hisport)
                    *hisport = p->buf_hisport;
                if (!peek)
                    p->buf_sig = RECV_UNUSED;
                return (len);
            default:
                outsnl (_LANG("ERROR: sock_recv_init data err"));
                return (0);
            }
        }
        break;

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
        r = (recv_data *) s->tcp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);

        if (len > s->tcp.rdatalen)
            len = s->tcp.rdatalen;
        if (len)
            memcpy (buffer, r->recv_bufs, len);
        return (len);
#endif
    }
    return (0);
}
#endif  /* USE_BSD_FUNC */


int sock_recv (sock_type *s, char *buffer, int len)
{
    recv_buf   *p;
    recv_data  *r;
    int i;

    switch (s->u.ip_type) {
    case UDP_PROTO:
        r = (recv_data*) s->udp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);

        /* find a used buffer
         */
        p = (recv_buf*) r->recv_bufs;
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
                outsnl (_LANG("ERROR: sock_recv_init data err"));
                return (0);
            }
        }
        return (0);

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO :
        r = (recv_data*) s->tcp.rdata;
        if (r->recv_sig != RECV_USED)
            return (-1);

        if (len > s->tcp.rdatalen)
            len = s->tcp.rdatalen;
        if (len)
            memcpy (buffer, r->recv_bufs, len);
        return (len);
#endif
  }
  return (0);
}

