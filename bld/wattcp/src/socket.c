/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#include "socket.h"
#include "pcdbug.h"

#if defined(USE_BSD_FUNC)

#ifdef __DJGPP__
  #include <sys/resource.h>
  #include <sys/fsext.h>
  #include <dos.h>
  #include <unistd.h>
#endif

#include "pcbufsiz.h"
#include "pchooks.h"

#ifdef USE_LIBPCAP
  #include "w32pcap.h"
#endif

static int     socket_block = 0;        /* sock_daemon() semaphore */
static int     s_last  = S_FIRST;       /* highest socket number */
static Socket *socket_list = NULL;
static BOOL    socket_init  = 0;

#if 0  /* !!to-do */
  #define SOCK_HASH_SIZE (MAX_SOCKETS / 32)
  static Socket *sk_hashes [SOCK_HASH_SIZE];
#endif

#define FREE_SK(x)  do {           \
                      if (x) {     \
                         free (x); \
                         x = NULL; \
                      }            \
                    } while (0)


/*
 * Memory allocation; print some info if allocation fails
 */
void *_sock_calloc (const char *file, unsigned line, size_t size)
{
    void *ptr;

#if defined(WATCOM386) && 0  /* find DOS4GW bug! */
    int rc = _heapset (0xCC);

    if (rc != _HEAPOK && rc != _HEAPEMPTY)
        SOCK_FATAL (("%s (%u) Fatal: heap corrupt\r\n", file, line));
#endif

#if defined(USE_FORTIFY) || defined(USE_BSD_FORTIFY)
    ptr = Fortify_calloc (1, size, file, line);
#else
    ptr = calloc (1, size);
#endif

    if (!ptr) {
#if defined(WATCOM386) && 0  /* find DOS4GW bug! */
        struct _heapinfo hi;
        _heapwalk (&hi);
#endif
        SOCK_FATAL (("%s (%u) Fatal: Allocation failed\r\n", file, line));
    }

#if !defined(USE_BSD_FATAL) && !defined(USE_FORTIFY) && !defined(USE_BSD_FORTIFY)
    ARGSUSED (file);
    ARGSUSED (line);
#endif
    return (ptr);
}

#if defined(USE_BSD_FORTIFY)  /* to detect leaks done here */
  #undef  SOCK_CALLOC
  #define SOCK_CALLOC(x)  Fortify_calloc (1, x, __FILE__, __LINE__)
#endif


/*
 *  `inuse[]' has a non-zero bit for each socket-descriptor in use.
 *  There can be max `MAX_SOCKETS' allocated at any time.  Dead stream
 *  sockets will be unlinked by `sock_daemon()' in due time.
 *
 *  Non-djgpp targets:
 *    Allocate a new descriptor (handle) by searching through `inuse' for
 *    the first zero bit. Update `s_last' as needed.
 *
 *  djgpp target:
 *    Allocate a descriptior from the "File System Extension" layer.
 *    `s_last' is not used (initialised to MAX_SOCKETS).
 */

static fd_set inuse [NUM_SOCK_FDSETS];

static int sock_get_fd (void)
{
#if defined(__DJGPP__) && defined(USE_FSEXT)
    extern int _fsext_demux (__FSEXT_Fnumber func,  /* in fsext.c */
                           int *rv, va_list _args);

    int s = __FSEXT_alloc_fd (_fsext_demux);

    if (s < 0) {
        SOCK_FATAL (("%s (%u) Fatal: FSEXT_alloc_fd() failed\r\n", __FILE__, __LINE__));
        return (-1);
    }

    if (FD_ISSET(s, &inuse[0])) {
        SOCK_FATAL (("%s (%u) Fatal: Reusing existing socket\n", __FILE__, __LINE__));
        return (-1);
    }

#else
    int s;

    for (s = S_FIRST; s < s_last; s++) {
        if (!FD_ISSET(s, &inuse[0])      /* not marked as in-use */
            && !_socklist_find(s)) {    /* don't use a dying socket */
            break;
        }
    }

#endif /* __DJGPP__ && USE_FSEXT */

    if (s < MAX_SOCKETS) {
        if (s == s_last)
            s_last++;

        FD_SET (s, &inuse[0]);
        return (s);
    }

    /* No vacant bits in 'inuse' array. djgpp (and DOS) could theoretically
     * return a file-handle > 'MAX_SOCKETS-1'.
     */
    return (-1);
}

