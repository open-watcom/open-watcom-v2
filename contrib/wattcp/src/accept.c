/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 *   0.6 : Sep 16, 1999 : fixes by Claus Oberste-Brandenburg
 *   0.7 : Nov 22, 1999 : G. Vanem - simplified the socket 'dup' action.
 *                        Simply allocate a new 'remote_addr'.
 *                        Poll backlogged listen-queue in accept-loop.
 *   0.8 : Dec 15, 1999 : Listen-queue filled in _sock_append() and TCB
 *                        is cloned when SYN is received in tcp_listen_state().
 *   0.9 : Mar 09, 2000 : Plugged a memory leak in dup_bind() where 'tcp_sock'
 *                        memory from socket() wasn't free'ed. Thanks to
 *                        Francisco Pastor <fpastor.etra-id@etra.es> for
 *                        finding this.
 *   0.91: Jun 01, 2000 : Rearranged acccept-loop for EWOULDBLOCK with no
 *                        listen_queue element (dropped 'continue' construct).
 */

#include "socket.h"

#if defined(USE_BSD_FUNC)

static int dup_bind   (Socket *socket, Socket **clone, int idx);
static int alloc_addr (Socket *socket, Socket  *clone);
static int listen_free(Socket *socket, int idx);


int accept (int s, struct sockaddr *addr, int *addrlen)
{
  Socket  *clone, *socket;
  volatile DWORD   timeout;
  volatile int     newsock = -1;
  volatile int     que_idx;
  volatile int     maxconn;

  socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\naccept:%d", s);

  if (!socket->local_addr)
  {
    SOCK_DEBUGF ((socket, ", not bound"));
    SOCK_ERR (ENOTCONN);
    return (-1);
  }

  if (socket->so_type != SOCK_STREAM)
  {
    SOCK_DEBUGF ((socket, ", EOPNOTSUPP"));
    SOCK_ERR (EOPNOTSUPP);
    return (-1);
  }

  if (!(socket->so_options & SO_ACCEPTCONN)) /* listen() not called */
  {
    SOCK_DEBUGF ((socket, ", not SO_ACCEPTCONN"));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (!(socket->so_state & (SS_ISLISTENING | SS_ISCONNECTING)))
  {
    SOCK_DEBUGF ((socket, ", not listening"));
    SOCK_ERR (ENOTCONN);
    return (-1);
  }

  if (addr && addrlen)
  {
    if (*addrlen < sizeof(*addr))
    {
      SOCK_DEBUGF ((socket, ", EFAULT"));
      SOCK_ERR (EFAULT);
      return (-1);
    }
    VERIFY_RW (addr, *addrlen);
  }

  /* Get max possible TCBs on listen-queue.
   * Some (or all) may be NULL until a SYN comes in.
   */
  maxconn = socket->backlog;
  if (maxconn < 1 || maxconn > SOMAXCONN)
  {
    SOCK_FATAL (("%s(%d): Illegal socket backlog %d",
                __FILE__, __LINE__, maxconn));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (socket->timeout)
       timeout = set_timeout (1000 * socket->timeout);
  else timeout = 0UL;


  if (_sock_sig_setup() < 0)
  {
    SOCK_ERR (EINTR);
    goto accept_fail;
  }

  /* Loop over all queue-slots and accept first connected TCB
   */
  for (que_idx = 0; ; que_idx = (++que_idx % maxconn))
  {
    tcp_Socket *sk = socket->listen_queue [que_idx];

    tcp_tick (NULL);

    SOCK_YIELD();

    /* No SYNs received yet. This shouldn't happen if we called 'accept()'
     * after 'select_s()' said that socket was readable. (At least one
     * connection on the listen-queue).
     */
    if (sk)
    {
      /* This could happen if 'accept()' was called too long after connection
       * was established and then closed by peer. This could also happen if
       * someone did a portscan on us. I.e. he sent 'SYN', we replied with
       * 'SYN+ACK' and he never sent an 'ACK'. Thus we timeout in
       * 'tcp_Retransmitter()' and abort the TCB.
       *
       * Queue slot is in any case ready for another 'SYN' to come and be
       * handled by '_sock_append()'.
       */
      if (sk->state >= tcp_StateLASTACK && sk->ip_type == 0)
      {
        SOCK_DEBUGF ((socket, ", aborted TCB (idx %d)", que_idx));
        listen_free (socket, que_idx);
        continue;
      }

      /* !!to-do: Should maybe loop over all maxconn TCBs and accept the
       *          one with oldest 'syn_timestamp'.
       */
      if (tcp_established(sk))
      {
        SOCK_DEBUGF ((socket, ", connected! (idx %d)", que_idx));
        break;
      }
    }

    /* We've polled all listen-queue slots and none are connected.
     * Return fail if socket is non-blocking.
     */
    if (que_idx == maxconn-1 && (socket->so_state & SS_NBIO))
    {
      SOCK_DEBUGF ((socket, ", would block"));
      SOCK_ERR (EWOULDBLOCK);
      goto accept_fail;
    }

    if (chk_timeout(timeout))
    {
      SOCK_DEBUGF ((socket, ", ETIMEDOUT"));
      SOCK_ERR (ETIMEDOUT);
      goto accept_fail;
    }
  }

  /* We're here only when above 'tcp_established()' succeeded.
   * Now duplicate 'socket' into a new listening socket 'clone'
   * with handle 'newsock'.
   */
  _sock_enter_scope();
  newsock = dup_bind (socket, &clone, que_idx);
  if (newsock < 0)
     goto accept_fail;

  if (alloc_addr(socket, clone) < 0)
  {
    SOCK_DEL_FD (newsock);
    goto accept_fail;
  }

  /* Clone is connected, but *not* listening/accepting.
   * Note: other 'so_state' bits from parent is unchanged.
   *       e.g. clone may be non-blocking.
   */
  clone->so_state   |=  SS_ISCONNECTED;
  clone->so_state   &= ~(SS_ISLISTENING | SS_ISCONNECTING);
  clone->so_options &= ~SO_ACCEPTCONN;

  /* Prevent a PUSH on first segment sent.
   */
  sock_noflush ((sock_type*)clone->tcp_sock);

  SOCK_DEBUGF ((clone, "\nremote %s (%d)",
                inet_ntoa (clone->remote_addr->sin_addr),
                ntohs (clone->remote_addr->sin_port)));

  if (addr && addrlen)
  {
    struct sockaddr_in *sa = (struct sockaddr_in*)addr;

    sa->sin_family = AF_INET;
    sa->sin_port   = clone->remote_addr->sin_port;
    sa->sin_addr   = clone->remote_addr->sin_addr;
    memset (sa->sin_zero, 0, sizeof(sa->sin_zero));
    *addrlen = sizeof(*sa);
  }

  _sock_leave_scope();
  _sock_sig_restore();
  return (newsock);

accept_fail:
  _sock_leave_scope();
  _sock_sig_restore();
  return (-1);
}


/*
 *  Duplicate a SOCK_STREAM 'socket' to 'newconn'. Doesn't set
 *  local/remote addresses. Transfer TCB from listen-queue[idx] of
 *  'socket' to TCB of 'clone'.
 */
static int dup_bind (Socket *sock, Socket **newconn, int idx)
{
  int fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (fd >= 0)
  {
    Socket *clone = _socklist_find (fd);

    /* child gets state from parent
     */
    clone->timeout    = sock->timeout;
    clone->close_time = sock->close_time;
    clone->keepalive  = sock->keepalive;
    clone->ip_tos     = sock->ip_tos;
    clone->ip_ttl     = sock->ip_ttl;
    clone->so_state   = sock->so_state;
    clone->so_options = sock->so_options;

    /* TCB for clone is from listen-queue[idx]; free tcp_sock from
     * socket(). Reuse listen-queue slot for another SYN.
     */
    free (clone->tcp_sock);
    clone->tcp_sock = sock->listen_queue[idx];
    sock->listen_queue [idx] = NULL;
    sock->syn_timestamp[idx] = 0;
    *newconn = clone;
  }
  return (fd);
}

/*
 *  Allocate and fill local/remote addresses for 'clone'.
 *  Take local address from 'socket', and remote address from
 *  TCB of clone.
 */
static int alloc_addr (Socket *socket, Socket *clone)
{
  struct in_addr peer;

  clone->local_addr  = SOCK_CALLOC (sizeof(*clone->local_addr));
  clone->remote_addr = SOCK_CALLOC (sizeof(*clone->remote_addr));

  if (!clone->local_addr || !clone->remote_addr)
  {
    SOCK_DEBUGF ((socket, ", ENOMEM"));
    SOCK_ERR (ENOMEM);
    return (-1);
  }

  peer.s_addr = htonl (clone->tcp_sock->hisaddr);
  clone->local_addr->sin_family  = AF_INET;
  clone->local_addr->sin_port    = socket->local_addr->sin_port;
  clone->local_addr->sin_addr    = socket->local_addr->sin_addr;

  clone->remote_addr->sin_family = AF_INET;
  clone->remote_addr->sin_port   = htons (clone->tcp_sock->hisport);
  clone->remote_addr->sin_addr   = peer;
  return (0);
}

/*
 * Release a listen-queue slot and associated memory.
 */
static int listen_free (Socket *socket, int idx)
{
  tcp_Socket *tcb = socket->listen_queue [idx];

  _tcp_unthread (tcb);

  if (tcb->rdata != &tcb->rddata[0])  /* free large Rx buffer? */
     free (tcb->rdata);
  free (tcb);
  socket->listen_queue [idx] = NULL;
  return (0);
}

/*
 *  Called from tcp_fsm.c / tcp_listen_state() (via _tcp_syn_hook) to
 *  append a new connection to the listen-queue of socket 'sock'.
 *
 *  TCB on input ('orig') has received a SYN. Replace TCB on output
 *  with a cloned TCB that we append to the listen-queue and eventually
 *  is used by accept() to create a new socket.
 *
 *  TCB on input ('orig') must still be listening for further connections
 *  on the same port as specified in call to _TCP_listen().
 */
int _sock_append (tcp_Socket **tcp)
{
  tcp_Socket *clone;
  tcp_Socket *orig = *tcp;
  Socket     *sock = NULL;   /* associated socket for '*tcp' */
  int         i;

  /* Lookup BSD-socket for Wattcp TCB
   */
  if (!_tcp_find_hook || (sock = (*_tcp_find_hook)(orig)) == NULL)
  {
    SOCK_DEBUGF ((NULL, "\n  sock_append: not found!?"));
    return (0);  /* i.e. could be a native Wattcp socket */
  }

  SOCK_DEBUGF ((sock, "\n  sock_append:%d", sock->fd));

  if (!(sock->so_options & SO_ACCEPTCONN))
  {
    SOCK_DEBUGF ((sock, ", not SO_ACCEPTCONN"));
    return (-1);  /* How could this happen? */
  }

  /* Find the first vacant slot for this clone
   */
  for (i = 0; i < sock->backlog; i++)
      if (!sock->listen_queue[i])
         break;

  if (i >= sock->backlog || i >= SOMAXCONN)
  {
    /* !!to-do: drop the oldest (or a random) slot in the listen-queue.
     */
    SOCK_DEBUGF ((sock, ", queue full (idx %d)", i));
    return (-1);
  }

  SOCK_DEBUGF ((sock, ", idx %d", i));

  clone = SOCK_CALLOC (sizeof(*clone));
  if (!clone)
  {
    SOCK_DEBUGF ((sock, ", ENOMEM"));
    return (-1);
  }

  /* Link in the semi-connected socket (SYN received, ACK will be sent)
   */
  sock->listen_queue[i]  = clone;
  sock->syn_timestamp[i] = set_timeout (0);

  /* Copy the TCB (except Tx-buffer) to clone
   */
  memcpy (clone, orig, sizeof(*clone) - sizeof(clone->data));
  clone->safetytcp = SAFETYTCP;

  /* Increase the TCP window (to 16kB)
   */
  sock_setbuf ((sock_type*)clone, calloc(DEFAULT_RCV_WIN,1), DEFAULT_RCV_WIN);

  /* Undo what tcp_handler() and tcp_listen_state() did to
   * this listening socket.
   */
  orig->hisport = 0;
  orig->hisaddr = 0;
  orig->myaddr  = 0;

  orig->seqnum  = INIT_SEQ();   /* set new ISS */
  orig->unhappy = FALSE;
  CLR_PEER_MAC_ADDR (orig);

#if defined(USE_DEBUG)          /* !!needs some work */
  orig->last_acknum[0] = orig->last_acknum[1] = 0;
  orig->last_seqnum[0] = orig->last_seqnum[1] = 0;
#endif

  clone->next  = _tcp_allsocs;
  _tcp_allsocs = clone;         /* prepend clone to TCB-list */
  *tcp = clone;                 /* clone is now the new TCB */
  return (0);
}

/*
  A handy note from:
    http://help.netscape.com/kb/corporate/960513-73.html

Solaris 2.3, 2.4, and 2.5 have a listen backlog queue for incoming
TCP/IP connections; its maximum length is 5 by default.  If you leave
this set to the default, then you will frequently see connections to
your web server time out under moderate loads, even though there are
enough idle web server listener processes available to handle the
connections and your Solaris system shows no other signs of resource
saturation.

The listen backlog queue holds connections that are "half-open" (in
the process of being opened), as well as connections that have been
fully opened but have not yet been accepted by any local processes.
This has no effect on the total number of open TCP/IP connections that
your Solaris system can deal with at once; it only means that your
system can't juggle more than five loose connections at a time, and
any other connections that come in while Solaris is busy with five
loose connections will be dropped and will time out on the client end.

On Solaris 2.3 and 2.4, you can bring the maximum queue length from 5
up to 32 by using the "ndd" command, which must be run as root:

    /usr/sbin/ndd -set /dev/tcp tcp_conn_req_max 32

    It is theoretically possible to increase this number beyond 32,
    although this is not recommended.  If increasing the maximum length
    from 5 to 32 solved the problem temporarily for you but your web
    server's traffic has now increased to a point where the symptoms
    appear with the maximum queue length set to 32, then you should
    contact Sun for further help with this.

    The Netscape servers on any Unix system will request a listen backlog
    queue length of 128 when they run; the operating system then reduces
    that to something it can handle.

    Solaris 2.5 allows a maximum listen backlog queue length as high as
    1024 (and you can raise it in the same way), but it still defaults to
    a maximum queue length of 5.

*/

#endif /* USE_BSD_FUNC */
