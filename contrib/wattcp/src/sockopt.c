/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 *   0.6 : Apr 07, 2000 : Added multicast options IP_ADD_MEMBERSHIP and
 *                        IP_DROP_MEMBERSHIP.
 *                        Contributed by Vlad Erochine <vlad@paragon.ru>
 *   0.7 : Jun 06, 2000 : Added support for SO_SNDLOWAT and SO_RCVLOWAT
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

static int set_sol_opt (Socket *s, int opt, const void *val, int len);
static int set_raw_opt (Socket *s, int opt, const void *val, int len);
static int get_sol_opt (Socket *s, int opt, void *val, int *len);
static int get_raw_opt (Socket *s, int opt, void *val, int *len);

static int set_tcp_opt (tcp_Socket *tcp, int opt, const void *val, int len);
static int set_udp_opt (udp_Socket *udp, int opt, const void *val, int len);
static int get_tcp_opt (tcp_Socket *tcp, int opt, void *val, int *len);
static int get_udp_opt (udp_Socket *udp, int opt, void *val, int *len);

static int udp_rx_buf  (udp_Socket *udp, unsigned size);
static int tcp_rx_buf  (tcp_Socket *tcp, unsigned size);
static int raw_rx_buf  (raw_Socket *raw, unsigned size);
static int udp_tx_buf  (udp_Socket *udp, unsigned size);
static int tcp_tx_buf  (tcp_Socket *tcp, unsigned size);
static int raw_tx_buf  (raw_Socket *raw, unsigned size);
static int set_tx_lowat (Socket *s, unsigned  size);
static int set_rx_lowat (Socket *s, unsigned  size);
static int get_tx_lowat (const Socket *s, unsigned *size);
static int get_rx_lowat (const Socket *s, unsigned *size);

#if defined(USE_DEBUG)
static const char *sockopt_name (int option, int level);

static __inline void do_error (void)
{
  char *err = strerror_s (errno_s);
  char *par = strchr (err, '(');

  if (!par)
     _sock_debugf (NULL, " errno %d", errno_s);
  else
  {
    char *eol = strrchr (par, '\n');
    if (eol)
       *eol = '\0';
    _sock_debugf (NULL, " %s", par);
  }
}
#endif

int setsockopt (int s, int level, int option, const void *optval, int optlen)
{
  Socket *socket = _socklist_find (s);
  int     rc;

  SOCK_DEBUGF ((socket, "\nsetsockopt:%d, %s", s, sockopt_name(option,level)));

  if (!socket)
  {
    if (_sock_dos_fd(s))
    {
      SOCK_DEBUGF ((NULL, ", ENOTSOCK"));
      SOCK_ERR (ENOTSOCK);
      return (-1);
    }
    SOCK_DEBUGF ((NULL, ", EBADF"));
    SOCK_ERR (EBADF);
    return (-1);
  }

  VERIFY_RW (optval, optlen);

  if ((WORD)level == SOL_SOCKET)
     rc = set_sol_opt (socket, option, optval, optlen);

  else if ((level == socket->so_proto) && (level == IPPROTO_TCP))
     rc = set_tcp_opt (socket->tcp_sock, option, optval, optlen);

  else if ((level == socket->so_proto) && (level == IPPROTO_UDP))
     rc = set_udp_opt (socket->udp_sock, option, optval, optlen);

  else if (((level == socket->so_proto) && (level == IPPROTO_IP)) ||
           ((level == socket->so_proto) && (level == IPPROTO_ICMP)))
     rc = set_raw_opt (socket, option, optval, optlen);

  else
  {
    SOCK_ERR (ENOPROTOOPT);
    rc = -1;
  }

#if defined(USE_DEBUG)
  if (rc < 0)
     do_error();
#endif

  return (rc);
}