/*
 *  _sock_dos_fd -
 *    Return TRUE if `s' is a valid DOS handle.
 *    Used to differentiate EBADF from ENOTSOCK.
 *
 *    Note: for non-djgpp targets 's' may have same value as a
 *          DOS-handle. This function should only be used when 's'
 *          isn't found in 'socket_list'.
 */
int _sock_dos_fd (int s)
{
    if (s >= fileno(stdin) && s <= fileno(stderr)) /* 0..2 (redirected) */
        return (1);

    if (s > fileno(stderr) && isatty(s))
        return (1);
    return (0);
}

/*
 *  Free receive buffer associated with udp/tcp sockets
 */
static __inline void _free_recv_buf (sock_type *sk)
{
    if (sk->u.rx_data != sk->u.rx_buf) {
        free (sk->u.rx_data);
        sk->u.rx_data = sk->u.rx_buf;
        sk->u.rx_datalen = 0;
    }
}

/*
 *  Free sending buffer associated with tcp sockets
 */
static __inline void _free_send_buf (sock_type *sk)
{
    if (sk->tcp.tx_data != sk->tcp.tx_buf) {
        free (sk->tcp.tx_data);
        sk->tcp.tx_data = sk->tcp.tx_buf;
        sk->tcp.tx_datalen = 0;
    }
}

/*
 *  socket_list_del
 *    Deletes the list element associated with a socket.
 *    Return pointer to next node.
 *    Return NULL if no next or sock not found.
 */
static __inline Socket *socket_list_del (int s)
{
    Socket *_socket, *next, *last;

    for (_socket = last = socket_list; _socket != NULL; last = _socket, _socket = _socket->next) {
        if (_socket->fd != s) {
            continue;
        }
        if (_socket == socket_list) {
            socket_list = _socket->next;
        } else {
            last->next = _socket->next;
        }
        next = _socket->next;
        free (_socket);
        return (next);
    }
    return (NULL);
}

/*
 *  Traverse socket-list to find other SOCK_STREAM sockets
 *  besides 'socket' which are also listening.
 *  Unhook '_tcp_syn_hook' if none found.
 */
static __inline void unset_tcp_syn_hook (Socket *socket)
{
    Socket *_socket;
    int    num = 0;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->so_type == SOCK_STREAM
          && (_socket->so_options & SO_ACCEPTCONN)
          && _socket != socket) {
            num++;
        }
    }
    if (num == 0) {
        _tcp_syn_hook = NULL;
    }
}

/*
 *  Traverse socket-list to find other SOCK_RAW sockets
 *  besides 'socket'. Unhook '_raw_ip_hook' if none found.
 */
static __inline void unset_raw_ip_hook (Socket *socket)
{
    Socket *_socket;
    int    num = 0;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->so_type == SOCK_RAW && _socket != socket) {
            num++;
        }
    }

    if (num == 0) {
        _raw_ip_hook = NULL;
    }
}

/*
 *  _sock_del_fd
 *    Delete the socket from `inuse' array and all memory associated
 *    with it. Also unlink it from the socket list (socket_list).
 *    Return pointer to next node in list or NULL if none/error.
 */
Socket * _sock_del_fd (const char *file, unsigned line, int s)
{
    Socket    *_socket, *next = NULL;
    sock_type *sk;

    SOCK_DEBUGF ((NULL, "\n  _sock_del_fd:%d", s));

    if (s < S_FIRST || s >= s_last || !FD_ISSET(s, &inuse[0])) {
        SOCK_FATAL (("%s (%u) Fatal: socket %d not inuse\r\n", file, line, s));
        return (NULL);
    }

    _socket = _socklist_find (s);
    if (_socket == NULL) {
        SOCK_FATAL (("%s (%u) Fatal: socket %d not in list\r\n", file, line, s));
        goto not_inuse;
    }

    if (_socket->cookie != SAFETYTCP) {    /* Aaarg! marker destroyed */
        SOCK_FATAL (("%s (%u) fatal: socket %d (%p) overwritten\r\n",
                 file, line, s, _socket));
        goto not_inuse;
    }

    sk = _socket->proto_sock;
    if (sk != NULL) {
        switch (_socket->so_type) {
        case SOCK_STREAM:
        case SOCK_DGRAM:
            reuse_localport (sk->u.myport); /* clear 'lport_inuse' bit now */
            sock_abort (sk);
            _free_recv_buf (sk);
            if (_socket->so_type == SOCK_STREAM) {
                _free_send_buf (sk);
            }
            break;
        case SOCK_RAW:
            sk->u.ip_type = 0;
            sk->raw.next = NULL;
            break;
        }
        FREE_SK (sk);
    }

    switch (_socket->so_type) {
    case SOCK_STREAM:
        unset_tcp_syn_hook (_socket);
        break;
    case SOCK_DGRAM:
        break;
    case SOCK_RAW:
        unset_raw_ip_hook (_socket);
        break;
    default:
        SOCK_DEBUGF ((NULL, "\n  _sock_del_fd(%d): unknown type %d",
                       s, _socket->so_type));
        break;
    }

    FREE_SK (_socket->local_addr);
    FREE_SK (_socket->remote_addr);
    FREE_SK (_socket->ip_opt);
    FREE_SK (_socket->bcast_pool);

#if defined(USE_FSEXT) && defined(__DJGPP__)
    /* Free the socket from File-System Extension system.
     * Free the duplicated handle from DOS's System File Table.
     */
    __FSEXT_set_function (s, NULL);
    _close (s);
#endif

    next = socket_list_del (s);  /* delete socket from linked list */

not_inuse:
    if (s == s_last-1)
        s_last--;

    FD_CLR (s, &inuse[0]);

#if !defined(USE_BSD_FATAL)
    ARGSUSED (file);
    ARGSUSED (line);
#endif

    return (next);
}

