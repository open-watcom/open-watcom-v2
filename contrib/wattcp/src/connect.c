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

static int  tcp_connect  (Socket *socket);
static int  udp_connect  (Socket *socket);
static int  raw_connect  (Socket *socket);
static int  nblk_connect (Socket *socket);
static void set_keepalive(Socket *sock);

/*
 * connect
 *  "connect" will attempt to open a connection on a foreign IP address and
 *  foreign port address.  This is achieved by specifying the foreign IP
 *  address and foreign port number in the "servaddr".
 */

int connect (int s, const struct sockaddr *servaddr, int addrlen)
{
  struct   sockaddr_in *addr   = (struct sockaddr_in*) servaddr;
  struct   Socket      *socket = _socklist_find (s);
  volatile int rc;

  SOCK_PROLOGUE (socket, "\nconnect:%d", s);

  if (socket->so_type == SOCK_STREAM)
  {
    if (socket->so_state & SS_ISCONNECTED)
    {
      SOCK_DEBUGF ((socket, ", EISCONN"));
      SOCK_ERR (EISCONN);
      return (-1);
    }
    if (socket->so_options & SO_ACCEPTCONN)
    {
      SOCK_DEBUGF ((socket, ", EOPNOTSUPP (listen sock)"));
      SOCK_ERR (EOPNOTSUPP);
      return (-1);
    }
    if (IN_MULTICAST(ntohl(addr->sin_addr.s_addr)))
    {
      SOCK_DEBUGF ((socket, ", EINVAL (mc)"));
      SOCK_ERR (EINVAL);
      return (-1);
    }
  }

  if (!addr || addrlen < sizeof(*servaddr))
  {
    SOCK_DEBUGF ((socket, ", EINVAL"));
    SOCK_ERR (EINVAL);
    return (-1);
  } 

  if (socket->remote_addr)
  {
    if ((socket->so_type == SOCK_STREAM) &&
        (socket->so_state & SS_NBIO))
       return nblk_connect (socket);

    SOCK_DEBUGF ((socket, ", connect already done!"));
    SOCK_ERR (EISCONN);
    return (-1);
  }

  socket->remote_addr = SOCK_CALLOC (sizeof(*socket->remote_addr));
  if (!socket->remote_addr)
  {
    SOCK_DEBUGF ((socket, ", ENOMEM (rem)"));
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  socket->remote_addr->sin_family = AF_INET;
  socket->remote_addr->sin_port   = addr->sin_port;
  socket->remote_addr->sin_addr   = addr->sin_addr;


  if (!socket->local_addr)
  {
    SOCK_DEBUGF ((socket, ", auto-binding"));

    socket->local_addr = SOCK_CALLOC (sizeof(*socket->local_addr));
    if (!socket->local_addr)
    {
      free (socket->remote_addr);
      socket->remote_addr = NULL;
      SOCK_DEBUGF ((socket, ", ENOMEM (loc)"));
      SOCK_ERR (ENOMEM);
      return (-1);
    }
    socket->local_addr->sin_family      = AF_INET;
    socket->local_addr->sin_port        = htons (findfreeport(0,TRUE));
    socket->local_addr->sin_addr.s_addr = htonl (my_ip_addr);
  }

  SOCK_DEBUGF ((socket, ", loc/rem ports: %u/%u",
                ntohs(socket->local_addr->sin_port),
                ntohs(socket->remote_addr->sin_port)));


  /* Not safe to run sock_daemon() now
   */
  _sock_start_crit();

  /* Setup SIGINT handler now.
   */
  if (_sock_sig_setup() < 0)
  {
    SOCK_ERR (EINTR);
    rc = -1;
    goto connect_fail;
  }

  switch (socket->so_type)
  {
    case SOCK_STREAM:
         rc = tcp_connect (socket);
         break;

    case SOCK_DGRAM:
         rc = udp_connect (socket);
         break;

    case SOCK_RAW:
         rc = raw_connect (socket);
         break;

    default:
         SOCK_ERR (EPROTONOSUPPORT);
         rc = -1;
  }

connect_fail:
  _sock_stop_crit();
  _sock_sig_restore();
  return (rc);
}

/*
 *  Handle SOCK_DGRAM connection. Always blocking in _arp_resolve()
 */
static int udp_connect (Socket *socket)
{
  if (!_UDP_open (socket,
                  socket->remote_addr->sin_addr,
                  socket->local_addr->sin_port,
                  socket->remote_addr->sin_port))
  {
    SOCK_DEBUGF ((socket, ", no route"));
    SOCK_ERR (EHOSTUNREACH);
    STAT (ipstats.ips_noroute++);
    return (-1);
  }
  socket->so_state &= ~SS_UNCONNECTED;
  socket->so_state |=  SS_ISCONNECTED;
  set_keepalive (socket);
  return (0);
}

/*
 *  SOCK_RAW "connect" is very simple.
 */
static int raw_connect (Socket *socket)
{
  socket->so_state |= SS_ISCONNECTED;

  /* Note: _arp_resolve() is done in ip_transmit()
   */
  return (0);
}

/*
 *  Handle SOCK_STREAM blocking connection.
 */
static int tcp_connect (Socket *socket)
{
  DWORD timeout;
  int   status;

  if (!_TCP_open (socket,
                  socket->remote_addr->sin_addr,
                  socket->local_addr->sin_port,
                  socket->remote_addr->sin_port))
  {
    SOCK_DEBUGF ((socket, ", no route"));
    SOCK_ERR (EHOSTUNREACH);
    STAT (ipstats.ips_noroute++);
    return (-1);
  }

  /* Don't let tcp_Retransmitter() kill this socket
   * before our `socket->timeout' expires
   */
  socket->tcp_sock->locflags |= LF_RCVTIMEO;

  /* We're here only when connect() is called the 1st time
   * (blocking or non-blocking socket).
   */
  socket->so_state |= SS_ISCONNECTING;

  if (socket->so_state & SS_NBIO)
  {
    /* if user calls getsockopt(SO_ERROR) before calling connect() again
     */
    socket->so_error = EALREADY;   
    SOCK_DEBUGF ((socket, ", EINPROGRESS"));
    SOCK_ERR (EINPROGRESS);
    return (-1);
  }

  /* Handle blocking stream socket connect.
   * maybe we should use select_s() instead ?
   */
  timeout = set_timeout (1000 * socket->timeout);
  status  = _ip_delay0 ((sock_type*)socket->tcp_sock,
                        socket->timeout, NULL, NULL);


  /* We got an ICMP_UNREACH from our peer
   */
  if (socket->so_state & SS_CONN_REFUSED)
  {
    SOCK_DEBUGF ((socket, ", ECONNREFUSED"));
    SOCK_ERR (ECONNREFUSED);
    return (-1);
  }

  if (chk_timeout(timeout))
  {
    SOCK_DEBUGF ((socket, ", ETIMEDOUT\n"));
    socket->so_state &= ~SS_ISCONNECTING;
    SOCK_ERR (ETIMEDOUT);
    return (-1);
  }

  if (status < 0)
  {
    SOCK_DEBUGF ((socket, ", ECONNRESET"));
    socket->so_state &= ~SS_ISCONNECTING;
    SOCK_ERR (ECONNRESET);
    return (-1);
  }

  socket->so_state &= ~SS_UNCONNECTED;
  socket->so_state &= ~SS_ISCONNECTING;
  socket->so_state |=  SS_ISCONNECTED;
  set_keepalive (socket);
  return (0);
}


/*
 *  Handle non-blocking SOCK_STREAM connection.
 *  Only called on 2nd (3rd etc) time a non-blocking
 *  connect() is called.
 */
static int nblk_connect (Socket *socket)
{
  if (socket->so_state & SS_ISCONNECTED)
  {
    SOCK_DEBUGF ((socket, ", connected!"));
    set_keepalive (socket);
    socket->so_error = 0;
    return (0);
  }

  if ((socket->so_state & (SS_ISDISCONNECTING | SS_CONN_REFUSED)) ||
      (socket->tcp_sock->state >= tcp_StateCLOSED))
  {
    SOCK_DEBUGF ((socket, ", ECONNREFUSED"));
    socket->so_error = ECONNREFUSED;
    SOCK_ERR (ECONNREFUSED);           /* could also be ECONNRESET */
    return (-1);
  }

  if (socket->so_state & SS_ISCONNECTING)
  {
    SOCK_DEBUGF ((socket, ", EALREADY"));
    socket->so_error = EALREADY;       /* should be redundant */
    SOCK_ERR (EALREADY);
    return (-1);
  }

  SOCK_FATAL (("%s (%d) Fatal: Unhandled non-block event\r\n",
              __FILE__, __LINE__));
  return (-1);
}

/*
 * Sets keepalive timer on DGRAM/STREAM socket
 */
static void set_keepalive (Socket *sock)
{
  if ((sock->so_options & SO_KEEPALIVE) && tcp_keepalive)
     sock->keepalive = set_timeout (1000 * tcp_keepalive);
}

#endif /* USE_BSD_FUNC */
