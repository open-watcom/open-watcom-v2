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

int listen (int s, int backlog)
{
  struct  in_addr addr;
  Socket *socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\nlisten:%d", s);

  if (!socket->local_addr)              /* bind() not called */
  {
    SOCK_DEBUGF ((socket, ", EINVAL"));
    SOCK_ERR (EINVAL);
    return (-1);
  }

  if (socket->so_type != SOCK_STREAM)
  {
    SOCK_DEBUGF ((socket, ", EOPNOTSUPP"));
    SOCK_ERR (EOPNOTSUPP);
    return (-1);
  }

  addr.s_addr = INADDR_ANY;
  _tcp_syn_hook = _sock_append;

  _TCP_listen (socket, addr, socket->local_addr->sin_port);
  SOCK_DEBUGF ((socket, ", port %d", ntohs(socket->local_addr->sin_port)));

  socket->timeout = 0;                  /* never times out */
  socket->so_state   |= SS_ISLISTENING;
  socket->so_options |= SO_ACCEPTCONN;

  /* legal backlog range [1..SOMAXCONN>
   */
  socket->backlog = min (backlog, SOMAXCONN-1);
  socket->backlog = max (socket->backlog, 1);
  SOCK_DEBUGF ((socket, ", backlog %d ", socket->backlog));
  return (0);
}

#endif  /* USE_BSD_FUNC */