int getsockopt (int s, int level, int option, void *optval, int *optlen)
{
  Socket *socket = _socklist_find (s);
  int     rc;

  SOCK_DEBUGF ((socket, "\ngetsockopt:%d, %s", s, sockopt_name(option,level)));

  if (!socket)
  {
    if (_sock_dos_fd(s))
    {
      SOCK_DEBUGF ((NULL, ", ENOTSOCK"));
      SOCK_ERR (ENOTSOCK);
      return (-1);
    }
    SOCK_DEBUGF ((NULL,", EBADF"));
    SOCK_ERR (EBADF);
    return (-1);
  }

  VERIFY_RW (optval, 0);
  VERIFY_RW (optlen, sizeof(u_long));

  if ((WORD)level == SOL_SOCKET)
     rc = get_sol_opt (socket, option, optval, optlen);

  else if (level == socket->so_proto == IPPROTO_TCP)
     rc = get_tcp_opt (socket->tcp_sock, option, optval, optlen);

  else if (level == socket->so_proto == IPPROTO_UDP)
     rc = get_udp_opt (socket->udp_sock, option, optval, optlen);

  else if ((level == socket->so_proto == IPPROTO_IP) ||
           (level == socket->so_proto == IPPROTO_ICMP))
     rc = get_raw_opt (socket, option, optval, optlen);

  else
  {
    SOCK_ERR (ENOPROTOOPT);
    rc = -1;
  }

#if defined(USE_DEBUG)
  if (rc < 0)
     do_error();
#endif

  return (rc);
}


