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

#ifdef USE_LIBPCAP
  #include "w32pcap.h"
#endif

static int     sk_block = 0;        /* sock_daemon() semaphore */
static int     sk_last  = SK_FIRST; /* highest socket number */
static Socket *sk_list  = NULL;
static BOOL    sk_init  = 0;

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

#if defined(__WATCOM386__) && 0  /* find DOS4GW bug! */
  int rc = _heapset (0xCC);

  if (rc != _HEAPOK && rc != _HEAPEMPTY)
     SOCK_FATAL (("%s (%u) Fatal: heap corrupt\r\n", file, line));
#endif

#if defined(USE_FORTIFY) || defined(USE_BSD_FORTIFY)
  ptr = Fortify_calloc (size, 1, file, line);
#else
  ptr = calloc (size, 1);
#endif

  if (!ptr)
  {
#if defined(__WATCOM386__) && 0  /* find DOS4GW bug! */
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
  #define SOCK_CALLOC(x)  Fortify_calloc (x, 1, __FILE__, __LINE__)
#endif


/*
 *  `inuse[]' has a non-zero bit for each socket-descriptor in use.
 *  There can be max `MAX_SOCKETS' allocated at any time.  Dead stream
 *  sockets will be unlinked by `sock_daemon()' in due time.
 *
 *  Non-djgpp targets:
 *    Allocate a new descriptor (handle) by searching through `inuse' for
 *    the first zero bit. Update `sk_last' as needed.
 *
 *  djgpp target:
 *    Allocate a descriptior from the "File System Extension" layer.
 *    `sk_last' is not used (initialised to MAX_SOCKETS).
 */

static fd_set inuse [NUM_SOCK_FDSETS];

static int sock_get_fd (void)
{
#if defined(__DJGPP__) && defined(USE_FSEXT)
  extern int _fsext_demux (__FSEXT_Fnumber func,  /* in fsext.c */
                           int *rv, va_list _args);

  int s = __FSEXT_alloc_fd (_fsext_demux);

  if (s < 0)
  {
    SOCK_FATAL (("%s (%u) Fatal: FSEXT_alloc_fd() failed\r\n",
                __FILE__, __LINE__));
    return (-1);
  }

  if (FD_ISSET(s,&inuse[0]))
  {
    SOCK_FATAL (("%s (%u) Fatal: Reusing existing socket\n",
                __FILE__, __LINE__));
    return (-1);
  }

#else
  int s;

  for (s = SK_FIRST; s < sk_last; s++)
      if (!FD_ISSET(s,&inuse[0]) &&    /* not marked as in-use */
          !_socklist_find(s))      /* don't use a dying socket */
         break;

#endif /* __DJGPP__ && USE_FSEXT */

  if (s < MAX_SOCKETS)
  {
    if (s == sk_last)
       sk_last++;

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
 *          isn't found in 'sk_list'.
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
 *  Setup a bigger receive buffer, the default in Wattcp
 *  is only 2k.
 *  Note: If calloc() fails, sock_setbuf() reverts to default
 *        2kB socket buffer.
 *  to-do: allow user to define size using SO_RCVBUF/SO_SNDBUF
 *         before calling connect().
 */
static __inline void set_rcv_buf (sock_type *p)
{
  int len = DEFAULT_RCV_WIN;
  sock_setbuf (p, calloc(len,1), len);
}

/*
 *  Free receive buffer associated with udp/tcp sockets
 */
static __inline void free_rcv_buf (sock_type *p)
{
  if (p->udp.rdata != &p->udp.rddata[0])
  {
    free (p->udp.rdata);
    p->udp.rdata = &p->udp.rddata[0];
    p->udp.rdatalen = 0;
  }
}

/*
 *  sk_list_del
 *    Deletes the list element associated with a socket.
 *    Return pointer to next node.
 *    Return NULL if no next or sock not found.
 */
static __inline Socket *sk_list_del (int s)
{
  Socket *sock, *next, *last;

  for (sock = last = sk_list; sock; last = sock, sock = sock->next)
  {
    if (sock->fd != s)
       continue;

    if (sock == sk_list)
         sk_list    = sock->next;
    else last->next = sock->next;
    next = sock->next;
    free (sock);
    return (next);
  }
  return (NULL);
}

/*
 *  Traverse socket-list to find other SOCK_STREAM sockets
 *  besides 'this' which are also listening.
 *  Unhook '_tcp_syn_hook' if none found.
 */
static __inline void unset_tcp_syn_hook (Socket *this)
{
  Socket *sock;
  int    num = 0;

  for (sock = sk_list; sock; sock = sock->next)
      if (sock->so_type == SOCK_STREAM      &&
         (sock->so_options & SO_ACCEPTCONN) &&
         sock != this)
        num++;

  if (num == 0)
     _tcp_syn_hook = NULL;
}

/*
 *  Traverse socket-list to find other SOCK_RAW sockets
 *  besides 'this'. Unhook '_raw_ip_hook' if none found.
 */
static __inline void unset_raw_ip_hook (Socket *this)
{
  Socket *sock;
  int    num = 0;

  for (sock = sk_list; sock; sock = sock->next)
      if (sock->so_type == SOCK_RAW && sock != this)
         num++;

  if (num == 0)
     _raw_ip_hook = NULL;
}

/*
 *  _sock_del_fd
 *    Delete the socket from `inuse' array and all memory associated
 *    with it. Also unlink it from the socket list (sk_list).
 *    Return pointer to next node in list or NULL if none/error.
 */
Socket * _sock_del_fd (const char *file, unsigned line, int s)
{
  Socket    *sock, *next = NULL;
  sock_type *sk;

  SOCK_DEBUGF ((NULL, "\n  _sock_del_fd:%d", s));

  if (s < SK_FIRST || s >= sk_last || !FD_ISSET(s,&inuse[0]))
  {
    SOCK_FATAL (("%s (%u) Fatal: socket %d not inuse\r\n", file, line, s));
    return (NULL);
  }

  sock = _socklist_find (s);
  if (!sock)
  {
    SOCK_FATAL (("%s (%u) Fatal: socket %d not in list\r\n", file, line, s));
    goto not_inuse;
  }

  if (sock->cookie != SAFETYTCP)  /* Aaarg! marker destroyed */
  {
    SOCK_FATAL (("%s (%u) fatal: socket %d (%p) overwritten\r\n",
                 file, line, s, sock));
    goto not_inuse;
  }

  switch (sock->so_type)
  {
    case SOCK_STREAM:
         sk = (sock_type*) sock->tcp_sock;
         if (sk)
         {
           reuse_localport (sk->tcp.myport); /* clear 'lport_inuse' bit now */
           sock_abort (sk);
           free_rcv_buf (sk);
         }
         FREE_SK (sock->tcp_sock);
         unset_tcp_syn_hook (sock);
         break;

    case SOCK_DGRAM:
         sk = (sock_type*) sock->udp_sock;
         if (sk)
         {
           reuse_localport (sk->udp.myport);
           sock_abort (sk);
           free_rcv_buf (sk);
         }
         FREE_SK (sock->udp_sock);
         break;

    case SOCK_RAW:
         sock->raw_sock->ip_type = 0;
         sock->raw_sock->next    = NULL;
         FREE_SK (sock->raw_sock);
         unset_raw_ip_hook (sock);
         break;

    default:
         SOCK_DEBUGF ((NULL, "\n  _sock_del_fd(%d): unknown type %d",
                       s, sock->so_type));
         break;
  }

  FREE_SK (sock->local_addr);
  FREE_SK (sock->remote_addr);    
  FREE_SK (sock->ip_opt);
  FREE_SK (sock->bcast_pool);

#if defined(USE_FSEXT) && defined(__DJGPP__)
  /* Free the socket from File-System Extension system.
   * Free the duplicated handle from DOS's System File Table.
   */
  __FSEXT_set_function (s, NULL);
  _close (s);
#endif

  next = sk_list_del (s);  /* delete socket from linked list */

not_inuse:
  if (s == sk_last-1)
     sk_last--;

  FD_CLR (s, &inuse[0]);

#if !defined(USE_DEBUG)
  ARGSUSED (file);
  ARGSUSED (line);
#endif

  return (next);
}

#ifdef NOT_USED
/*
 *  sock_find_fd
 *    Finds the 'fd' associated with pointer 'socket'.
 *    Return -1 if not found.
 */
static int sock_find_fd (const Socket *socket)
{
  Socket *sock;

  for (sock = sk_list; sock; sock = sock->next)
      if (sock == socket)
         return (sock->fd);
  return (-1);
}

/*
 *  sock_find_udp
 *    Finds the 'Socket' associated with udp-socket 'udp'.
 *    Return NULL if not found.
 */
static Socket *sock_find_udp (const udp_Socket *udp)
{
  Socket *sock;

  for (sock = sk_list; sock; sock = sock->next)
      if (sock->udp_sock == udp)
         return (sock);
  return (NULL);
}
#endif

/*
 *  sock_find_tcp
 *    Finds the 'Socket' associated with tcp-socket 'tcp'.
 *    Return NULL if not found.
 */
static void *sock_find_tcp (const tcp_Socket *tcp)
{
  Socket *sock;

  for (sock = sk_list; sock; sock = sock->next)
      if (sock->tcp_sock == tcp)
         return (void*)sock;
  return (NULL);
}


/*
 *  sock_raw_recv - Called from _ip_handler() via `_raw_ip_hook'.
 *    IP-header is already checked in _ip_handler().
 *    Finds all 'Socket' associated with raw IP-packet 'ip'.
 *    Enqueue to 'sock->raw_sock'.
 *    Return >=1 if 'ip' is consumed, 0 otherwise.
 *
 *  Fix-me: This routine will steal all packets destined for
 *          SOCK_STREAM/SOCK_DGRAM sockets if those sockets are
 *          allocated after the SOCK_RAW socket (behind in sk_list).
 */
static int sock_raw_recv (const in_Header *ip)
{
  Socket *sock;
  int     num_enqueued = 0;
  int     num_dropped  = 0;
  int     hlen = in_GetHdrLen (ip);
  DWORD   dst  = ntohl (ip->destination);
  size_t  len  = ntohs (ip->length);

  /* Jumbo packets won't match any raw-sockets
   */
  if (len > sizeof(sock->raw_sock->data))
     return (0);

  /* Not addressed to us or not (limited) broadcast
   */
  if (!is_local_addr(dst) && !is_ip_brdcast(ip))
     return (0);

  for (sock = sk_list; sock; sock = sock->next)
  {
#if 0 /* !! to-do */
    if (sock->so_type == SOCK_RAW && sock->so_proto == IPPROTO_RAW)
       ; /* socket matches every IP-protocol, enqueue */
#endif

    if (ip->proto == IPPROTO_TCP && sock->so_type == SOCK_STREAM)
       return (0);

    if (ip->proto == IPPROTO_UDP && sock->so_type == SOCK_DGRAM)
       return (0);

    if (sock->so_type != SOCK_RAW ||
        (ip->proto != sock->so_proto && sock->so_proto != IPPROTO_IP))
       continue;

    /* !!to-do: follow the 'sock->raw_sock->next' pointer to first
     *          vacant buffer.
     * assumes sock->raw_sock is non-NULL
     */
    if (sock->raw_sock->used)  
    {
      num_dropped++;
      SOCK_DEBUGF ((sock, "\n  socket %d dropped IP, proto %d",
                    sock->fd, ip->proto));
    }
    else
    {
      /* Copy IP-header to raw_sock.ip
       */
      memcpy (&sock->raw_sock->ip, ip, sizeof(*ip));

      /* Copy any IP-options
       */
      if (hlen > sizeof(*ip) && sock->ip_opt)
      {
        int olen = min (sock->ip_opt_len, hlen - sizeof(*ip));
        memcpy (&sock->ip_opt, ip+1, olen);
      }

      /* Copy rest of IP-packet
       */
      memcpy (&sock->raw_sock->data, (BYTE*)ip+hlen, len);
      sock->raw_sock->used = TRUE;
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
static Socket *tcp_sock_daemon (Socket *sock, tcp_Socket *tcp)
{
  Socket *next  = sock->next;
  int     s     = sock->fd;
  int     state = tcp->state;

  if ((sock->so_options & SO_KEEPALIVE) && chk_timeout(sock->keepalive))
  {
    sock_keepalive ((sock_type*)tcp);
    if (tcp_keepalive)
         sock->keepalive = set_timeout (1000 * tcp_keepalive);
    else sock->keepalive = 0;
  }
  
  if (state == tcp_StateSYNSENT)        /* opening active tcp session */
  {
    sock->so_state |= SS_ISCONNECTING;
  }
  else if (state == tcp_StateESTAB)        /* established tcp session */
  {
    sock->so_state |=  SS_ISCONNECTED;
    sock->so_state &= ~SS_ISCONNECTING;
    sock->so_state &= ~SS_ISDISCONNECTING;
  }
  else if (state >= tcp_StateTIMEWT)             /* dying tcp session */
  {
    sock_type *sk = (sock_type*)tcp;
    int  closing  = sock->so_state & (SS_ISDISCONNECTING | SS_CANTSENDMORE);

    sock->so_state &= ~(SS_ISCONNECTED | SS_ISCONNECTING);

    if (sock->close_time && (sock->so_state & SS_CANTRCVMORE))
    {
      /* Flush any remaining Rx data received after shutdown(0) called.
       */
      sock_fastread (sk, NULL, -1);
    }

    if (closing && tcp->ip_type == 0) /* fully closed, refused or aborted */
    {
      int expired = 0;

      if (!sock_rbused(sk))
      {
        free_rcv_buf (sk);         /* free memory not needed anymore */
        FREE_SK (sock->ip_opt);
      }

      if (sock->close_time)        /* close_s() called */
         expired = (time(NULL) - sock->close_time >= sock->linger_time);

      /* If linger-period expired and fully closed, delete the TCB
       */
      if (expired && state == tcp_StateCLOSED)  
      {
        SOCK_DEBUGF ((sock, "\n  tcp_sock_daemon del:%d, lport %d",
                      s, tcp->myport));
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
static Socket *udp_sock_daemon (Socket *sock, udp_Socket *udp)
{
  Socket *next = sock->next;

  if ((sock->so_state & (SS_ISDISCONNECTING | SS_CANTSENDMORE)) &&
      (udp->rdatalen == 0 || udp->ip_type == 0))
  {
    SOCK_DEBUGF ((sock, "\n  udp_sock_daemon del:%d", sock->fd));
    next = SOCK_DEL_FD (sock->fd);
  }
  return (next);
}

/*
 * Called from tcp_tick(), but not more than once every 55msec
 */
static void sock_daemon (void)
{
  Socket *sock, *next = NULL;

  /* If we're in a critical region (e.g. select_s()) where we don't
   * want our socket-list to change, do this later.
   */
  if (sk_block)
     return;

  for (sock = sk_list; sock; sock = next)
  {
    int s = sock->fd;
    next  = sock->next;

    if (!FD_ISSET(s,&inuse[0]))
       continue;

    if (sock->local_addr == NULL)  /* not bound to anything yet */
       continue;

#if 0
    _sock_debugf (NULL, "\nsock_daemon:%d", sock->fd);
#endif

    switch (sock->so_type)
    {
      case SOCK_STREAM:
           if (sock->tcp_sock)
              next = tcp_sock_daemon (sock, sock->tcp_sock);
           break;
      case SOCK_DGRAM:
           if (sock->udp_sock)
              next = udp_sock_daemon (sock, sock->udp_sock);
           break;
    }
  }
}

/*
 * Start and stop critical regions from letting `sk_list' be
 * destroyed (e.g. in sock_daemon) and thus confusing select(),
 * connect() etc.
 */
void _sock_start_crit (void)
{
  if (sk_block < INT_MAX)
    ++sk_block;
}

void _sock_stop_crit (void)
{
  if (sk_block > 0)
  {
    --sk_block;

#ifdef SIGALRM         /* handle SIGALRM raised in crit-section */
    /* !!to-do */
#endif

    if (!sk_block)
       sock_daemon();  /* run blocked sock_daemon() */
  }
}


#if defined(USE_BSD_FORTIFY) && defined(USE_DEBUG)
static void fortify_exit (void)
{
  Socket *sock;

  for (sock = sk_list; sock; sock = sock->next)
  {
    char *type = "<?>";
    void *data = NULL;
    tcp_Socket *tcp;

    switch (sock->so_type)
    {
      case SOCK_STREAM:
           type = "TCP";
           data = sock->tcp_sock;
           break;
      case SOCK_DGRAM:
           type = "UDP";
           data = sock->udp_sock;
           break;
      case SOCK_RAW:
           type = "Raw";
           data = sock->raw_sock;
           break;
    }
    SOCK_DEBUGF ((NULL, "\n%2d: inuse %d, type %s, data %08lX",
                  sock->fd, FD_ISSET(sock->fd,&inuse[0]) ? 1 : 0,
                  type, (DWORD)data));

    tcp = sock->tcp_sock;
    if (tcp)
    {
      if (tcp->ip_type == TCP_PROTO)
           SOCK_DEBUGF ((NULL, " (ip_type %d, state %s, ports %u/%u, rdatalen %d)",
                         TCP_PROTO, tcpState[tcp->state],
                         tcp->myport, tcp->hisport, tcp->rdatalen));
      else if (sock->so_state & SS_ISDISCONNECTING)
           SOCK_DEBUGF ((NULL, " (closed)"));
      else SOCK_DEBUGF ((NULL, " (aborted?)"));
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
  sk_last = MAX_SOCKETS;
#else
  sk_last = SK_FIRST;
#endif  /* __DJGPP__ */

  sk_list = NULL;
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
  Socket *sock;

  if (!sk_init)
  {
    if (!InitSockets())
       return (NULL);
    sk_init = 1;
  }
  for (sock = sk_list; sock; sock = sock->next)
      if (sock->fd == s)
         return (sock);
  return (NULL);
}

/*
 * sock_list_add
 *   Adds a new socket to the sk_list
 */
static Socket *sock_list_add (int s, int type, int proto)
{
  Socket *sock = SOCK_CALLOC (sizeof(*sock));
  void   *proto_sk;

  if (!sock)
     return (NULL);

  switch (proto)
  {
    case IPPROTO_TCP:
         /* Only tcp times out on inactivity
          */
         sock->timeout     = sock_delay;
         sock->linger_time = TCP_LINGERTIME;
         sock->tcp_sock    = proto_sk = SOCK_CALLOC (sizeof(*sock->tcp_sock));
         if (!sock->tcp_sock)
         {
           free (sock);
           return (NULL);
         }
         break;

    case IPPROTO_UDP:
         sock->udp_sock = proto_sk = SOCK_CALLOC (sizeof(*sock->udp_sock));
         if (!sock->udp_sock)
         {
           free (sock);
           return (NULL);
         }
         break;

    default:
         sock->raw_sock = proto_sk = SOCK_CALLOC (sizeof(*sock->raw_sock));
         if (!sock->raw_sock)
         {
           free (sock);
           return (NULL);
         }
         sock->raw_sock->ip_type = IP_TYPE;
         sock->raw_sock->next = NULL; /* !!to-do: make list of MAX_RAW_BUFS */
         break;
  }

#if defined(USE_FSEXT) && defined(__DJGPP__)
  if (!__FSEXT_set_data (s,sock))
  {
    free (proto_sk);
    free (sock);
    SOCK_FATAL (("%s (%d) Fatal: cannot grow FSEXT table\r\n",
                __FILE__, __LINE__));
    return (NULL);
  }
#else
  ARGSUSED (proto_sk);
#endif

  /* Link 'sock' into the 'sk_list'
   */
  if (!sk_list)
  {
    sock->next = NULL;
    sk_list    = sock;
  }
  else
  {
    sock->next = sk_list;
    sk_list    = sock;
  }
  sock->fd         = s;
  sock->so_type    = type;
  sock->so_proto   = proto;
  sock->so_state   = SS_UNCONNECTED;
  sock->send_lowat = DEFAULT_SEND_LOWAT;
  sock->recv_lowat = DEFAULT_RECV_LOWAT;
  sock->ip_ttl     = IPDEFTTL;
  sock->ip_tos     = 0;
  sock->cookie     = SAFETYTCP;

  return (sock);
}

/*
 * Select (and check) a suitable protocol for socket-type
 */
static __inline int set_proto (int type, int *proto)
{
  if (type == SOCK_STREAM)
  {
    if (*proto == 0)
        *proto = IPPROTO_TCP;

    else if (*proto != IPPROTO_TCP)
    {
      SOCK_DEBUGF ((NULL, "\nsocket: invalid STREAM protocol (%d)", *proto));
      return (-1);
    }
    _tcp_find_hook = sock_find_tcp;
  }
  else if (type == SOCK_DGRAM)
  {
    if (*proto == 0)
        *proto = IPPROTO_UDP;

    else if (*proto != IPPROTO_UDP)
    {
      SOCK_DEBUGF ((NULL, "\nsocket: invalid DGRAM protocol (%d)", *proto));
      return (-1);
    }
  }
  else if (type == SOCK_RAW)
  {
    if (*proto == IPPROTO_RAW)     /* match all IP-protocols */
        *proto = IPPROTO_IP;
    _raw_ip_hook = sock_raw_recv;  /* hook for _ip_handler() */
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
  Socket *sock;
  int     s, ss;

  if (!sk_init && !InitSockets())
  {
    SOCK_ERR (ENETDOWN);
    return (-1);
  }
  sk_init = 1;

  if (family != AF_INET)
  {
    SOCK_DEBUGF ((NULL, "\nsocket: invalid family (%d)", family));
    SOCK_ERR (EAFNOSUPPORT);
    return (-1);
  }

  if (type != SOCK_STREAM &&
      type != SOCK_DGRAM  &&
#if defined(USE_LIBPCAP)
      type != SOCK_PACKET &&
#endif
      type != SOCK_RAW)
  {
    SOCK_DEBUGF ((NULL, "\nsocket: invalid type (%d)", type));
    SOCK_ERR (ESOCKTNOSUPPORT);
    return (-1);
  }

  if (type == SOCK_RAW && (protocol < 0 || protocol > 255))
  {
    SOCK_DEBUGF ((NULL, "\nsocket: invalid SOCK_RAW proto (%d)", protocol));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (set_proto (type, &protocol) < 0)
  {
    SOCK_ERR (EPROTONOSUPPORT);
    return (-1);
  }

  s = sock_get_fd();
  if (s < 0)
  {
    SOCK_ERR (EMFILE);
    return (-1);
  }

  sock = sock_list_add (s, type, protocol);
  ss   = (sock ? s : -1);

  switch (type)
  {
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

  if (!sock)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }

#if defined(USE_LIBPCAP)
  if (type == SOCK_PACKET) /* promiscuous mode */
  {
    char err_buf[256];

    _pcap_w32 = pcap_open_live ("pkt", ETH_MAX, 1, 0, err_buf);
    if (!_pcap_w32)
    {
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
static int stream_cancel (const tcp_Socket *tcp)
{
  Socket *socket;

  for (socket = sk_list; socket; socket = socket->next)
      if (socket->so_type == SOCK_STREAM && tcp == socket->tcp_sock)
      {
        socket->so_state |= SS_CONN_REFUSED;
        socket->so_error  = ECONNREFUSED;
      }
  return (1);
}

static int dgram_cancel (const udp_Socket *udp)
{
  Socket *socket;

  for (socket = sk_list; socket; socket = socket->next)
      if (socket->so_type == SOCK_DGRAM && udp == socket->udp_sock)
      {
        socket->so_state |= SS_CONN_REFUSED;
        socket->so_error  = ECONNREFUSED;
      }
  return (1);
}

static int sol_callback (void *p, int icmp_type)
{
  sock_type *s = (sock_type*)p;
 
  SOCK_DEBUGF ((NULL, "\nsol_callback (s=%08lX, IP-type=%d, ICMP-type %d)",
                (DWORD)p, s->udp.ip_type, icmp_type));

  if (icmp_type == ICMP_UNREACH || icmp_type == ICMP_PARAMPROB)
  {
    if (s->udp.ip_type == UDP_PROTO)
       return dgram_cancel (&s->udp);

    if (s->udp.ip_type == TCP_PROTO)
       return stream_cancel (&s->tcp);
  }
  return (0);
}


/*
 * Open and listen routines for SOCK_DGRAM at the socket-level
 */
int _UDP_open (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port)
{
  DWORD ip = ntohl (host.s_addr);

  loc_port = ntohs (loc_port);
  rem_port = ntohs (rem_port);

  if (!udp_open (socket->udp_sock, loc_port, ip, rem_port, NULL))
     return (0);

  set_rcv_buf ((sock_type*)socket->udp_sock);
  socket->udp_sock->sol_callb = sol_callback;
  return (1);
}

int _UDP_listen (Socket *socket, struct in_addr host, WORD port)
{
  udp_Socket *udp = socket->udp_sock;

  port = ntohs (port);

  if (socket->so_state & SS_PRIV)
  {
    int   pool_size  = sizeof(recv_buf) * MAX_DGRAMS;
    void *pool = malloc (pool_size);
    DWORD addr;

    if (!pool)
    {
      SOCK_FATAL (("%s (%d) Fatal: Allocation failed\r\n",
                  __FILE__, __LINE__));
      SOCK_ERR (ENOMEM);
      return (-1);
    }
    socket->bcast_pool = (recv_buf**) pool;

    /* Mapping `INADDR_ANY' to `INADDR_BROADCAST' causes udp_handler()
     * to demux to the correct watt-socket; s->hisaddr = 0xFFFFFFFF in
     * passive socket demux loop.
     */
    if (host.s_addr == INADDR_ANY)
         addr = INADDR_BROADCAST;
    else addr = ntohl (host.s_addr);

    udp_listen (udp, port, addr, 0, NULL);

    /* Setup _recvdaemon() to enqueue broadcast/"unconnected" messages
     */
    sock_recv_init (udp, pool, pool_size);
  }
  else
  {
    DWORD ip = ntohl (host.s_addr);
    udp_listen (udp, port, ip, 0, NULL);
  }
  udp->sol_callb = sol_callback;
  return (1);
}


/*
 * Open and listen routines for SOCK_STREAM at the socket-level
 */
int _TCP_open (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port)
{
  DWORD dest = ntohl (host.s_addr);

  loc_port = ntohs (loc_port);
  rem_port = ntohs (rem_port);

  if (!tcp_open (socket->tcp_sock, loc_port, dest, rem_port, NULL))
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
  set_rcv_buf ((sock_type*)socket->tcp_sock);
  socket->tcp_sock->sol_callb = sol_callback;
  return (1);
}

int _TCP_listen (Socket *socket, struct in_addr host, WORD port)
{
  DWORD addr     = ntohl (host.s_addr);
  WORD  loc_port = ntohs (port);

  tcp_listen (socket->tcp_sock, loc_port, addr, 0, NULL, 0);
  socket->tcp_sock->sol_callb = sol_callback;
  return (1);
}

#ifdef NOT_USED
/*
 * _sock_half_open -
 *   Return true if peer closed his side.
 *   There might still be data to read
 */
int _sock_half_open (const tcp_Socket *s)
{
  if (!s || s->ip_type == UDP_PROTO || s->ip_type == IP_TYPE)
     return (0);

  return (s->state >= tcp_StateFINWT1 &&
          s->state <= tcp_StateCLOSED);
}
#endif

#if 0  /* not finished */
/*
 * socketpair() - Create a pair of connected sockets.
 * Modified version based on Linux's version.
 */
int socketpair (int family, int type, int protocol, int usockvec[2])
{
  Socket *sock1, *sock2;
  int     s1, s2;

  if ((s1 = socket (family, type, protocol)) < 0)
     return (fd1);

  sock1 = socklist_find (s1);

  /* Now grab another socket and try to connect the two together.
   */
  if ((s2 = socket (family, type, protocol)) < 0)
  {
    close_s (s1);
    return (-EINVAL);
  }

  sock2 = socklist_find (s2);

  sock1->conn = sock2;
  sock2->conn = sock1;
  sock1->so_state = SS_CONNECTED;
  sock2->so_state = SS_CONNECTED;

  usockvec[0] = s1;
  usockvec[1] = s2;
  return (0);
}
#endif

#endif /* USE_BSD_FUNC */
