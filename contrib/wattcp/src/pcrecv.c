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
static int _recvdaemon (udp_Socket *s, BYTE *data, int len,
                        tcp_PseudoHeader *ph, udp_Header *udp)
{
  recv_data *r;
  recv_buf  *p;
  int i;

  switch (s->ip_type)
  {
    case UDP_PROTO:
         r = (recv_data*)s->rdata;
         p = (recv_buf*) r->recv_bufs;
         if (r->recv_sig != RECV_USED)
         {
           outsnl (_LANG("ERROR: udp recv data conflict"));
           return (0);
         }
         /* find an unused buffer
          */
         for (i = 0; i < r->recv_bufnum; i++, p++)
             switch (p->buf_sig)
             {
               case RECV_USED:
                    break;
               case RECV_UNUSED:  /* take this one */
                    p->buf_sig     = RECV_USED;
                    p->buf_hisip   = ph->src;
                    p->buf_hisport = udp->srcPort;
                    len = min (len, sizeof(p->buf_data));
                    if (len > 0)
                    {
                      memcpy (p->buf_data, data, len);
                      p->buf_len = len;
                    }
                    else
                      p->buf_len = -1;  /* a 0-byte probe */
                    return (0);
               default:
                    outsnl (_LANG("ERROR: sock_recv_daemon data err"));
                    return (0);
             }
         return (0);

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         {
           tcp_Socket *t = (tcp_Socket*) s;

           r = (recv_data*) t->rdata;
           if (len > t->max_seg)
              return (0);

           if (r->recv_sig != RECV_USED)
           {
             outsnl (_LANG("ERROR: tcp recv data conflict"));
             return (0);
           }
           /* stick it on the end if you can
            */
           i = t->maxrdatalen - t->rdatalen;
           if (i > 1)
           {
             /* we can accept some of this */
             if (len > i)
                 len = i;
             if (len > 0)
                memcpy (&r->recv_bufs[s->rdatalen], data, len);
             s->rdatalen += len;
             return (len);
           }
           return (0);   /* didn't take none */
         }
#endif
  }
  return (0);
}


int sock_recv_used (udp_Socket *s)
{
  recv_data  *r;
  recv_buf   *p;

  int i, len;

  switch (_chk_socket((sock_type*)s))
  {
    case VALID_UDP:
         r = (recv_data*)s->rdata;
         p = (recv_buf*) r->recv_bufs;
         if (r->recv_sig != RECV_USED)
            return (-1);

         for (i = len = 0; i < r->recv_bufnum; i++, p++)
             if (p->buf_sig == RECV_USED)
                len += abs (p->buf_len);
         return (len);

#if !defined(USE_UDP_ONLY)
    case VALID_TCP:
         {
           tcp_Socket *t = (tcp_Socket*)s;

           r = (recv_data*) t->rdata;
           if (r->recv_sig != RECV_USED)
              return (-1);
           return (t->rdatalen);
         }
#endif
  }
  return (0);
}


int sock_recv_init (udp_Socket *s, void *space, int len)
{
  recv_buf  *p = space;
  recv_data *r = (recv_data*) &s->rddata[0];
  int i;

  memset (p, 0, len);                /* clear data area */
  memset (r, 0, sizeof(s->rddata));  /* clear table */

  s->protoHandler = (ProtoHandler) _recvdaemon;
  r->recv_sig     = RECV_USED;
  r->recv_bufs    = (BYTE*) p;
  r->recv_bufnum  = len / sizeof(recv_buf);
  if (s->ip_type == UDP_PROTO)
     for (i = 0; i < r->recv_bufnum; i++, p++)
         p->buf_sig = RECV_UNUSED;
  return (0);
}

#if defined(USE_BSD_FUNC)

int sock_recv_from (udp_Socket *s, DWORD *hisip, WORD *hisport,
                    char *buffer, int len, int peek)
{
  tcp_Socket *t;
  recv_buf   *p;
  recv_data  *r = (recv_data*) s->rdata;
  int i;

  if (r->recv_sig != RECV_USED)
     return (-1);

  switch (s->ip_type)
  {
    case UDP_PROTO:
         p = (recv_buf*) r->recv_bufs;

         /* find a used buffer
          */
         for (i = 0; i < r->recv_bufnum; i++, p++)
         {
           switch (p->buf_sig)
           {
             case RECV_UNUSED:
                  break;
             case RECV_USED:
                  if (p->buf_len < 0)  /* a 0-byte probe packet */
                     len = -1;
                  else
                  {
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
         return (0);

#if !defined(USE_UDP_ONLY)
    case TCP_PROTO:
         t = (tcp_Socket*) s;
         if (len > t->rdatalen)
             len = t->rdatalen;
         if (len)
             memcpy (buffer, r->recv_bufs, len);
         return (len);
#endif
  }
  return (0);
}
#endif  /* USE_BSD_FUNC */


int sock_recv (udp_Socket *s, char *buffer, int len)
{
  recv_buf   *p;
  recv_data  *r = (recv_data*) s->rdata;
  int i;

  if (r->recv_sig != RECV_USED)
     return (-1);

  switch (s->ip_type)
  {
    case UDP_PROTO:
         p = (recv_buf*) r->recv_bufs;

         /* find a used buffer
          */
         for (i = 0; i < r->recv_bufnum; i++, p++)
         {
           switch (p->buf_sig)
           {
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
         {
           tcp_Socket *t = (tcp_Socket*) s;

           if (len > t->rdatalen)
               len = t->rdatalen;
           if (len)
              memcpy (buffer, r->recv_bufs, len);
           return (len);
         }
#endif
  }
  return (0);
}

