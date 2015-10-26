/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

static int ip_transmit   (Socket *socket, const void *buf, int len);
static int udp_transmit  (Socket *socket, const void *buf, int len);
static int tcp_transmit  (Socket *socket, const void *buf, int len, int flags);
static int setup_udp_raw (Socket *socket, const struct sockaddr *to, int tolen);

static int transmit (const char *func, int s, const void *buf, int len,
                     int flags, const struct sockaddr *to, int tolen);

int sendto (int s, const void *buf, int len, int flags, const struct sockaddr *to, int tolen)
{
  return transmit ("sendto", s, buf, len, flags, to, tolen);
}

int send (int s, const void *buf, int len, int flags)
{
  return transmit ("send", s, buf, len, flags, NULL, 0);
}

int write_s (int s, const char *buf, int nbyte)
{
  return transmit ("write_s", s, buf, nbyte, 0, NULL, 0);
}

int writev_s (int s, const struct iovec *vector, size_t count)
{
  char   *buffer, *bp;
  size_t  i, to_copy, bytes = 0;

  /* Find the total number of bytes to write
   */
  for (i = 0; i < count; i++)
      bytes += vector[i].iov_len;

  if (bytes == 0)
     return (0);

  /* Allocate a temporary buffer to hold the data
   */
  buffer = alloca (bytes);
  if (!buffer)
  {
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  to_copy = bytes;
  bp = buffer;

  /* Copy the data into buffer.
   */
  for (i = 0; i < count; ++i)
  {
    size_t copy = min (vector[i].iov_len, to_copy);

    memcpy (bp, vector[i].iov_base, copy);
    bp      += copy;
    to_copy -= copy;
    if (to_copy == 0)
       break;
  }
  return transmit ("writev_s", s, (const void*)buffer, bytes, 0, NULL, 0);
}

/*
 * Close socket if MSG_EOR specified in flags.
 */
static __inline void msg_eor_close (Socket *socket)
{
  switch (socket->so_type)
  {
    case SOCK_STREAM:
         socket->so_state |= SS_CANTSENDMORE;
         sock_close ((sock_type*)socket->tcp_sock);
         break;
    case SOCK_DGRAM:
         socket->so_state |= SS_CANTSENDMORE;
         sock_close ((sock_type*)socket->udp_sock);
         break;
    case SOCK_RAW:
         socket->so_state |= SS_CANTSENDMORE;
         break;
  }
}

/*
 * transmit() flags:
 *   MSG_DONTROUTE                                     (not supported)
 *   MSG_EOR       Close sending side after data sent
 *   MSG_TRUNC                                         (not supported)
 *   MSG_CTRUNC                                        (not supported)
 *   MSG_OOB                                           (not supported)
 *   MSG_WAITALL   Wait till room in tx-buffer         (not supported)
 */
static int transmit (const char *func, int s, const void *buf, int len,
                     int flags, const struct sockaddr *to, int tolen)
{
  Socket *socket = _socklist_find (s);
  int     rc;

  SOCK_DEBUGF ((socket, "\n%s:%d, len=%d", func, s, len));

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

  if (socket->so_type == SOCK_STREAM ||      /* TCP-socket or */
      (socket->so_state & SS_ISCONNECTED))   /* "connected" udp/raw */
  {
    /* Note: SOCK_RAW doesn't really need a local address/port, but
     * makes the code more similar for all socket-types.
     * Disadvantage is that SOCK_RAW ties up a local port and a bit
     * more memory.
     */

    if (!socket->local_addr)
    {
      SOCK_DEBUGF ((socket, ", no local_addr"));
      SOCK_ERR (ENOTCONN);
      return (-1);
    }

    if (!socket->remote_addr)
    {
      SOCK_DEBUGF ((socket, ", no remote_addr"));
      SOCK_ERR (ENOTCONN);
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
        SOCK_DEBUGF ((socket, ", ECONNREFUSED"));
        SOCK_ERR (ECONNREFUSED);
      }
      return (-1);
    }
  }

  /* connectionless protocol setup
   */
  if (socket->so_type == SOCK_DGRAM || socket->so_type == SOCK_RAW)
  {
    if (!to || tolen < sizeof(*to))
    {
      SOCK_DEBUGF ((socket, ", no to-addr"));
      SOCK_ERR (EINVAL);
      return (-1);
    }
    if (setup_udp_raw(socket,to,tolen) < 0)
       return (-1);
  }

  VERIFY_RW (buf, len);


  /* Setup SIGINT handler now.
   */
  if (_sock_sig_setup() < 0)
  {
    SOCK_ERR (EINTR);
    return (-1);
  }

  switch (socket->so_type)
  {
    case SOCK_DGRAM:
         rc = udp_transmit (socket, buf, len);
         break;

    case SOCK_STREAM:
         rc = tcp_transmit (socket, buf, len, flags);
         break;

    case SOCK_RAW:
         rc = ip_transmit (socket, buf, len);
         break;

    default:
         SOCK_DEBUGF ((socket, ", EPROTONOSUPPORT"));
         SOCK_ERR (EPROTONOSUPPORT);
         rc = -1;
  }

  _sock_sig_restore();

  if (rc >= 0 && (flags & MSG_EOR))
     msg_eor_close (socket);

  return (rc);
}


