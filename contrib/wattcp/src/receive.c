/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */


#include "socket.h"
#include "asmpkt.h"

#if defined(USE_BSD_FUNC)

static int tcp_receive (Socket *sock, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen);

static int udp_receive (Socket *sock, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen);

static int raw_receive (Socket *sock, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen);

/*
 * receive() flags:
 *  MSG_PEEK
 *  MSG_WAITALL
 *  MSG_OOB       (not yet supported)
 *  MSG_DONTROUTE (not yet supported)
 *  MSG_EOR       (not yet supported)
 *  MSG_TRUNC     (not yet supported)
 *  MSG_CTRUNC    (not yet supported)
 *
 *  Only one flags bit is handled at a time.
 */

static int receive (const char *func, int s, void *buf, int len, int flags,
                    struct sockaddr *from, int *fromlen)
{
  Socket *socket = _socklist_find (s);
  int     ret    = 0;
  char    fmt[30];

  fmt[0] = '\n';
  strcpy (fmt+1, func);
  strcat (fmt, ":%d");

  SOCK_PROLOGUE (socket, fmt, s);
  VERIFY_RW (buf, len);

  if (from && fromlen)
  {
    VERIFY_RW (from, *fromlen);
    if (*fromlen < sizeof(*from))
    {
      SOCK_DEBUGF ((socket, ", EADDRNOTAVAIL"));
      SOCK_ERR (EADDRNOTAVAIL);
      return (-1);
    }
  }

  if (socket->so_state & SS_CANTRCVMORE)
  {
    SOCK_DEBUGF ((socket, ", Can't recv more"));
    SOCK_ERR (ENOTCONN);  /* maybe EPIPE ? */
    return (-1);
  }

  if (socket->so_state & SS_CONN_REFUSED)
  {
    if (socket->so_error == ECONNRESET)  /* set in tcp_sockreset() */
    {
      SOCK_DEBUGF ((socket, ", ECONNRESET"));
      SOCK_ERR (ECONNRESET);
    }
    else
    {
      SOCK_DEBUGF ((socket, ", ECONNREFUSED (1)"));
      SOCK_ERR (ECONNREFUSED);
    }
    return (-1);
  }

  if ((flags & MSG_PEEK) && (flags & MSG_WAITALL))
  {
    SOCK_DEBUGF ((socket, ", invalid PEEK+WAITALL flags"));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  /* If application installs the same signal handlers we do, we must 
   * exit cracefully from below loops.
   */
  if (_sock_sig_setup() < 0)
  {
    SOCK_ERR (EINTR);
    ret = -1;
    goto recv_exit;
  }

  switch (socket->so_type)
  {
    case SOCK_STREAM:
         ret = tcp_receive (socket, buf, len, flags, from, fromlen);
         SOCK_DEBUGF ((socket, ", len=%d", ret));
         break;

    case SOCK_DGRAM:
         ret = udp_receive (socket, buf, len, flags, from, fromlen);
         SOCK_DEBUGF ((socket, ", len=%d", ret));
         break;
               
    case SOCK_RAW:
         ret = raw_receive (socket, buf, len, flags, from, fromlen);
         SOCK_DEBUGF ((socket, ", len=%d", ret));
         break;

#ifdef USE_LIBPCAP
    case SOCK_PACKET:
         if (pcap_next(_pcap_w32, &_pcap_w32_hdr))
              ret = _pcap_w32_hdr.caplen;
         else ret = 0;
         SOCK_DEBUGF ((socket, ", len=%d", ret));
         break;
#endif

    default:
         SOCK_DEBUGF ((socket, ", EPROTONOSUPPORT"));
         SOCK_ERR (EPROTONOSUPPORT);
         ret = -1;
  }

recv_exit:
  _sock_sig_restore();
  return (ret);
}

/*
 * recvfrom(): receive from socket 's'. Address (src-ip/port) is put
 *             in 'from' (if non-NULL)
 */
int recvfrom (int s, void *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
  return receive ("recvfrom", s, buf, len, flags, from, fromlen);
}

/*
 * recv(): receive data from socket 's'. Flags may be
 *         MSG_PEEK, MSG_OOB or MSG_WAITALL
 *         Normally used on SOCK_STREAM sockets.
 */
int recv (int s, void *buf, int len, int flags)
{
  return receive ("recv", s, buf, len, flags, NULL, NULL);
}

/*
 * read_s(): As above but no flags.
 */
int read_s (int s, char *buf, int len)
{
  return receive ("read_s", s, buf, len, 0, NULL, NULL);
}

/*
 * Fill in packet's address in 'from' and length in 'fromlen'.
 * Only used for UDP & Raw-IP. TCP have peer info in 'socket->remote_addr'.
 */
static void udp_raw_fill_from (struct sockaddr *from, int *fromlen,
                               struct in_addr  *peer, WORD port)
{
  struct sockaddr_in *sa = (struct sockaddr_in*) from;