#ifdef NOT_USED
/*
 *  socket_find_fd
 *    Finds the 'fd' associated with pointer 'socket'.
 *    Return -1 if not found.
 */
static int socket_find_fd (const Socket *socket)
{
    Socket *_socket;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket == socket) {
            return (_socket->fd);
        }
    }
    return (-1);
}

/*
 *  socket_find_udp
 *    Finds the 'Socket' associated with udp-socket 'sk'.
 *    Return NULL if not found.
 */
static Socket *socket_find_udp (const sock_type *sk)
{
    Socket *_socket;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->proto_sock == sk) {
            return (_socket);
        }
    }
    return (NULL);
}
#endif

/*
 *  socket_find_tcp
 *    Finds the 'Socket' associated with tcp-socket 'sk'.
 *    Return NULL if not found.
 */
static Socket *socket_find_tcp (const sock_type *sk)
{
    Socket *_socket;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->proto_sock == sk) {
            return _socket;
        }
    }
    return (NULL);
}


/*
 *  socket_raw_recv - Called from _ip_handler() via `_raw_ip_hook'.
 *    IP-header is already checked in _ip_handler().
 *    Finds all 'Socket' associated with raw IP-packet 'ip'.
 *    Enqueue to 'socket->proto_sock->raw'.
 *    Return >=1 if 'ip' is consumed, 0 otherwise.
 *
 *  Fix-me: This routine will steal all packets destined for
 *          SOCK_STREAM/SOCK_DGRAM sockets if those sockets are
 *          allocated after the SOCK_RAW socket (behind in socket_list).
 */
static int socket_raw_recv (const in_Header *ip)
{
    Socket *_socket;
    sock_type *sk;
    int     num_enqueued = 0;
    int     num_dropped  = 0;
    int     hlen = in_GetHdrLen (ip);
    DWORD   dst  = ntohl (ip->destination);
    size_t  len  = ntohs (ip->length);

    /* Jumbo packets won't match any raw-sockets
     */
    if (len > sizeof(_socket->proto_sock->raw.data))
        return (0);

    /* Not addressed to us or not (limited) broadcast
     */
    if (!is_local_addr(dst) && !is_ip_brdcast(ip))
        return (0);

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
#if 0 /* !! to-do */
        if (_socket->so_type == SOCK_RAW && _socket->so_proto == IPPROTO_RAW)
            ; /* socket matches every IP-protocol, enqueue */
#endif

        if (ip->proto == IPPROTO_TCP && _socket->so_type == SOCK_STREAM)
            return (0);

        if (ip->proto == IPPROTO_UDP && _socket->so_type == SOCK_DGRAM)
            return (0);

        if (_socket->so_type != SOCK_RAW ||
            (ip->proto != _socket->so_proto && _socket->so_proto != IPPROTO_IP))
            continue;

        /* !!to-do: follow the '_socket->proto_sock->raw.next' pointer to first
         *          vacant buffer.
         * assumes _socket->proto_sock is non-NULL
         */
        sk = _socket->proto_sock;
        if (sk->raw.used) {
            num_dropped++;
            SOCK_DEBUGF ((_socket, "\n  socket %d dropped IP, proto %d", _socket->fd, ip->proto));
        } else {
            /* Copy IP-header to raw_sock.ip
             */
            memcpy (&sk->raw.ip, ip, sizeof(*ip));

            /* Copy any IP-options
             */
            if (hlen > sizeof(*ip) && _socket->ip_opt) {
                int olen = min (_socket->ip_opt_len, hlen - sizeof(*ip));
                memcpy (&_socket->ip_opt, ip+1, olen);
            }

            /* Copy rest of IP-packet
             */
            memcpy (sk->raw.data, (BYTE*)ip+hlen, len);
            sk->raw.used = TRUE;
            num_enqueued++;
        }
    }

    if (num_enqueued > 0)     /* both enqueued and dropped is possible */
        STAT (ipstats.ips_delivered++);

    if (num_dropped > 0)
        STAT (ipstats.ips_idropped++);