/*
 *  Setup remote_addr for SOCK_RAW/SOCK_DGRAM (connectionless) protocols.
 *  Must reconnect socket if 'remote_addr' and 'to' address are different.
 *  I.e we're sending to another host/port than last time.
 */
static int setup_udp_raw (Socket *socket, const struct sockaddr *to, int tolen)
{
  struct sockaddr_in *peer = (struct sockaddr_in*) to;
  DWORD  keepalive = socket->keepalive;
  WORD   lport     = 0;
  char  *rdata     = NULL;
  int    rc;

  if (socket->so_state & SS_ISCONNECTED)
  {
    /* Don't reconnect if same peer address/port.
     */
    if (peer->sin_addr.s_addr == socket->remote_addr->sin_addr.s_addr &&
        peer->sin_port        == socket->remote_addr->sin_port)
       return (1);

    SOCK_DEBUGF ((socket, ", reconnecting"));

    free (socket->remote_addr);
    socket->remote_addr = NULL;

    /* Clear any effect of previous ICMP errors etc.
     */
    socket->so_state &= ~(SS_CONN_REFUSED|SS_CANTSENDMORE|SS_CANTRCVMORE);
    socket->so_error  = 0;

    if (socket->so_type == SOCK_DGRAM)
    {
      lport = socket->udp_sock->myport;
      rdata = (char*)socket->udp_sock->rdata;  /* preserve current data */
    }
  }

  /* For SOCK_DGRAM, udp_close() will be called when (re)opening socket.
   */
  _sock_enter_scope();
  rc = connect (socket->fd, to, tolen);
  _sock_leave_scope();

  if (rc < 0)
     return (-1);


  if (rdata)  /* Must be SOCK_DGRAM */
  {
    udp_Socket *udp = socket->udp_sock;

    free (udp->rdata);                /* free new rx-buffer set in connect() */
    udp->rdata       = (BYTE*) rdata; /* reuse previous data buffer */
    udp->maxrdatalen = DEFAULT_RCV_WIN;

    grab_localport (lport);   /* Restore freed localport */
  }

  /* restore keepalive timer changed in connect()
   */
  socket->keepalive = keepalive;
  return (1);
}

/*
 * Check for enough room in Tx-buffer for a non-blocking socket
 * to transmit without waiting. Only called for SOCK_DGRAM/SOCK_STREAM
 * sockets.
 *
 * If '*len > room', modify '*len' on output to 'room' (the size of
 * bytes left in tx-buf).
 */
static __inline int check_non_block_tx (Socket *socket, int *len)
{
  sock_type *sk;
  int        room;

  if (socket->so_type == SOCK_DGRAM)
       sk = (sock_type*) socket->udp_sock;
  else sk = (sock_type*) socket->tcp_sock;

  room = sock_tbleft (sk);
  if (*len <= room)
     return (0);     /* okay, enough room, '*len' unmodified */

#if 0
  SOCK_YIELD();      /* a small delay to clear up things */
  tcp_tick (sk);

  room = sock_tbleft (sk);
  if (*len <= room)
     return (0);
#endif

  /* Still no room, but cannot split up datagrams (only in ip-fragments)
   */
  if (socket->so_type == SOCK_DGRAM) 
     return (-1);

  /* stream: Tx room below (or equal) low-water mark is failure.
   */
  if (*len > 0 && room <= socket->send_lowat)
     return (-1);

  /* streams may be split up, modify '*len'
   */
  *len = room;
  return (0);
}

/*
 *  TCP transmitter
 */
