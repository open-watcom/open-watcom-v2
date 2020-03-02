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

typedef unsigned    sock_size;

static int set_sol_opt (Socket *socket, int opt, const void *optval, socklen_t optlen);
static int set_raw_opt (Socket *socket, int opt, const void *optval, socklen_t optlen);
static int get_sol_opt (Socket *socket, int opt, void *optval, socklen_t *optlen);
static int get_raw_opt (Socket *socket, int opt, void *optval, socklen_t *optlen);

static int set_tcp_opt (sock_type *sk, int opt, const void *optval, socklen_t optlen);
static int set_udp_opt (sock_type *sk, int opt, const void *optval, socklen_t optlen);
static int get_tcp_opt (sock_type *sk, int opt, void *optval, socklen_t *optlen);
static int get_udp_opt (sock_type *sk, int opt, void *optval, socklen_t *optlen);

static int udp_rx_buf  (sock_type *sk, sock_size size);
static int tcp_rx_buf  (sock_type *sk, sock_size size);
static int raw_rx_buf  (sock_type *sk, sock_size size);
static int udp_tx_buf  (sock_type *sk, sock_size size);
static int tcp_tx_buf  (sock_type *sk, sock_size size);
static int raw_tx_buf  (sock_type *sk, sock_size size);
static int set_tx_lowat (Socket *socket, const sock_size *optval);
static int set_rx_lowat (Socket *socket, const sock_size *optval);
static int get_tx_lowat (const Socket *socket, sock_size *optval);
static int get_rx_lowat (const Socket *socket, sock_size *optval);

#if defined(USE_DEBUG)
static const char *sockopt_name (int option, int level);

static __inline void do_error (void)
{
    char *err = strerror_s (errno_s);
    char *par = strchr (err, '(');

    if (!par) {
        _sock_debugf (NULL, " errno %d", errno_s);
    } else {
        char *eol = strrchr (par, '\n');
        if (eol)
            *eol = '\0';
        _sock_debugf (NULL, " %s", par);
    }
}
#endif

int setsockopt (int s, int level, int option, const void *optval, socklen_t optlen)
{
    Socket *socket = _socklist_find (s);
    sock_type *sk;
    int     rc;

    SOCK_DEBUGF ((socket, "\nsetsockopt:%d, %s", s, sockopt_name(option,level)));

    if (socket == NULL) {
        if (_sock_dos_fd(s)) {
            SOCK_DEBUGF ((NULL, ", ENOTSOCK"));
            SOCK_ERR (ENOTSOCK);
            return (-1);
        }
        SOCK_DEBUGF ((NULL, ", EBADF"));
        SOCK_ERR (EBADF);
        return (-1);
    }

    VERIFY_RW (optval, optlen);

    sk = socket->proto_sock;
    if (level == SOL_SOCKET) {
        rc = set_sol_opt (socket, option, optval, optlen);
    } else if ((level == socket->so_proto) && (level == IPPROTO_TCP)) {
        rc = set_tcp_opt (sk, option, optval, optlen);
    } else if ((level == socket->so_proto) && (level == IPPROTO_UDP)) {
        rc = set_udp_opt (sk, option, optval, optlen);
    } else if (((level == socket->so_proto) && (level == IPPROTO_IP)) ||
           ((level == socket->so_proto) && (level == IPPROTO_ICMP))) {
        rc = set_raw_opt (socket, option, optval, optlen);
    } else {
        SOCK_ERR (ENOPROTOOPT);
        rc = -1;
    }

#if defined(USE_DEBUG)
    if (rc < 0)
        do_error();
#endif

    return (rc);
}