  if (sa && fromlen && *fromlen >= sizeof(*sa))
  {
    sa->sin_addr   = *peer;
    sa->sin_family = AF_INET;
    sa->sin_port   = port;
    memset (&sa->sin_zero, 0, sizeof(sa->sin_zero));
  }
  if (fromlen)
     *fromlen = sizeof (*sa);
}


/*
 *  TCP receiver
 */
static int tcp_receive (Socket *socket, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen)
{
  int        ret   = 0;
  int        fin   = 0;    /* got FIN from peer */
  DWORD      timer = 0UL;
  sock_type *sk    = (sock_type*) socket->tcp_sock;

  if (!from && !socket->local_addr)
  {
    SOCK_DEBUGF ((socket, ", no local_addr"));
    SOCK_ERR (ENOTCONN);
    return (-1);
  }   

  if (socket->timeout && sock_inactive)
     timer = set_timeout (1000 * socket->timeout);

  while (1)
  {
    int ok = (tcp_tick(sk) != 0);

    tcp_Retransmitter (1);

    if (socket->so_state & SS_ISDISCONNECTING)   /* from 'fin' below */
       goto read_it;

#if 0  /* !to-do */
    if ((socket-so_options & SO_OOBINLINE) && urgent_data(sk))
    {
      ret = urgent_data_read (sk, (BYTE*)buf, len);
      break;
    }
#endif

    /* Don't do this for a listening socket
     */
    if (!(socket->so_options & SO_ACCEPTCONN))
    {
      if (sk->tcp.locflags & LF_GOT_FIN)         /* got FIN, no unACK data */
      {
        socket->so_state |=  SS_ISDISCONNECTING; /* We may receive more */
        socket->so_state &= ~SS_ISCONNECTED;     /* no longer ESTAB state */
        fin = 1;
        SOCK_DEBUGF ((socket, ", got FIN"));
        goto read_it;
      }

      if (!ok)
      {
        socket->so_state |= (SS_CANTRCVMORE | SS_ISDISCONNECTING);
        socket->so_state &= ~SS_ISCONNECTED;
        SOCK_DEBUGF ((socket, ", ENOTCONN"));
        SOCK_ERR (ENOTCONN);
        return (-1);
      }
    }

    if (sock_rbused(sk) > socket->recv_lowat)
    {
read_it:
      if (flags & MSG_PEEK)
           ret = sock_preread (sk, (BYTE*)buf, len);
      else if (flags & MSG_WAITALL)
           ret = sock_read    (sk, (BYTE*)buf, len);
      else ret = sock_fastread(sk, (BYTE*)buf, len);
      break;
    }

    if (socket->so_state & SS_CONN_REFUSED)
    {
      SOCK_DEBUGF ((socket, ", ECONNREFUSED (2)"));
      SOCK_ERR (ECONNREFUSED);
      return (-1);
    }

    if (socket->so_state & SS_NBIO)
    {
      SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
      SOCK_ERR (EWOULDBLOCK);
      return (-1);
    }

    if (chk_timeout(timer))
    {
      SOCK_DEBUGF ((socket, ", ETIMEDOUT"));
      SOCK_ERR (ETIMEDOUT);
      return (-1);
    }

    SOCK_YIELD();
  }

  if (ret > 0)
  {
    if (from)
       memcpy (from, socket->remote_addr, sizeof(*from));
    if (fromlen)
       *fromlen = sizeof (*from);
  }
  else if (ret < 0)    
  {
    if (fin)     /* A FIN and -1 from sock_xread() maps to 0 */
       ret = 0;
    else         /* else some buffer/socket error */
    {
      SOCK_DEBUGF ((socket, ", EIO"));
      SOCK_ERR (EIO);
    }
  }
  return (ret);
}


/*
 *  UDP receiver
 */
static int udp_receive (Socket *socket, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen)
{ 
  int   ret   = 0;
  DWORD timer = 0UL;

  if (socket->timeout)
     timer = set_timeout (1000 * socket->timeout);


#if 0  /* !!to-do: This needs a redesign */

  /* If bind() not called, allow data from anybody on any port !!
   */
  if (from && !socket->local_addr)
  {
    struct sockaddr_in addr = { AF_INET, 0, { INADDR_ANY }};

    socket->so_state |= SS_PRIV;

    if (_UDP_listen (socket, addr.sin_addr, addr.sin_port) < 0)
       return (-1);
  }
#endif

  while (1)
  {
    sock_type *sk = (sock_type*) socket->udp_sock;

    SOCK_YIELD();

    if (!tcp_tick(sk))
    {
      socket->so_state |= SS_CANTRCVMORE;
      SOCK_DEBUGF ((socket, ", ENOTCONN"));
      SOCK_ERR (ENOTCONN);
      return (-1);
    }

    tcp_Retransmitter (1);

    /* If this socket is for broadcast (or is unbound), check the
     * queue setup by sock_recv_init() (in _UDP_listen).
     * Note: it is possible to receive 0-byte probe packets.
     */
    if (socket->so_state & SS_PRIV)
    {
      struct in_addr peer;
      udp_Socket *udp  = socket->udp_sock;
      WORD        port = ntohs (socket->local_addr->sin_port);

      ret = sock_recv_from (udp, &peer.s_addr, &port, buf, len,
                            (flags & MSG_PEEK) ? 1 : 0);

      if (ret != 0 && peer.s_addr)
      {
        udp_raw_fill_from (from, fromlen, &peer, port);
        SOCK_DEBUGF ((socket, ", remote: %s (%d)",
                      inet_ntoa(peer), ntohs(port)));
        if (ret < 0)   /* 0-byte probe */
           return (0);
        return (ret);
      }
    }

    else if (sock_rbused(sk) > socket->recv_lowat)
    {
      if (flags & MSG_PEEK)
           ret = sock_preread  (sk, (BYTE*)buf, len);
      else if (flags & MSG_WAITALL)
           ret = sock_read     (sk, (BYTE*)buf, len);
      else ret = sock_fastread (sk, (BYTE*)buf, len);
      break;
    }

    if (socket->so_state & SS_CONN_REFUSED)
    {
      SOCK_DEBUGF ((socket, ", ECONNREFUSED (2)"));
      SOCK_ERR (ECONNREFUSED);
      return (-1);
    }

    if (socket->so_state & SS_NBIO)
    {
      SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
      SOCK_ERR (EWOULDBLOCK);
      return (-1);
    }

    if (chk_timeout(timer))
    {
      SOCK_DEBUGF ((socket, ", ETIMEDOUT"));
      SOCK_ERR (ETIMEDOUT);
      return (-1);
    }
  }

  if (ret > 0)
  {
    struct in_addr peer;
    WORD   port;

    port = htons (socket->udp_sock->hisport);
    peer.s_addr = htonl (socket->udp_sock->hisaddr);

    udp_raw_fill_from (from, fromlen, &peer, port);

    if (socket->remote_addr)
    {
      socket->remote_addr->sin_family = AF_INET;
      socket->remote_addr->sin_addr   = peer;
      socket->remote_addr->sin_port   = port;
    }

    SOCK_DEBUGF ((socket, ", remote: %s (%d)",
                  inet_ntoa(peer), ntohs(port)));
  }
  return (ret);
}


/*
 * Raw-IP receiver. Doesn't handle IP-options yet.
 */
static int raw_receive (Socket *socket, void *buf, int len, int flags,
                        struct sockaddr *from, int *fromlen)
{
  raw_Socket  *raw = socket->raw_sock;
  DWORD        timer;
  static DWORD loop;