static int tcp_transmit (Socket *socket, const void *buf, int len, int flags)
{
  sock_type *sk = (sock_type*)socket->tcp_sock;

  tcp_tick (sk);
  tcp_Retransmitter (1);

  if (sk->tcp.state < tcp_StateESTAB || sk->tcp.state >= tcp_StateLASTACK)
  {
    socket->so_state |= SS_CANTSENDMORE;
    SOCK_DEBUGF ((socket, ", ENOTCONN (%s)",  /* !! or EPIPE */
                 (sk->tcp.locflags & LF_GOT_FIN) ?
                   "got FIN" : "can't send"));
    SOCK_ERR (ENOTCONN);
    return (-1);
  }

  if (socket->so_state & SS_NBIO)
  {
    int in_len = len;

    if (check_non_block_tx(socket,&len) < 0)
    {
      SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
      SOCK_ERR (EWOULDBLOCK);
      return (-1);
    }
    if (in_len != len)
       SOCK_DEBUGF ((socket, " [%d]", len)); /* trace "len=x [y]" */
  }

  SOCK_DEBUGF ((socket, ", %s (%d) / TCP",
                inet_ntoa(socket->remote_addr->sin_addr),
                ntohs(socket->remote_addr->sin_port)));

#if 0
  /* Must wait for room in send buffer
   */
  if ((flags & MSG_WAITALL) || len > sock_tbleft(sk))
       len = sock_write (sk, (BYTE*)buf, len);
  else len = sock_fastwrite (sk, (BYTE*)buf, len);

#else
  /* This is more efficient. The above sock_fastwrite() would
   * effectively turn off Nagle's algorithm.
   */
  ARGSUSED (flags);
  len = sock_write (sk, (BYTE*)buf, len);
#endif

  if (len <= 0)    /* error in tcp_write() */
  {
    if (sk->tcp.state != tcp_StateESTAB)
    {
      SOCK_DEBUGF ((socket, ", ENOTCONN"));
      SOCK_ERR (ENOTCONN);   /* maybe EPIPE? */
    }
    else
    {
      SOCK_DEBUGF ((socket, ", ENETDOWN"));
      SOCK_ERR (ENETDOWN);
    }
    return (-1);
  }
  return (len);
}


/*
 *  UDP transmitter
 */
static int udp_transmit (Socket *socket, const void *buf, int len)
{
  sock_type *sk   = (sock_type*) socket->udp_sock;
  u_long     dest = socket->remote_addr->sin_addr.s_addr;
  int        tx_room, rc;
  int        is_bcast, is_multi;

  if (!tcp_tick(sk))
  {
    socket->so_state |= SS_CANTSENDMORE;
    SOCK_DEBUGF ((socket, ", ENOTCONN (can't send)")); /* !! or EPIPE */
    SOCK_ERR (ENOTCONN);
    return (-1);
  }
  tcp_Retransmitter (1);

  if ((socket->so_state & SS_NBIO) && check_non_block_tx(socket,&len) < 0)
  {
    SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
    SOCK_ERR (EWOULDBLOCK);
    return (-1);
  }

  is_bcast = (dest == INADDR_BROADCAST || dest == INADDR_ANY);
  is_multi = IN_MULTICAST (ntohl(socket->remote_addr->sin_addr.s_addr));

  SOCK_DEBUGF ((socket, ", %s (%d) / UDP %s",
                inet_ntoa(socket->remote_addr->sin_addr),
                ntohs(socket->remote_addr->sin_port),
                is_multi ? "(mc)" : ""));

  if (len == 0)   /* 0-byte probe packet */
     return ip_transmit (socket, NULL, 0);

  tx_room = sock_tbleft (sk);  /* always MTU-28 */

  /* Special tests for broadcast messages
   */
  if (is_bcast)
  {
    if (len > tx_room)        /* don't allow fragments */
    {
      SOCK_DEBUGF ((socket, ", EMSGSIZE"));
      SOCK_ERR (EMSGSIZE);
      STAT (ipstats.ips_odropped++);
      return (-1);
    }
    if (_pktserial)           /* Link-layer doesn't allow broadcast */
    {
      SOCK_DEBUGF ((socket, ", EADDRNOTAVAIL"));
      SOCK_ERR (EADDRNOTAVAIL);
      STAT (ipstats.ips_odropped++);
      return (-1);
    }
  }

  /* set new TTL if setsockopt() used before sending to Class-D socket
   */
  if (is_multi)
     sk->udp.ttl = socket->ip_ttl;

#if defined(USE_FRAGMENTS)
  if ((long)len > USHRT_MAX - sizeof(udp_Header))
  {
    SOCK_DEBUGF ((socket, ", EMSGSIZE"));
    SOCK_ERR (EMSGSIZE);
    STAT (ipstats.ips_toolong++);
    return (-1);
  }

  if (len > tx_room)
     return SEND_IP_FRAGMENTS (sk, UDP_PROTO, dest, buf, len);
#endif

  rc = sock_write (sk, (BYTE*)buf, len);
  if (rc <= 0)    /* error in udp_write() */
  {
    SOCK_DEBUGF ((socket, ", ENETDOWN"));
    SOCK_ERR (ENETDOWN);
    return (-1);
  }
  return (rc);
}

/*
 *  Raw IP transmitter
 */