#ifdef USE_DEBUG
    if (num_dropped > 0 || num_enqueued > 0)
        DEBUG_RX (NULL, ip);
#endif

    return (num_enqueued);
}

/*
 *  tcp_sock_daemon -
 *    Called by sock_daemon() to handle SOCK_STREAM sockets.
 *
 *    Unlink the socket from the linked list if application has
 *    read all data and tcp_state has become CLOSED and the linger
 *    period has expired.
 *
 */
static Socket *tcp_sock_daemon (Socket *socket, sock_type *sk)
{
    Socket *next  = socket->next;
    int     s     = socket->fd;
    int     state = sk->tcp.state;

    if ((socket->so_options & SO_KEEPALIVE) && chk_timeout(socket->keepalive)) {
        sock_keepalive (sk);
        if (tcp_keepalive) {
            socket->keepalive = set_timeout (1000 * tcp_keepalive);
        } else {
            socket->keepalive = 0;
        }
    }

    if (state == tcp_StateSYNSENT) {          /* opening active tcp session */
        socket->so_state |= SS_ISCONNECTING;
    } else if (state == tcp_StateESTAB) {     /* established tcp session */
        socket->so_state |=  SS_ISCONNECTED;
        socket->so_state &= ~SS_ISCONNECTING;
        socket->so_state &= ~SS_ISDISCONNECTING;
    } else if (state >= tcp_StateTIMEWT) {    /* dying tcp session */
        int  closing  = socket->so_state & (SS_ISDISCONNECTING | SS_CANTSENDMORE);

        socket->so_state &= ~(SS_ISCONNECTED | SS_ISCONNECTING);

        if (socket->close_time && (socket->so_state & SS_CANTRCVMORE)) {
            /* Flush any remaining Rx data received after shutdown(0) called.
             */
            sock_fastread (sk, NULL, -1);
        }

        if (closing && sk->u.ip_type == 0) {     /* fully closed, refused or aborted */
            int expired = 0;

            if (!sock_tbused(sk)) {
                _free_send_buf (sk);
            }
            if (!sock_rbused(sk)) {
                _free_recv_buf (sk);         /* free memory not needed anymore */
                FREE_SK (socket->ip_opt);
            }
            if (socket->close_time)        /* close_s() called */
                expired = (time(NULL) - socket->close_time >= socket->linger_time);

            /* If linger-period expired and fully closed, delete the TCB
             */
            if (expired && state == tcp_StateCLOSED) {
                SOCK_DEBUGF ((socket, "\n  tcp_sock_daemon del:%d, lport %d", s, sk->tcp.myport));
                next = SOCK_DEL_FD (s);
            }
        }
    }
    return (next);
}


/*
 * Called by sock_daemon() for SOCK_DGRAM sockets.
 *
 * Unlink the socket from the linked list if application have read all
 * data and if "state" is disconnecting.
 *
 * Note: Setting 'SS_ISDISCONNECTING' is really a mis-nomer, but
 * should indicate socket is closed/aborted with Rx-data remaining.
 */
static Socket *udp_sock_daemon (Socket *socket, sock_type *sk)
{
    Socket *next = socket->next;

    if ((socket->so_state & (SS_ISDISCONNECTING | SS_CANTSENDMORE))
      && (sk->udp.rx_datalen == 0 || sk->u.ip_type == 0)) {
        SOCK_DEBUGF ((socket, "\n  udp_sock_daemon del:%d", socket->fd));
        next = SOCK_DEL_FD (socket->fd);
    }
    return (next);
}

/*
 * Called from tcp_tick(), but not more than once every 55msec
 */