  if (!raw || len < sizeof(raw->ip))
  {
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (socket->timeout)
       timer = set_timeout (1000 * socket->timeout);
  else timer = 0;
  loop = 1;

  while (1)
  {
    struct in_addr peer;
    struct ip     *ip;
    int    ip_len;

    /* give sock_rbused() and memcpy() below a chance to run
     * before sock_raw_recv() (via tcp_tick) drops a packet
     */
    if (loop++ > 1)
    {
      tcp_tick (NULL);
      tcp_Retransmitter (1);
    }

    ip_len = sock_rbused ((sock_type*)raw);  /* includes header length */
    if (ip_len >= sizeof(*ip) + socket->recv_lowat)
    {
      if (len < sizeof(*ip))
      {
        raw->used = FALSE;
        continue;
      }
      ip = (struct ip*) buf;

      /* SOCK_RAW shall allway return IP-header and data in 'buf'
       */
      memcpy (ip, &raw->ip, sizeof(*ip));
      len = min (ip_len-sizeof(*ip), len);
      if (len > 0)
         memcpy (++ip, &raw->data, len);

      peer.s_addr = raw->ip.source;
      raw->used = FALSE;

      udp_raw_fill_from (from, fromlen, &peer, 0);
      SOCK_DEBUGF ((socket, ", remote: %s", inet_ntoa(peer)));
      return (len + sizeof(*ip));
    }

    if (socket->so_state & SS_NBIO)
    {
      SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
      SOCK_ERR (EWOULDBLOCK);
      break;
    }

    if (chk_timeout(timer))
    {
      SOCK_DEBUGF ((socket, ", ETIMEDOUT"));
      SOCK_ERR (ETIMEDOUT);
      break;
    }

    SOCK_YIELD();
  }

  ARGSUSED (flags);
  return (-1);
}

#endif  /* USE_BSD_FUNC */