static int ip_transmit (Socket *socket, const void *tx, int len)
{
  eth_address eth;
  u_long      dest;
  unsigned    tx_len, tx_room;
  sock_type  *sk = (sock_type*)socket->udp_sock;

  struct ip   *ip  = (struct ip*) tx;
  const  BYTE *buf = (const BYTE*) tx;
  WORD   flags     = 0;
  DWORD  offset;
  UINT   h_len, o_len;

  tcp_tick (NULL);        /* process other TCBs too */
  tcp_Retransmitter (1);

  /* This should never happen
   */
  if (ip && (socket->so_state & SS_NBIO) &&
      sock_tbleft(sk) < (len + socket->send_lowat))
  {
    SOCK_DEBUGF ((socket, ", EWOULDBLOCK"));
    SOCK_ERR (EWOULDBLOCK);
    return (-1);
  }

  if (ip)
  {
    offset = ntohs (ip->ip_off);
    flags  = offset & ~IP_OFFMASK;
    offset = (offset & IP_OFFMASK) << 3; /* 0 <= ip_ofs <= 65536-8 */
  }

  SOCK_DEBUGF ((socket, ", %s / Raw",
                inet_ntoa(socket->remote_addr->sin_addr)));

  if (ip && (socket->inp_flags & INP_HDRINCL))
  {
    dest    = ip->ip_dst.s_addr;
    tx_len  = len;
    tx_room = mtu;
  }
  else
  {
    dest    = socket->remote_addr->sin_addr.s_addr;
    tx_len  = len + sizeof (*ip);
    tx_room = mtu + sizeof (*ip);
  }

  if (!dest || !_arp_resolve(ntohl(dest),&eth,0))
  {
    SOCK_DEBUGF ((socket, ", no route"));
    SOCK_ERR (EHOSTUNREACH);
    STAT (ipstats.ips_noroute++);
    return (-1);
  }

#if defined(USE_FRAGMENTS)
  if (!(socket->inp_flags & INP_HDRINCL) &&
      tx_len + socket->ip_opt_len > tx_room)
  {
    sk = (sock_type*)socket->raw_sock;

    if (flags & IP_DF)
    {
      SOCK_DEBUGF ((socket, ", EMSGSIZE"));
      SOCK_ERR (EMSGSIZE);
      STAT (ipstats.ips_toolong++);
      return (-1);
    }
    return SEND_IP_FRAGMENTS (sk, sk->raw.ip_type, dest, buf, len);
  }
#else
  if (!(socket->inp_flags & INP_HDRINCL) &&
      tx_len + socket->ip_opt_len > tx_room)
  {
    SOCK_DEBUGF ((socket, ", EMSGSIZE"));
    SOCK_ERR (EMSGSIZE);
    STAT (ipstats.ips_toolong++);
    return (-1);
  }
#endif

  ip = (struct ip*) _eth_formatpacket (&eth, IP_TYPE);

  if (socket->inp_flags & INP_HDRINCL)
  {
    memcpy (ip, buf, len);
    if (ip->ip_src.s_addr == 0)
    {
      ip->ip_src.s_addr = gethostid();
      ip->ip_sum = 0;
      ip->ip_sum = ~checksum ((void*)ip, ip->ip_hl << 2);
    }
    if (ip->ip_sum == 0)
        ip->ip_sum = ~checksum ((void*)ip, ip->ip_hl << 2);
  }
  else
  {
    if (socket->ip_opt && socket->ip_opt_len > 0)
    {
      BYTE *data;

      o_len = min (socket->ip_opt_len, sizeof(socket->ip_opt->ip_opts));
      h_len = sizeof(*ip) + o_len;
      data  = (BYTE*)ip + h_len;
      memcpy (ip+1, &socket->ip_opt->ip_opts, o_len);
      memcpy (data, buf, len);
      tx_len += o_len;
      if (socket->ip_opt->ip_dst.s_addr)   /* using source routing */
         dest = socket->ip_opt->ip_dst.s_addr;
    }
    else
    {
      if (buf)
         memcpy (ip+1, buf, len);
      h_len = sizeof (*ip);
    }

    ip->ip_v   = IPVERSION;
    ip->ip_hl  = h_len >> 2;
    ip->ip_tos = socket->ip_tos;
    ip->ip_len = htons (tx_len);
    ip->ip_id  = _get_ip_id();
    ip->ip_off = 0;
    ip->ip_ttl = socket->ip_ttl;
    ip->ip_p   = socket->so_proto;

    ip->ip_src.s_addr = gethostid();
    ip->ip_dst.s_addr = dest;

    ip->ip_sum = 0;
    ip->ip_sum = ~checksum (ip, h_len);
  }

  DEBUG_TX (NULL, ip);

  if (!_eth_send(tx_len))
  {
    SOCK_DEBUGF ((socket, ", ENETDOWN"));
    SOCK_ERR (ENETDOWN);
    return (-1);
  }
  if (buf)
     buf += tx_len;

  return (len);
}
#endif  /* USE_BSD_FUNC */