static void sock_daemon (void)
{
    Socket *_socket, *next = NULL;
    sock_type *sk;

    /* If we're in a critical region (e.g. select_s()) where we don't
     * want our socket-list to change, do this later.
     */
    if (socket_block)
        return;

    for (_socket = socket_list; _socket != NULL; _socket = next) {
        int s = _socket->fd;
        next  = _socket->next;

        if (!FD_ISSET(s, &inuse[0]))
            continue;

        if (_socket->local_addr == NULL)  /* not bound to anything yet */
            continue;

#if 0
        _sock_debugf (NULL, "\nsock_daemon:%d", _socket->fd);
#endif
        sk = _socket->proto_sock;
        if (sk != NULL) {
            switch (_socket->so_type) {
            case SOCK_STREAM:
                next = tcp_sock_daemon (_socket, sk);
                break;
            case SOCK_DGRAM:
                next = udp_sock_daemon (_socket, sk);
                break;
            }
        }
    }
}

/*
 * Start and stop critical regions from letting `socket_list' be
 * destroyed (e.g. in sock_daemon) and thus confusing select(),
 * connect() etc.
 */
void _sock_start_crit (void)
{
    if (socket_block < INT_MAX) {
        ++socket_block;
    }
}

void _sock_stop_crit (void)
{
    if (socket_block > 0) {
        --socket_block;

#ifdef SIGALRM         /* handle SIGALRM raised in crit-section */
        /* !!to-do */
#endif

        if (socket_block == 0) {
            sock_daemon();  /* run blocked sock_daemon() */
        }
    }
}


#if defined(USE_BSD_FORTIFY) && defined(USE_DEBUG)
static void fortify_exit (void)
{
    Socket *_socket;
    sock_type *sk;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        const char *type;

        sk = _socket->proto_sock;
        switch (_socket->so_type) {
        case SOCK_STREAM:
            type = "TCP";
            break;
        case SOCK_DGRAM:
            type = "UDP";
            break;
        case SOCK_RAW:
            type = "Raw";
            break;
        default:
            type = "<?>";
            break;
        }
        SOCK_DEBUGF ((NULL, "\n%2d: inuse %d, type %s, data %08lX",
                  _socket->fd, FD_ISSET(_socket->fd, &inuse[0]) ? 1 : 0,
                  type, (DWORD)sk));

        if (sk != NULL) {
            if (sk->u.ip_type == TCP_PROTO) {
                SOCK_DEBUGF ((NULL, " (ip_type %d, state %s, ports %u/%u, rxdatalen %d)",
                         TCP_PROTO, tcpState[sk->tcp.state],
                         sk->tcp.myport, sk->tcp.hisport, sk->tcp.rx_datalen));
            } else if (_socket->so_state & SS_ISDISCONNECTING) {
                SOCK_DEBUGF ((NULL, " (closed)"));
            } else {
                SOCK_DEBUGF ((NULL, " (aborted?)"));
            }
        }
    }

    Fortify_ListAllMemory();
    Fortify_OutputStatistics();
}
#endif /* USE_BSD_FORTIFY && USE_DEBUG */


static int InitSockets (void)
{
    extern int __pull_neterr_module;  /* to make linker pull in */
    __pull_neterr_module = 0;         /* correct sys_errlist[]  */

    _watt_do_exit = 0;    /* don't make sock_init() call exit() */
    if (sock_init())
        return (0);

#ifdef __DJGPP__
    {
        struct rlimit r;
        getrlimit (RLIMIT_NOFILE, &r);
        r.rlim_max = MAX_SOCKETS;     /* We don't know this before we try it */
        setrlimit (RLIMIT_NOFILE, &r);
    }
    s_last = MAX_SOCKETS;
#else
    s_last = S_FIRST;
#endif  /* __DJGPP__ */

    socket_list = NULL;
    memset (&inuse, 0, sizeof(inuse));
    addwattcpd (sock_daemon);

#if defined(USE_BSD_FORTIFY) && defined(USE_DEBUG)
    (void) Fortify_EnterScope();
    (void) Fortify_SetOutputFunc (bsd_fortify_print);
    atexit (fortify_exit);
#endif
    return (1);
}

/*
 * _socklist_find
 *   Returns a pointer to the structure that contains the socket ID passed
 *   to the function. If socket `s' was not found, NULL is returned
 */
Socket *_socklist_find (int s)
{
    Socket *_socket;

    if (!socket_init) {
        if (!InitSockets())
            return (NULL);
        socket_init = 1;
    }
    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->fd == s) {
            return (_socket);
        }
    }
    return (NULL);
}

