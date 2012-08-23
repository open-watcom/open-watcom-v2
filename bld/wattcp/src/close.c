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

static int close_stream (int s);
static int close_dgram  (int s);
static int close_raw    (int s);

int close_s (int s)
{
  Socket *socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\nclose:%d", s);

  socket->so_error = 0;

  switch (socket->so_type)
  {
    case SOCK_RAW:
         return close_raw (s);

    case SOCK_DGRAM:
         return close_dgram (s);

    case SOCK_STREAM:
         return close_stream (s);
  }

  SOCK_ERR (EPROTOTYPE);   /* correct errno? */
  return (-1);
}

int closesocket (int s)
{
  return close_s (s);
}

/*
 * Close a SOCK_STREAM socket
 */
static int close_stream (int s)
{
  Socket    *socket = _socklist_find (s);
  sock_type *sk     = (sock_type*)socket->tcp_sock;
  int  i, listen_abort = 0;

  if ((socket->so_state & SS_ISDISCONNECTING) && socket->close_time)
  {
    SOCK_DEBUGF ((socket, ", close already called"));
    SOCK_ERR (EBADF);
    return (-1);
  }

  /* Save memory and abort listen() socket and queue now.
   */
  if (socket->so_options & SO_ACCEPTCONN)
  { 
    SOCK_DEBUGF ((socket, ", listen abort, backlog %d", socket->backlog));
    listen_abort = 1;
    for (i = 0; i < socket->backlog && i < SOMAXCONN; i++)
    {
      sock_type *tcb = (sock_type*) socket->listen_queue[i];
      if (tcb)
      {
        tcb->tcp.rdatalen = 0;   /* flush Rx data */
        sock_abort (tcb);
        free (tcb);
        socket->listen_queue[i] = NULL;
      }
    }
  }
  else if (sk)
  {
    sk->tcp.rdatalen = 0;
    sock_flush (sk);
    sock_close (sk);
  }

  if (listen_abort || !socket->local_addr)
  {
    /* 's' is a listening socket or we never received any thing, kill it now.
     */
    SOCK_DEBUGF ((socket, ", fast kill!"));
    SOCK_DEL_FD (s);
  }
  else
  {
    /* sock_daemon() will free socket from list and inuse array.
     * Local port is marked for reuse after TCP_LINGERTIME (2min).
     */
    socket->so_state  |= (SS_ISDISCONNECTING | SS_CANTSENDMORE);
    socket->so_state  |= SS_CANTRCVMORE;  /* !! should we do this? */
    socket->close_time = time (NULL);
  }
  return (0);
}


/*
 * Close a SOCK_DGRAM socket
 */
static int close_dgram (int s)
{
  Socket    *socket = _socklist_find (s);      /* 'socket' is non-NULL */
  sock_type *sk     = (sock_type*)socket->udp_sock;

  sk->udp.rdatalen = 0;   /* flush Rx data */
  sock_close (sk);
  socket->so_state |= (SS_ISDISCONNECTING | SS_CANTSENDMORE);

  if (!socket->local_addr)
     SOCK_DEL_FD (s);
  return (0);
}

/*
 * Close a SOCK_RAW socket
 */
static int close_raw (int s)
{
  SOCK_DEL_FD (s);
  return (0);
}

#endif /* (USE_BSD_FUNC) */