int getsockopt (int s, int level, int option, void *optval, socklen_t *optlen)
{
    Socket *socket = _socklist_find (s);
    sock_type *sk;
    int     rc;

    SOCK_DEBUGF ((socket, "\ngetsockopt:%d, %s", s, sockopt_name(option,level)));

    if (socket == NULL) {
        if (_sock_dos_fd(s)) {
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

    sk = socket->proto_sock;
    if (level == SOL_SOCKET) {
        rc = get_sol_opt (socket, option, optval, optlen);
    } else if (level == socket->so_proto == IPPROTO_TCP) {
        rc = get_tcp_opt (sk, option, optval, optlen);
    } else if (level == socket->so_proto == IPPROTO_UDP) {
        rc = get_udp_opt (sk, option, optval, optlen);
    } else if ((level == socket->so_proto == IPPROTO_IP) ||
           (level == socket->so_proto == IPPROTO_ICMP)) {
        rc = get_raw_opt (socket, option, optval, optlen);
    } else {
        SOCK_ERR (ENOPROTOOPT);
        rc = -1;
    }

#if defined(USE_DEBUG)
    if (rc < 0)
        do_error();
#endif

    return (rc);
}


static int set_sol_opt (Socket *socket, int opt, const void *optval, socklen_t optlen)
{
    sock_type   *sk = socket->proto_sock;

    switch (opt) {
    case SO_DEBUG:
#if defined(USE_DEBUG)
        if (*(BOOL*)optval) {   /* on/off */
            socket->so_options |= SO_DEBUG;
            _sock_dbug_on();
        } else {
            socket->so_options &= ~SO_DEBUG;
            _sock_dbug_off();
        }
#endif
        break;
    case SO_ACCEPTCONN:
        if (*(BOOL*)optval) {   /* on/off */
            socket->so_options |=  SO_ACCEPTCONN;
        } else {
            socket->so_options &= ~SO_ACCEPTCONN;
        }
        break;
    case SO_RCVTIMEO:
      {
        struct timeval *tv = (struct timeval*) optval;
        if (optlen != sizeof(*tv) || tv->tv_usec < 0) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        if (tv->tv_sec == 0) {      /* i.e. use system default */
            socket->timeout = sock_delay;
        } else {
            socket->timeout = tv->tv_sec + tv->tv_usec/1000000UL;
        }
        break;
      }
    case SO_SNDTIMEO:    /* Don't think we need this */
        break;
    case SO_REUSEADDR:
        /*
         * SO_REUSEADDR enables local address reuse, used to bind
         * multiple socks to the same port but with different ip-addr.
         */
        if (sk != NULL) {
            if (socket->so_proto == IPPROTO_TCP || socket->so_proto == IPPROTO_UDP) {
                reuse_localport (sk->u.myport);
                return (0);
            }
        }
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
/*  case SO_REUSEPORT:  missing in BSD? */
        /*
         * SO_REUSEPORT enables duplicate address and port bindings
         * ie, one can bind multiple socks to the same <ip_addr.port> pair
         */
    case SO_KEEPALIVE:
    case SO_DONTROUTE:
    case SO_DONTLINGER:
    case SO_BROADCAST:
    case SO_USELOOPBACK:
    case SO_OOBINLINE:
        break;
    case SO_SNDLOWAT:
        return set_tx_lowat (socket, optval);
    case SO_RCVLOWAT:
        return set_rx_lowat (socket, optval);
    case SO_RCVBUF:
      {
        sock_size size = *(const sock_size*) optval;
        if (size == 0) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        if (sk != NULL) {
            if (socket->so_proto == IPPROTO_UDP) {
                return udp_rx_buf (sk, size);
            }
            if (socket->so_proto == IPPROTO_TCP) {
                return tcp_rx_buf (sk, size);
            }
            return raw_rx_buf (sk, size);
        }
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
      }
    case SO_SNDBUF:
      {
        sock_size size = *(const sock_size*) optval;
        if (size == 0) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        if (sk != NULL) {
            if (socket->so_proto == IPPROTO_UDP) {
                return udp_tx_buf (sk, size);
            }
            if (socket->so_proto == IPPROTO_TCP) {
                return tcp_tx_buf (sk, size);
            }
            return raw_tx_buf (sk, size);
        }
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
      }
    case SO_LINGER:
      {
        struct linger *linger = (struct linger*) optval;

        if (optlen < sizeof(*linger)) {
            SOCK_ERR (EINVAL);
            return (-1);
        }

        if (socket->so_type != SOCK_STREAM || sk == NULL) {
            SOCK_ERR (ENOPROTOOPT);
            return (-1);
        }

        if (linger->l_onoff == 0 && linger->l_linger == 0) {
            sk->tcp.locflags &= ~LF_LINGER;
            socket->linger_time = 0;
        } else if (linger->l_onoff && linger->l_linger > 0) {
            if (linger->l_linger < 100 * TCP_LINGERTIME) {
                socket->linger_time = linger->l_linger / 100;  /* in 10ms units */
            } else {
                socket->linger_time = TCP_LINGERTIME;
            }
            sk->tcp.locflags |= LF_LINGER;
        }
        break;
      }
    default:
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
    }
    return (0);
}


static int get_sol_opt (Socket *socket, int opt, void *optval, socklen_t *optlen)
{
    sock_type      *sk = socket->proto_sock;

    switch (opt) {
    case SO_DEBUG:
    case SO_ACCEPTCONN:
        *(BOOL*)optval = ( (socket->so_options & opt) != 0 );   /* on/off */
        *optlen = sizeof(BOOL);
        break;
    case SO_OOBINLINE:
#if 0
        if (sk == NULL || socket->so_proto != IPPROTO_TCP) {
            SOCK_ERR (ENOPROTOOPT);
            return (-1);
        }
        if (socket->so_options & SO_OOBINLINE) {
            *(int*)optval = urgent_data (sk);
        } else {
            *(int*)optval = 0;
        }
        break;
#endif
        /* fall through */
    case SO_REUSEADDR:    /* to-do !! */
    case SO_KEEPALIVE:
    case SO_DONTROUTE:
    case SO_DONTLINGER:
    case SO_BROADCAST:
    case SO_USELOOPBACK:
        break;
    case SO_SNDLOWAT:
        return get_tx_lowat (socket, optval);
    case SO_RCVLOWAT:
        return get_rx_lowat (socket, optval);
    case SO_RCVBUF:
        if (sk != NULL) {
            *optlen = sizeof(int);
            *(int*)optval = sizeof (sk->raw.data);
            if (socket->so_proto == IPPROTO_UDP || socket->so_proto == IPPROTO_TCP) {
                *(int*)optval = sock_rbsize (sk);
                return (0);
            }
            *(size_t*)optval = sizeof (sk->raw.data);
            return (0);
        }
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
    case SO_SNDBUF:
        if (sk != NULL) {
            *optlen = sizeof(int);
            if (socket->so_proto == IPPROTO_UDP) {
                *(unsigned*)optval = 0;
                return (0);
            }
            if (socket->so_proto == IPPROTO_TCP) {
                *(unsigned*)optval = sock_tbsize (sk);
                return (0);
            }
            *(size_t*)optval = sizeof (sk->raw.data);
            return (0);
        }
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
    case SO_LINGER:
      {
        struct linger *linger = (struct linger*) optval;

        if (optlen == NULL || *optlen < sizeof(*linger)) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        if (socket->so_type != SOCK_STREAM || sk == NULL) {
            SOCK_ERR (ENOPROTOOPT);
            return (-1);
        }
        *optlen = sizeof(*linger);
        linger->l_onoff  = (sk->tcp.locflags & LF_LINGER) ? 1 : 0;
        linger->l_linger = 100 * socket->linger_time;
        break;
      }
    case SO_SNDTIMEO:
        break;
    case SO_RCVTIMEO:
      {
        struct timeval *tv;
        if (*optlen < sizeof(*tv)) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        tv = (struct timeval*)optval;
        if (socket->timeout == 0) {
            tv->tv_usec = LONG_MAX;
            tv->tv_sec  = LONG_MAX;
        } else {
            tv->tv_usec = 0;
            tv->tv_sec  = socket->timeout;
        }
        break;
      }
    case SO_ERROR:
        *(int*)optval = socket->so_error;
        *optlen = sizeof(socket->so_error);
        socket->so_error = 0;   /* !! should be do this */
        break;
    case SO_TYPE:
        *(int*)optval = socket->so_type;
        *optlen = sizeof(socket->so_type);
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
static int set_tcp_opt (sock_type *sk, int opt, const void *optval, socklen_t optlen)
{
    switch (opt) {
    case TCP_NODELAY:
        if (*(BOOL*)optval) {  /* on/off */
            /* disable Nagle's algorithm */
            SETON_SOCKMODE(sk->tcp, TCP_MODE_NONAGLE);
            sk->tcp.locflags |= LF_NODELAY;
        } else {
            /* turn on Nagle */
            SETOFF_SOCKMODE(sk->tcp, TCP_MODE_NONAGLE);
            sk->tcp.locflags &= ~LF_NODELAY;
        }
        break;
    case TCP_MAXSEG:
      {
        long MSS = *(long*)optval;
        if (MSS < 1 || MSS > MAX_WINDOW) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        sk->tcp.max_seg = *(int*)optval;
        break;
      }
    case TCP_NOPUSH:
        if (*(BOOL*)optval) {  /* on/off */
            sk->tcp.locflags |=  LF_NOPUSH;
        } else {
            sk->tcp.locflags &= ~LF_NOPUSH;
        }
        break;
    case TCP_NOOPT:
        if (*(BOOL*)optval) {  /* on/off */
            sk->tcp.locflags |=  LF_NOOPT;
        } else {
            sk->tcp.locflags &= ~LF_NOOPT;
        }
        break;
    default:
        SOCK_ERR (ENOPROTOOPT);
        return (-1);
    }
    ARGSUSED (optlen);
    return (0);
}

static int get_tcp_opt (sock_type *sk, int opt, void *optval, socklen_t *optlen)
{
    switch (opt) {
    case TCP_NODELAY:
        *(BOOL*)optval = ISON_SOCKMODE(sk->tcp, TCP_MODE_NONAGLE);  /* on/off */
        *optlen = sizeof(BOOL);
        break;
    case TCP_MAXSEG:
        *(int*)optval = sk->tcp.max_seg;
        *optlen = sizeof(int);
        break;
    case TCP_NOPUSH:
        *(BOOL*)optval = ( (sk->tcp.locflags & LF_NOPUSH) != 0 );   /* on/off */
        *optlen = sizeof(BOOL);
        break;
    case TCP_NOOPT:
        *(BOOL*)optval = ( (sk->tcp.locflags & LF_NOOPT) != 0 );    /* on/off */
        *optlen = sizeof(BOOL);
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
static int set_udp_opt (sock_type *sk, int opt, const void *optval, socklen_t optlen)
{
    ARGSUSED (sk);          /* no udp option support yet */
    ARGSUSED (opt);
    ARGSUSED (optval);
    ARGSUSED (optlen);
    SOCK_ERR (ENOPROTOOPT);
    return (-1);
}


static int get_udp_opt (sock_type *sk, int opt, void *optval, socklen_t *optlen)
{
    ARGSUSED (sk);          /* no udp option support yet */
    ARGSUSED (opt);
    ARGSUSED (optval);
    ARGSUSED (optlen);
    SOCK_ERR (ENOPROTOOPT);
    return (-1);
}

/*
 * set/get IP/ICMP-layer (raw/multicast) options
 */
static int set_raw_opt (Socket *socket, int opt, const void *optval, socklen_t optlen)
{
    sock_type *sk = socket->proto_sock;

    switch (opt) {
    case IP_OPTIONS:
        if (!socket->ip_opt && (socket->ip_opt = SOCK_CALLOC(sizeof(*socket->ip_opt))) == NULL) {
            SOCK_ERR (ENOMEM);
            return (-1);
        }
        if (optlen == 0 && socket->ip_opt) {
            free (socket->ip_opt);
            socket->ip_opt     = NULL;
            socket->ip_opt_len = 0;
        } else {
            socket->ip_opt_len = min (optlen, sizeof(*socket->ip_opt));
            memcpy (&socket->ip_opt->ip_opts, optval, socket->ip_opt_len);
        }
        break;
    case IP_HDRINCL:
        if (*(BOOL*)optval) {   /* on/off */
            socket->inp_flags |=  INP_HDRINCL;
        } else {
            socket->inp_flags &= ~INP_HDRINCL;
        }
        break;
    case IP_TOS:
        socket->ip_tos = *(int*)optval;
        if (sk != NULL)
            sk->tcp.tos = socket->ip_tos;
        break;
    case IP_TTL:
        socket->ip_ttl = min (1, *(int *)optval);
        if (sk != NULL) {
            if (socket->so_proto == IPPROTO_UDP || socket->so_proto == IPPROTO_TCP) {
                sk->u.ttl = socket->ip_ttl;
            }
        }
        break;
    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      {
#ifdef USE_MULTICAST
        DWORD  ip;
        struct ip_mreq *l_pMreq = (struct ip_mreq*)optval;

        if (!l_pMreq || optlen < sizeof(*l_pMreq)) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        ip = ntohl (l_pMreq->imr_multiaddr.s_addr);
        if (!is_multicast(ip)) {
            SOCK_ERR (EINVAL);
            return (-1);
        }
        if (!_multicast_on) {
            SOCK_ERR (EADDRNOTAVAIL);
            return (-1);
        }
        if (opt == IP_ADD_MEMBERSHIP && !join_mcast_group(ip)) {
            SOCK_ERR (ENOBUFS);        /* !!correct errno? */
            return (-1);
        }
        if (opt == IP_DROP_MEMBERSHIP && !leave_mcast_group(ip)) {
            SOCK_ERR (EADDRNOTAVAIL);  /* !!correct errno? */
            return (-1);
        }
#endif
        break;
      }
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

static int get_raw_opt (Socket *socket, int opt, void *optval, socklen_t *optlen)
{
    switch (opt) {
    case IP_OPTIONS:
        if (socket->ip_opt) {
            *optlen = socket->ip_opt_len;
            memcpy (optval, socket->ip_opt->ip_opts, *optlen);
        } else {
            memset (optval, 0, *optlen);
            *optlen = 0;
        }
        break;
    case IP_HDRINCL:
        *(BOOL*)optval = ( (socket->inp_flags & INP_HDRINCL) != 0 );    /* on/off */
        *optlen = sizeof(BOOL);
        break;
    case IP_TOS:
        *(int*)optval = socket->ip_tos;
        break;
    case IP_TTL:
        *(int*)optval = socket->ip_ttl;
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
static int tcp_rx_buf (sock_type *sk, sock_size size)
{
    BYTE *buf;

    size = min (size, MAX_TCP_RECV_BUF);  /* 64kB/1MB */
    buf  = realloc (sk->tcp.rdata, size);
    if (!buf) {
        SOCK_ERR (ENOMEM);
        return (-1);
    }

    /* Copy the data to new buffer. Data might be overlapping
     * hence using movmem(). Also clear rest of buffer.
     */
    if (sk->tcp.rdatalen > 0) {
        int len = min ((long)size, sk->tcp.rdatalen);

        movmem (sk->tcp.rdata, buf, len);
        if (size > sk->tcp.rdatalen) {
            memset (sk->tcp.rdata + sk->tcp.rdatalen, 0, size - sk->tcp.rdatalen);
        }
    }
    sk->tcp.rdata       = buf;
    sk->tcp.maxrdatalen = size;
#if (DOSX)
    if (size > 64*1024)
        sk->tcp.send_wscale = size >> 16;
#endif
    return (0);
}

/*
 * Set transmit buffer size for TCP.
 * Max size accepted is 64k.
 */
static int tcp_tx_buf (sock_type *sk, sock_size size)
{
#ifdef NOT_YET
    BYTE *buf;

    size = min (size, MAX_TCP_SEND_BUF);
    buf  = realloc (sk->tcp.data, size);
    if (!buf) {
        SOCK_ERR (ENOMEM);
        return (-1);
    }

    /* Copy current data to new buffer. Data might be overlapping
     * hence using movmem().
     */
    if (sk->tcp.datalen > 0) {
        int len = min ((long)size, sk->tcp.datalen);
        movmem (sk->tcp.data, buf, len);
    }
    sk->tcp.data       = buf;
    sk->tcp.maxdatalen = size;
#else
    ARGSUSED (sk);
    ARGSUSED (size);
#endif
    return (0);
}

/*
 * Set receive buffer size for UDP.
 * Max size accepted is 64k.
 */
static int udp_rx_buf (sock_type *sk, sock_size size)
{
    BYTE *buf;

    size = min (size, MAX_UDP_RECV_BUF);
    buf  = realloc (sk->udp.rdata, size);
    if (!buf) {
        SOCK_ERR (ENOMEM);
        return (-1);
    }

    /* Copy current data to new buffer. Data might be overlapping
     * hence using movmem(). Also clear rest of buffer.
     */
    if (sk->udp.rdatalen > 0) {
        int len = min ((long)size, sk->udp.rdatalen);

        movmem (sk->udp.rdata, buf, len);
        if (size > sk->udp.rdatalen) {
            memset (buf + sk->udp.rdatalen, 0, size - sk->udp.rdatalen);
        }
    }
    sk->udp.rdata       = buf;
    sk->udp.maxrdatalen = size;
    return (0);
}

/*
 * Set transmit buffer sizes for UDP.
 * Max size accepted is 64k.
 */
static int udp_tx_buf (sock_type *sk, sock_size size)
{
#ifdef NOT_YET
    BYTE *buf;

    size = min (size, MAX_UDP_SEND_BUF);
    buf  = realloc (sk->udp.data, size);
    if (!buf) {
        SOCK_ERR (ENOMEM);
        return (-1);
    }
#else
    ARGSUSED (sk);
    ARGSUSED (size);
#endif
    return (0);
}


/*
 * Set receive buffer size for RAW socket
 */
static int raw_rx_buf (sock_type *sk, sock_size size)
{
    /* to-do !! */
    ARGSUSED (sk);
    ARGSUSED (size);
    return (0);
}

static int raw_tx_buf (sock_type *sk, sock_size size)
{
    /* to-do !! */
    ARGSUSED (sk);
    ARGSUSED (size);
    return (0);
}


/*
 * Set send buffer "low water marks"
 */
static int set_tx_lowat (Socket *socket, const sock_size *optval)
{
    switch (socket->so_type) {
    case SOCK_STREAM:
        socket->send_lowat = min (*optval, MAX_TCP_SEND_BUF-1);
        break;
    case SOCK_DGRAM:
        socket->send_lowat = min (*optval, MAX_UDP_SEND_BUF-1);
        break;
    case SOCK_RAW:
        socket->send_lowat = min (*optval, sizeof(socket->proto_sock->raw.data)-1);
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
static int set_rx_lowat (Socket *socket, const sock_size *optval)
{
    sock_type *sk = socket->proto_sock;

    switch (socket->so_type) {
    case SOCK_STREAM:
        if (sk != NULL)
            socket->send_lowat = min (*optval, sk->tcp.maxrdatalen);
        break;
    case SOCK_DGRAM:
        if (sk != NULL)
            socket->send_lowat = min (*optval, sk->udp.maxrdatalen);
        break;
    case SOCK_RAW:
        socket->send_lowat = min (*optval, sizeof(sk->raw.data));
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
static int get_tx_lowat (const Socket *socket, sock_size *optval)
{
    if (socket->so_type == SOCK_STREAM ||
        socket->so_type == SOCK_DGRAM  ||
        socket->so_type == SOCK_RAW)
    {
        *optval = socket->send_lowat;
        return (0);
    }
    SOCK_ERR (ENOPROTOOPT);
    return (-1);
}

static int get_rx_lowat (const Socket *socket, sock_size *optval)
{
    if (socket->so_type == SOCK_STREAM ||
        socket->so_type == SOCK_DGRAM  ||
        socket->so_type == SOCK_RAW)
    {
        *optval = socket->recv_lowat;
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

    while (num) {
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
    switch (level) {
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