/*
 * socket_list_add
 *   Adds a new socket to the socket_list
 */
static Socket *socket_list_add (int s, int type, int proto)
{
    Socket *_socket = SOCK_CALLOC (sizeof(*_socket));
    sock_type *proto_sk;

    if (_socket == NULL)
        return (NULL);

    switch (proto) {
    case IPPROTO_TCP:
        proto_sk = SOCK_CALLOC (sizeof(tcp_Socket));
        break;
    case IPPROTO_UDP:
        proto_sk = SOCK_CALLOC (sizeof(udp_Socket));
        break;
    default:
        proto_sk = SOCK_CALLOC (sizeof(raw_Socket));
        break;
    }
    if (proto_sk == NULL) {
        free (_socket);
        return (NULL);
    }

#if defined(USE_FSEXT) && defined(__DJGPP__)
    if (!__FSEXT_set_data (s, _socket)) {
        free (proto_sk);
        free (_socket);
        SOCK_FATAL (("%s (%d) Fatal: cannot grow FSEXT table\r\n", __FILE__, __LINE__));
        return (NULL);
    }
#endif

    switch (proto) {
    case IPPROTO_TCP:
        /* Only tcp times out on inactivity
         */
        _socket->timeout     = sock_delay;
        _socket->linger_time = TCP_LINGERTIME;
        break;
    case IPPROTO_UDP:
        break;
    default:
        proto_sk->u.ip_type = IP_TYPE;
        proto_sk->raw.next = NULL; /* !!to-do: make list of MAX_RAW_BUFS */
        break;
    }

    /* Link '_socket' into the 'socket_list'
     */
    if (!socket_list) {
        _socket->next = NULL;
        socket_list = _socket;
    } else {
        _socket->next = socket_list;
        socket_list = _socket;
    }
    _socket->proto_sock = proto_sk;
    _socket->fd         = s;
    _socket->so_type    = type;
    _socket->so_proto   = proto;
    _socket->so_state   = SS_UNCONNECTED;
    _socket->tx_lowat   = DEFAULT_TX_LOWAT;
    _socket->rx_lowat   = DEFAULT_RX_LOWAT;
    _socket->ip_ttl     = IPDEFTTL;
    _socket->ip_tos     = 0;
    _socket->cookie     = SAFETYTCP;

    return (_socket);
}

/*
 * Select (and check) a suitable protocol for socket-type
 */
static __inline int set_proto (int type, int *proto)
{
    if (type == SOCK_STREAM) {
        if (*proto == 0) {
            *proto = IPPROTO_TCP;
        } else if (*proto != IPPROTO_TCP) {
            SOCK_DEBUGF ((NULL, "\nsocket: invalid STREAM protocol (%d)", *proto));
            return (-1);
        }
        _tcp_find_hook = socket_find_tcp;
    } else if (type == SOCK_DGRAM) {
        if (*proto == 0) {
            *proto = IPPROTO_UDP;
        } else if (*proto != IPPROTO_UDP) {
            SOCK_DEBUGF ((NULL, "\nsocket: invalid DGRAM protocol (%d)", *proto));
            return (-1);
        }
    } else if (type == SOCK_RAW) {
        if (*proto == IPPROTO_RAW)     /* match all IP-protocols */
            *proto = IPPROTO_IP;
        _raw_ip_hook = socket_raw_recv;  /* hook for _ip_handler() */
    }
    return (0);
}

/*
 * socket
 *  Parameters:
 *    family   - The protocol family.  Only supports the AF_INET family
 *    type     - SOCK_STREAM (tcp), SOCK_DGRAM (udp) or SOCK_RAW (ip)
 *    protocol - IPPROTO_TCP, IPPROTO_UDP or 0
 *
 *  Return value - The socket ID number
 */