static int set_sol_opt (Socket *s, int opt, const void *val, int len)
{
  struct timeval *tv   = (struct timeval*) val;
  int             on   = *(int*) val;
  unsigned        size = *(unsigned*) val;

  switch (opt)
  {
    case SO_DEBUG:
#if defined(USE_DEBUG)
         if (on)
         {
           s->so_options |= SO_DEBUG;
           _sock_dbug_on();
         }
         else
         {
           s->so_options &= ~SO_DEBUG;
           _sock_dbug_off();
         }
#endif
         break;

    case SO_ACCEPTCONN:
         if (on)
              s->so_options |=  SO_ACCEPTCONN;
         else s->so_options &= ~SO_ACCEPTCONN;
         break;

    case SO_RCVTIMEO:
         if (len != sizeof(*tv) || tv->tv_usec < 0)
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         if (tv->tv_sec == 0)        /* i.e. use system default */
              s->timeout = sock_delay;
         else s->timeout = tv->tv_sec + tv->tv_usec/1000000UL;
         break;

    case SO_SNDTIMEO:    /* Don't think we need this */
         break;

    /*
     * SO_REUSEADDR enables local address reuse, used to bind
     * multiple socks to the same port but with different ip-addr.
     */
    case SO_REUSEADDR:
         if (s->tcp_sock && s->so_proto == IPPROTO_TCP)
         {
           reuse_localport (s->tcp_sock->myport);
           return (0);
         }
         if (s->udp_sock && s->so_proto == IPPROTO_UDP)
         {
           reuse_localport (s->udp_sock->myport);
           return (0);
         }
         SOCK_ERR (ENOPROTOOPT);
         return (-1);

    /*
     * SO_REUSEPORT enables duplicate address and port bindings
     * ie, one can bind multiple socks to the same <ip_addr.port> pair
     */
/*  case SO_REUSEPORT:  missing in BSD? */
    case SO_KEEPALIVE:
    case SO_DONTROUTE:
    case SO_DONTLINGER:
    case SO_BROADCAST:
    case SO_USELOOPBACK:
    case SO_OOBINLINE:
         break;

    case SO_SNDLOWAT:
         return set_tx_lowat (s, size);

    case SO_RCVLOWAT:
         return set_rx_lowat (s, size);

    case SO_RCVBUF:
         if (size == 0)
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         if (s->udp_sock && s->so_proto == IPPROTO_UDP)
            return udp_rx_buf (s->udp_sock, size);

         if (s->tcp_sock && s->so_proto == IPPROTO_TCP)
            return tcp_rx_buf (s->tcp_sock, size);

         if (s->raw_sock)
            return raw_rx_buf (s->raw_sock, size);

         SOCK_ERR (ENOPROTOOPT);
         return (-1);

    case SO_SNDBUF:
         if (size == 0)
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         if (s->udp_sock && s->so_proto == IPPROTO_UDP)
            return udp_tx_buf (s->udp_sock, size);

         if (s->tcp_sock && s->so_proto == IPPROTO_TCP)
            return tcp_tx_buf (s->tcp_sock, size);

         if (s->raw_sock)
            return raw_tx_buf (s->raw_sock, size);

         SOCK_ERR (ENOPROTOOPT);
         return (-1);

    case SO_LINGER:
         {
           struct linger *linger = (struct linger*) val;

           if (len < sizeof(*linger))
           {
             SOCK_ERR (EINVAL);
             return (-1);
           }

           if (s->so_type != SOCK_STREAM || !s->tcp_sock)
           {
             SOCK_ERR (ENOPROTOOPT);
             return (-1);
           }

           if (linger->l_onoff == 0 && linger->l_linger == 0)
           {
             s->tcp_sock->locflags &= ~LF_LINGER;
             s->linger_time = 0;
           }
           else if (linger->l_onoff && linger->l_linger > 0)
           {
             unsigned sec = TCP_LINGERTIME;

             if (linger->l_linger < 100 * TCP_LINGERTIME)
                sec = linger->l_linger / 100;  /* in 10ms units */

             s->linger_time = sec;
             s->tcp_sock->locflags |= LF_LINGER;
           }
         }
         break;

    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}


static int get_sol_opt (Socket *s, int opt, void *val, int *len)
{
  struct timeval *tv;
  unsigned       *size = (unsigned*)val;

  switch (opt)
  {
    case SO_DEBUG:
    case SO_ACCEPTCONN:
         *(int*)val = (s->so_options & opt);
         *len = sizeof(int);
         break;

    case SO_OOBINLINE:
#if 0
         if (!s->tcp_sock || s->so_proto != IPPROTO_TCP)
         {
           SOCK_ERR (ENOPROTOOPT);
           return (-1);
         }
         if (s->so_options & SO_OOBINLINE)
              *(int*)val = urgent_data ((sock_type*)s->tcp_sock);
         else *(int*)val = 0;
         break;
#endif

    case SO_REUSEADDR:    /* to-do !! */
    case SO_KEEPALIVE:
    case SO_DONTROUTE:
    case SO_DONTLINGER:
    case SO_BROADCAST:
    case SO_USELOOPBACK:
         break;

    case SO_SNDLOWAT:
         return get_tx_lowat (s, size);

    case SO_RCVLOWAT:
         return get_rx_lowat (s, size);

    case SO_RCVBUF:
         if (s->udp_sock && s->so_proto == IPPROTO_UDP)
         {
           *(int*)val = sock_rbsize ((sock_type*)s->udp_sock);
           return (0);
         }
         if (s->tcp_sock && s->so_proto == IPPROTO_TCP)
         {
           *(int*)val = sock_rbsize ((sock_type*)s->tcp_sock);
           return (0);
         }
         if (s->raw_sock)
         {
           *(size_t*)val = sizeof (s->raw_sock->data);
           return (0);
         }
         SOCK_ERR (ENOPROTOOPT);
         return (-1);

    case SO_SNDBUF:
         if (s->udp_sock && s->so_proto == IPPROTO_UDP)
         {
           *(unsigned*)val = 0;
           return (0);
         }
         if (s->tcp_sock && s->so_proto == IPPROTO_TCP)
         {
           *(unsigned*)val = sock_tbsize ((sock_type*)s->tcp_sock);
           return (0);
         }
         if (s->raw_sock)
         {
           *(size_t*)val = sizeof (s->raw_sock->data);
           return (0);
         }
         SOCK_ERR (ENOPROTOOPT);
         return (-1);

    case SO_LINGER:
         {
           struct linger *linger = (struct linger*) val;

           if (!len || *len < sizeof(*linger))
           {
             SOCK_ERR (EINVAL);
             return (-1);
           }
           if (s->so_type != SOCK_STREAM || !s->tcp_sock)
           {
             SOCK_ERR (ENOPROTOOPT);
             return (-1);
           }
           *(size_t*)len    = sizeof(*linger);
           linger->l_onoff  = (s->tcp_sock->locflags & LF_LINGER) ? 1 : 0;
           linger->l_linger = 100 * s->linger_time;
         }
         break;

    case SO_SNDTIMEO:
         break;

    case SO_RCVTIMEO:
         if (*len < sizeof(*tv))
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         tv = (struct timeval*)val;
         if (s->timeout == 0)
         {
           tv->tv_usec = LONG_MAX;
           tv->tv_sec  = LONG_MAX;
         }
         else
         {
           tv->tv_usec = 0;
           tv->tv_sec  = s->timeout;
         }
         break;

    case SO_ERROR:
         *(int*)val    = s->so_error;
         *(size_t*)len = sizeof(s->so_error);
         s->so_error = 0;   /* !! should be do this */
         break;

    case SO_TYPE:
         *(int*)val    = s->so_type;
         *(size_t*)len = sizeof(s->so_type);
         break;

    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  SOCK_ERR (0);
  return (0);
}

/*
 * set/get TCP-layer options
 */
static int set_tcp_opt (tcp_Socket *tcp, int opt, const void *val, int len)
{
  BOOL on = *(BOOL*)val;
  long MSS;

  switch (opt)
  {
    case TCP_NODELAY:
         if (on)     /* disable Nagle's algorithm */
         {
           sock_mode ((sock_type*)tcp, TCP_MODE_NONAGLE);
           tcp->locflags |= LF_NODELAY;
         }
         else        /* turn on Nagle */
         {
           sock_mode ((sock_type*)tcp, TCP_MODE_NAGLE);
           tcp->locflags &= ~LF_NODELAY;
         }
         break;

    case TCP_MAXSEG:
         MSS = *(long*)val;
         if (MSS < 1 || MSS > MAX_WINDOW)
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         tcp->max_seg = *(int*)val;
         break;

    case TCP_NOPUSH:
         if (on)
              tcp->locflags |=  LF_NOPUSH;
         else tcp->locflags &= ~LF_NOPUSH;
         break;

    case TCP_NOOPT:
         if (on)
              tcp->locflags |=  LF_NOOPT;
         else tcp->locflags &= ~LF_NOOPT;
         break;

    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  ARGSUSED (len);
  return (0);
}

static int get_tcp_opt (tcp_Socket *tcp, int opt, void *val, int *len)
{
  switch (opt)
  {
    case TCP_NODELAY:
         if (tcp->sockmode & TCP_MODE_NONAGLE)
              *(int*)val = TCP_NODELAY;
         else *(int*)val = 0;
         *(size_t*)len = sizeof(int);
         break;

    case TCP_MAXSEG:
         *(int*)val    = tcp->max_seg;
         *(size_t*)len = sizeof(int);
         break;

    case TCP_NOPUSH:
         *(int*)val    = (tcp->locflags & LF_NOPUSH);
         *(size_t*)len = sizeof (tcp->locflags);
         break;

    case TCP_NOOPT:
         *(int*)val    = (tcp->locflags & LF_NOOPT);
         *(size_t*)len = sizeof (tcp->locflags);
         break;

    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}

/*
 * set/get UDP-layer options
 */
static int set_udp_opt (udp_Socket *udp, int opt, const void *val, int len)
{
  ARGSUSED (udp);      /* no udp option support yet */
  ARGSUSED (opt);
  ARGSUSED (val);
  ARGSUSED (len);
  SOCK_ERR (ENOPROTOOPT);
  return (-1);
}


static int get_udp_opt (udp_Socket *udp, int opt, void *val, int *len)
{
  ARGSUSED (udp);    /* no udp option support yet */
  ARGSUSED (opt);
  ARGSUSED (val);
  ARGSUSED (len);
  SOCK_ERR (ENOPROTOOPT);
  return (-1);
}

/*
 * set/get IP/ICMP-layer (raw/multicast) options
 */
static int set_raw_opt (Socket *s, int opt, const void *val, int len)
{
  BOOL on = *(BOOL*)val;
#ifdef USE_MULTICAST
  struct ip_mreq *l_pMreq;
  DWORD  ip;
#endif

  switch (opt)
  {
    case IP_OPTIONS:
         if (!s->ip_opt && (s->ip_opt = SOCK_CALLOC(sizeof(*s->ip_opt))) == NULL)
         {
           SOCK_ERR (ENOMEM);
           return (-1);
         }
         if (len == 0 && s->ip_opt)
         {
           free (s->ip_opt);
           s->ip_opt     = NULL;
           s->ip_opt_len = 0;
         }
         else
         {
           s->ip_opt_len = min (len, sizeof(*s->ip_opt));
           memcpy (&s->ip_opt->ip_opts, val, s->ip_opt_len);
         }
         break;

    case IP_HDRINCL:
         if (on)
              s->inp_flags |=  INP_HDRINCL;
         else s->inp_flags &= ~INP_HDRINCL;
         break;

    case IP_TOS:
         s->ip_tos = *(int*)val;
         if (s->tcp_sock)
             s->tcp_sock->tos = s->ip_tos;
         break;

    case IP_TTL:
         s->ip_ttl = min (1, *(int*)val);
         if (s->udp_sock)
            s->udp_sock->ttl = s->ip_ttl;
         else if (s->tcp_sock)
            s->tcp_sock->ttl = s->ip_ttl;
         break;

    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
#ifdef USE_MULTICAST
         l_pMreq = (struct ip_mreq*)val;
         if (!l_pMreq || len < sizeof(*l_pMreq))
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         ip = ntohl (l_pMreq->imr_multiaddr.s_addr);
         if (!is_multicast(ip))
         {
           SOCK_ERR (EINVAL);
           return (-1);
         }
         if (!_multicast_on)
         {
           SOCK_ERR (EADDRNOTAVAIL);
           return (-1);
         }
         if (opt == IP_ADD_MEMBERSHIP && !join_mcast_group(ip))
         {
           SOCK_ERR (ENOBUFS);        /* !!correct errno? */
           return (-1);
         }
         if (opt == IP_DROP_MEMBERSHIP && !leave_mcast_group(ip))
         {
           SOCK_ERR (EADDRNOTAVAIL);  /* !!correct errno? */
           return (-1);
         }
#endif
         break;

    case IP_RECVOPTS:
    case IP_RECVRETOPTS:
    case IP_RECVDSTADDR:
    case IP_RETOPTS:
    case IP_MULTICAST_IF:
    case IP_MULTICAST_TTL:
    case IP_MULTICAST_LOOP:
    case IP_MULTICAST_VIF:
    case IP_RSVP_ON:
    case IP_RSVP_OFF:
    case IP_RSVP_VIF_ON:
    case IP_RSVP_VIF_OFF:
    case IP_PORTRANGE:
    case IP_RECVIF:
         break;

    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}

static int get_raw_opt (Socket *s, int opt, void *val, int *len)
{
  switch (opt)
  {
    case IP_OPTIONS:
         if (s->ip_opt)
         {
           *len = s->ip_opt_len;
           memcpy (val, s->ip_opt->ip_opts, *len);
         }
         else
         {
           memset (val, 0, *len);
           *len = 0;
         }
         break;

    case IP_HDRINCL:
         *(int*)val = (s->inp_flags & INP_HDRINCL);
         break;

    case IP_TOS:
         *(int*)val = s->ip_tos;
         break;

    case IP_TTL:
         *(int*)val = s->ip_ttl;
         break;

    case IP_RECVOPTS:
    case IP_RECVRETOPTS:
    case IP_RECVDSTADDR:
    case IP_RETOPTS:
    case IP_MULTICAST_IF:
    case IP_MULTICAST_TTL:
    case IP_MULTICAST_LOOP:
    case IP_MULTICAST_VIF:
    case IP_RSVP_ON:
    case IP_RSVP_OFF:
    case IP_RSVP_VIF_ON:
    case IP_RSVP_VIF_OFF:
    case IP_PORTRANGE:
    case IP_RECVIF:
         break;

    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}

/*
 * Set receive buffer size for TCP.
 * Max size accepted is 64k * (2 << TCP_MAX_WINSHIFT) = 1MByte.
 * Or 64kB for small/large models.
 */
static int tcp_rx_buf (tcp_Socket *tcp, unsigned size)
{
  BYTE *buf;

  size = min (size, MAX_TCP_RECV_BUF);  /* 64kB/1MB */
  buf  = realloc (tcp->rdata, size);
  if (!buf)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  /* Copy the data to new buffer. Data might be overlapping
   * hence using movmem(). Also clear rest of buffer.
   */
  if (tcp->rdatalen > 0)
  {
    int len = min ((long)size, tcp->rdatalen);

    movmem (tcp->rdata, buf, len);
    if (size > tcp->rdatalen)
       memset (tcp->rdata + tcp->rdatalen, 0, size - tcp->rdatalen);
  }
  tcp->rdata       = buf;
  tcp->maxrdatalen = size;
#if (DOSX)
  if (size > 64*1024)
     tcp->send_wscale = size >> 16;
#endif
  return (0);
}

/*
 * Set transmit buffer size for TCP.
 * Max size accepted is 64k.
 */
static int tcp_tx_buf (tcp_Socket *tcp, unsigned size)
{
#ifdef NOT_YET
  BYTE *buf;

  size = min (size, MAX_TCP_SEND_BUF);
  buf  = realloc (tcp->data, size);
  if (!buf)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  /* Copy current data to new buffer. Data might be overlapping
   * hence using movmem().
   */
  if (tcp->datalen > 0)
  {
    int len = min ((long)size, udp->datalen);
    movmem (udp->data, buf, len);
  }
  udp->data       = buf;
  udp->maxdatalen = size;
#else
  ARGSUSED (tcp);
  ARGSUSED (size);
#endif
  return (0);
}

/*
 * Set receive buffer size for UDP.
 * Max size accepted is 64k.
 */
static int udp_rx_buf (udp_Socket *udp, unsigned size)
{
  BYTE *buf;

  size = min (size, MAX_UDP_RECV_BUF);
  buf  = realloc (udp->rdata, size);
  if (!buf)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  /* Copy current data to new buffer. Data might be overlapping
   * hence using movmem(). Also clear rest of buffer.
   */
  if (udp->rdatalen > 0)
  {
    int len = min ((long)size, udp->rdatalen);

    movmem (udp->rdata, buf, len);
    if (size > udp->rdatalen)
       memset (buf + udp->rdatalen, 0, size - udp->rdatalen);
  }
  udp->rdata       = buf;
  udp->maxrdatalen = size;
  return (0);
}

/*
 * Set transmit buffer sizes for UDP.
 * Max size accepted is 64k.
 */
static int udp_tx_buf (udp_Socket *udp, unsigned size)
{
#ifdef NOT_YET
  BYTE *buf;

  size = min (size, MAX_UDP_SEND_BUF);
  buf  = realloc (udp->data, size);
  if (!buf)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }
#else
  ARGSUSED (udp);
  ARGSUSED (size);
#endif
  return (0);
}


/*
 * Set receive buffer size for RAW socket
 */
static int raw_rx_buf (raw_Socket *raw, unsigned size)
{
  /* to-do !! */
  ARGSUSED (raw);
  ARGSUSED (size);
  return (0);
}

static int raw_tx_buf (raw_Socket *raw, unsigned size)
{
  /* to-do !! */
  ARGSUSED (raw);
  ARGSUSED (size);
  return (0);
}


/*
 * Set send buffer "low water marks"
 */
static int set_tx_lowat (Socket *sock, unsigned size)
{
  switch (sock->so_type)
  {
    case SOCK_STREAM:
         sock->send_lowat = min (size, MAX_TCP_SEND_BUF-1);
         break;
    case SOCK_DGRAM:
         sock->send_lowat = min (size, MAX_UDP_SEND_BUF-1);
         break;
    case SOCK_RAW:
         sock->send_lowat = min (size, sizeof(sock->raw_sock->data)-1);
         break;
    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}

/*
 * Set receive/transmit buffer "low water marks"
 */
static int set_rx_lowat (Socket *sock, unsigned size)
{
  switch (sock->so_type)
  {
    case SOCK_STREAM:
         if (sock->tcp_sock)
            sock->send_lowat = min (size, sock->tcp_sock->maxrdatalen);
         break;
    case SOCK_DGRAM:
         if (sock->udp_sock)
            sock->send_lowat = min (size, sock->udp_sock->maxrdatalen);
         break;
    case SOCK_RAW:
         sock->send_lowat = min (size, sizeof(sock->raw_sock->data));
         break;
    default:
         SOCK_ERR (ENOPROTOOPT);
         return (-1);
  }
  return (0);
}

/*
 * Get receive/transmit buffer "low water marks"
 */
static int get_tx_lowat (const Socket *sock, unsigned *size)
{
  if (sock->so_type == SOCK_STREAM ||
      sock->so_type == SOCK_DGRAM  ||
      sock->so_type == SOCK_RAW)
  {
    *size = sock->send_lowat;
    return (0);
  }
  SOCK_ERR (ENOPROTOOPT);
  return (-1);
}

static int get_rx_lowat (const Socket *sock, unsigned *size)
{
  if (sock->so_type == SOCK_STREAM ||
      sock->so_type == SOCK_DGRAM  ||
      sock->so_type == SOCK_RAW)
  {
    *size = sock->recv_lowat;
    return (0);
  }
  SOCK_ERR (ENOPROTOOPT);
  return (-1);
}

#if defined(USE_DEBUG)
/*
 * Handle printing of option names
 */
struct opt_list {
       int         opt_val;
       const char *opt_name;
     };

static const struct opt_list sol_options[] = {
                  { SO_DEBUG,       "SO_DEBUG"       },
                  { SO_ACCEPTCONN,  "SO_ACCEPTCONN"  },
                  { SO_REUSEADDR,   "SO_REUSEADDR"   },
                  { SO_KEEPALIVE,   "SO_KEEPALIVE"   },
                  { SO_DONTROUTE,   "SO_DONTROUTE"   },
                  { SO_BROADCAST,   "SO_BROADCAST"   },
                  { SO_USELOOPBACK, "SO_USELOOPBACK" },
                  { SO_LINGER,      "SO_LINGER"      },
                  { SO_OOBINLINE,   "SO_OOBINLINE"   },
                  { SO_SNDBUF,      "SO_SNDBUF"      },
                  { SO_RCVBUF,      "SO_RCVBUF"      },
                  { SO_SNDLOWAT,    "SO_SNDLOWAT"    },
                  { SO_RCVLOWAT,    "SO_RCVLOWAT"    },
                  { SO_SNDTIMEO,    "SO_SNDTIMEO"    },
                  { SO_RCVTIMEO,    "SO_RCVTIMEO"    },
                  { SO_ERROR,       "SO_ERROR"       },
                  { SO_TYPE,        "SO_TYPE"        }
                };

static const struct opt_list tcp_options[] = {
                  { TCP_NODELAY, "TCP_NODELAY" },
                  { TCP_MAXSEG,  "TCP_MAXSEG"  },
                  { TCP_NOPUSH,  "TCP_NOPUSH"  },
                  { TCP_NOOPT,   "TCP_NOOPT"   }
                };

static const struct opt_list raw_options[] = {
                  { IP_OPTIONS        , "IP_OPTIONS"         },
                  { IP_HDRINCL        , "IP_HDRINCL"         },
                  { IP_TOS            , "IP_TOS"             },
                  { IP_TTL            , "IP_TTL"             },
                  { IP_RECVOPTS       , "IP_RECVOPTS"        },
                  { IP_RECVRETOPTS    , "IP_RECVRETOPTS"     },
                  { IP_RECVDSTADDR    , "IP_RECVDSTADDR"     },
                  { IP_RETOPTS        , "IP_RETOPTS"         },
                  { IP_MULTICAST_IF   , "IP_MULTICAST_IF"    },
                  { IP_MULTICAST_TTL  , "IP_MULTICAST_TTL"   },
                  { IP_MULTICAST_LOOP , "IP_MULTICAST_LOOP"  },
                  { IP_ADD_MEMBERSHIP , "IP_ADD_MEMBERSHIP"  },
                  { IP_DROP_MEMBERSHIP, "IP_DROP_MEMBERSHIP" },
                  { IP_MULTICAST_VIF  , "IP_MULTICAST_VIF"   },
                  { IP_RSVP_ON        , "IP_RSVP_ON"         },
                  { IP_RSVP_OFF       , "IP_RSVP_OFF"        },
                  { IP_RSVP_VIF_ON    , "IP_RSVP_VIF_ON"     },
                  { IP_RSVP_VIF_OFF   , "IP_RSVP_VIF_OFF"    },
                  { IP_PORTRANGE      , "IP_PORTRANGE"       },
                  { IP_RECVIF         , "IP_RECVIF"          },
                  { IP_FW_ADD         , "IP_FW_ADD"          },
                  { IP_FW_DEL         , "IP_FW_DEL"          },
                  { IP_FW_FLUSH       , "IP_FW_FLUSH"        },
                  { IP_FW_ZERO        , "IP_FW_ZERO"         },
                  { IP_FW_GET         , "IP_FW_GET"          },
                  { IP_NAT            , "IP_NAT"             }
                };


static const char *lookup (int option, const struct opt_list *opt, int num)
{
  static char buf[10];

  while (num)
  {
    if (opt->opt_val == option)
       return (opt->opt_name);
    num--;
    opt++;
  }
  sprintf (buf, "?%d", option);
  return (buf);
}

static const char *sockopt_name (int option, int level)
{
  switch ((DWORD)level)
  {
    case IPPROTO_UDP:
         return ("udp option!?");

    case SOL_SOCKET:
         return lookup (option, sol_options, DIM(sol_options));

    case IPPROTO_TCP:
         return lookup (option, tcp_options, DIM(tcp_options));

    case IPPROTO_IP:
    case IPPROTO_ICMP:
         return lookup (option, raw_options, DIM(raw_options));

    default:
         return ("invalid level?");
  }
}

#endif  /* USE_DEBUG    */
#endif  /* USE_BSD_FUNC */