int socket (int family, int type, int protocol)
{
    Socket *_socket;
    int     s, ss;

    if (!socket_init && !InitSockets()) {
        SOCK_ERR (ENETDOWN);
        return (-1);
    }
    socket_init = 1;

    if (family != AF_INET) {
        SOCK_DEBUGF ((NULL, "\nsocket: invalid family (%d)", family));
        SOCK_ERR (EAFNOSUPPORT);
        return (-1);
    }

    if (type != SOCK_STREAM
      && type != SOCK_DGRAM
#if defined(USE_LIBPCAP)
      && type != SOCK_PACKET
#endif
      && type != SOCK_RAW) {
        SOCK_DEBUGF ((NULL, "\nsocket: invalid type (%d)", type));
        SOCK_ERR (ESOCKTNOSUPPORT);
        return (-1);
    }

    if (type == SOCK_RAW && (protocol < 0 || protocol > 255)) {
        SOCK_DEBUGF ((NULL, "\nsocket: invalid SOCK_RAW proto (%d)", protocol));
        SOCK_ERR (EINVAL);
        return (-1);
    }

    if (set_proto (type, &protocol) < 0) {
        SOCK_ERR (EPROTONOSUPPORT);
        return (-1);
    }

    s = sock_get_fd();
    if (s < 0) {
        SOCK_ERR (EMFILE);
        return (-1);
    }

    _socket = socket_list_add (s, type, protocol);
    ss = (_socket != NULL ? s : -1);

    switch (type) {
    case SOCK_STREAM:
        SOCK_DEBUGF ((NULL, "\nsocket: fam:AF_INET type:STREAM, proto %d, %d",
                       protocol, ss));
        break;
    case SOCK_DGRAM:
        SOCK_DEBUGF ((NULL, "\nsocket: fam:AF_INET type:DGRAM, proto %d, %d",
                       protocol, ss));
        break;
    case SOCK_RAW:
        SOCK_DEBUGF ((NULL, "\nsocket: fam:AF_INET type:RAW, proto %d, %d",
                       protocol, ss));
        break;
#if defined(USE_LIBPCAP)
    case SOCK_PACKET:
        SOCK_DEBUGF ((NULL, "\nsocket: fam:AF_INET type:PACK, proto %d, %d",
                       protocol, ss));
        break;
#endif
    }

    if (_socket == NULL) {
        SOCK_ERR (ENOMEM);
        return (-1);
    }

#if defined(USE_LIBPCAP)
    if (type == SOCK_PACKET) {  /* promiscuous mode */
        char err_buf[256];

        _pcap_w32 = pcap_open_live ("pkt", ETH_MAX, 1, 0, err_buf);
        if (!_pcap_w32) {
            SOCK_ERR (EHOSTDOWN);
            /* !!to-do: plug memory leak
             */
            return (-1);
        }
    }
#endif

    return (s);
}

/*
 *  Callback handlers for "ICMP Port/Host Unreachable" or
 *  "ICMP Parameter Problem" issued by lower layer (udp_cancel()
 *  and tcp_cancel() in pctcp.c)
 *
 *  Note: a single ICMP message might apply to several sockets,
 *        but currrently there is a 1-to-1 relation between a
 *        'socket' and a 'tcp' (or 'udp') structure.
 */
static int stream_cancel (const sock_type *sk)
{
    Socket *_socket;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->so_type == SOCK_STREAM && sk == _socket->proto_sock) {
            _socket->so_state |= SS_CONN_REFUSED;
            _socket->so_error  = ECONNREFUSED;
        }
    }
    return (1);
}

static int dgram_cancel (const sock_type *sk)
{
    Socket *_socket;

    for (_socket = socket_list; _socket != NULL; _socket = _socket->next) {
        if (_socket->so_type == SOCK_DGRAM && sk == _socket->proto_sock) {
            _socket->so_state |= SS_CONN_REFUSED;
            _socket->so_error  = ECONNREFUSED;
        }
    }
    return (1);
}

static int sol_callback (sock_type *sk, int icmp_type)
{
    SOCK_DEBUGF ((NULL, "\nsol_callback (s=%08lX, IP-type=%d, ICMP-type %d)",
                (DWORD)sk, sk->u.ip_type, icmp_type));

    if (icmp_type == ICMP_UNREACH || icmp_type == ICMP_PARAMPROB) {
        if (sk->u.ip_type == UDP_PROTO)
            return dgram_cancel (sk);

        if (sk->u.ip_type == TCP_PROTO) {
            return stream_cancel (sk);
        }
    }
    return (0);
}


/*
 * Open and listen routines for SOCK_DGRAM at the socket-level
 */
int _UDP_open (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port)
{
    sock_type *sk = socket->proto_sock;
    DWORD ip = ntohl (host.s_addr);

    loc_port = ntohs (loc_port);
    rem_port = ntohs (rem_port);

    if (!udp_open (&sk->udp, loc_port, ip, rem_port, NULL))
        return (0);

    sock_recv_buf (sk, DEFAULT_RECV_WIN);
    sk->udp.sol_callb = sol_callback;
    return (1);
}

int _UDP_listen (Socket *socket, struct in_addr host, WORD port)
{
    sock_type *sk = socket->proto_sock;
    DWORD addr;

    port = ntohs (port);

    if (socket->so_state & SS_PRIV) {
        int   pool_size  = sizeof(recv_buf) * MAX_DGRAMS;
        char  *pool = malloc (pool_size);

        if (!pool) {
#if defined(USE_BSD_FATAL)
            SOCK_FATAL (("%s (%d) Fatal: Allocation failed\r\n", __FILE__, __LINE__));
#else
            SOCK_ERR (ENOMEM);
            return (-1);
#endif
        }
        socket->bcast_pool = (recv_buf**) pool;

        /* Mapping `INADDR_ANY' to `INADDR_BROADCAST' causes udp_handler()
         * to demux to the correct watt-socket; s->hisaddr = 0xFFFFFFFF in
         * passive socket demux loop.
         */
        if (host.s_addr == INADDR_ANY) {
            addr = INADDR_BROADCAST;
        } else {
            addr = ntohl (host.s_addr);
        }

        udp_listen (&sk->udp, port, addr, 0, NULL);

        /* Setup _recvdaemon() to enqueue broadcast/"unconnected" messages
         */
        sock_recv_init (sk, pool, pool_size);
    } else {
        addr = ntohl (host.s_addr);
        udp_listen (&sk->udp, port, addr, 0, NULL);
    }
    sk->udp.sol_callb = sol_callback;
    return (1);
}


/*
 * Open and listen routines for SOCK_STREAM at the socket-level
 */
int _TCP_open (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port)
{
    sock_type *sk = socket->proto_sock;
    DWORD dest = ntohl (host.s_addr);

    loc_port = ntohs (loc_port);
    rem_port = ntohs (rem_port);

    if (!tcp_open (&sk->tcp, loc_port, dest, rem_port, NULL))
        return (0);

    /*
     * The paramters to tcp_open() is a bit tricky, but the internal Wattcp
     * socket 's' contains the following elements that must match in the
     * first 'for-loop' of tcp_handler().
     *
     * s->hisport != 0                i.e. active (non-listening) port
     * s->myaddr  == ip->destination, our IP-address
     * s->hisaddr == ip->source,      above 'dest' address
     * s->myport  == tcp->dstPort,    above 'loc_port'
     * s->hisport == tcp->srcPort,    above 'rem_port'
     */

    /* Advertise a large rcv-win from the next ACK
     */
    sock_recv_buf (sk, DEFAULT_RECV_WIN);
    sk->tcp.sol_callb = sol_callback;
    return (1);
}

int _TCP_listen (Socket *socket, struct in_addr host, WORD port)
{
    sock_type *sk = socket->proto_sock;
    DWORD addr     = ntohl (host.s_addr);
    WORD  loc_port = ntohs (port);

    tcp_listen (&sk->tcp, loc_port, addr, 0, NULL, 0);
    sk->tcp.sol_callb = sol_callback;
    return (1);
}

#ifdef NOT_USED
/*
 * _sock_half_open -
 *   Return true if peer closed his side.
 *   There might still be data to read
 */
int _sock_half_open (const sock_type *sk)
{
    if (sk == NULL || sk->u.ip_type == UDP_PROTO || sk->u.ip_type == IP_TYPE)
        return (0);

    return (sk->tcp.state >= tcp_StateFINWT1 && sk->tcp.state <= tcp_StateCLOSED);
}
#endif

#if 0  /* not finished */
/*
 * socketpair() - Create a pair of connected sockets.
 * Modified version based on Linux's version.
 */
int socketpair (int family, int type, int protocol, int usockvec[2])
{
    Socket *_socket1, *_socket2;
    int     s1, s2;

    if ((s1 = socket (family, type, protocol)) < 0)
        return (fd1);

    _socket1 = _socklist_find (s1);

    /* Now grab another socket and try to connect the two together.
     */
    if ((s2 = socket (family, type, protocol)) < 0) {
        close_s (s1);
        return (-EINVAL);
    }

    _socket2 = _socklist_find (s2);

    _socket1->conn = _socket2;
    _socket2->conn = _socket1;
    _socket1->so_state = SS_CONNECTED;
    _socket2->so_state = SS_CONNECTED;

    usockvec[0] = s1;
    usockvec[1] = s2;
    return (0);
}
#endif

#endif /* USE_BSD_FUNC */
