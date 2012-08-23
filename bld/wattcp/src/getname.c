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

int getsockname (int s, struct sockaddr *name, int *namelen)
{
  Socket *socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\ngetsockname:%d", s);

  if (!name || !namelen || (*namelen < sizeof(struct sockaddr)))
  {
    SOCK_DEBUGF ((socket, ", EINVAL"));
    SOCK_ERR (EINVAL);
    *namelen = 0;
    return (-1);
  }

  if (!socket->local_addr)
  {
    SOCK_DEBUGF ((socket, ", EINVAL"));
    SOCK_ERR (EINVAL);    /* according HP/UX manpage */
    return (-1);
  }

  VERIFY_RW (name, *namelen);

  *namelen = sizeof(struct sockaddr);
  memcpy (name, socket->local_addr, *namelen);

  SOCK_DEBUGF ((socket, ", %s (%d)",
                inet_ntoa(socket->local_addr->sin_addr),
                ntohs(socket->local_addr->sin_port)));
  return (0);
}

int getpeername (int s, struct sockaddr *name, int *namelen)
{
  Socket *socket = _socklist_find (s);

  SOCK_PROLOGUE (socket, "\ngetpeername:%d", s);

  if (name && namelen)
  {
    if (*namelen < sizeof(struct sockaddr))
    {
      SOCK_DEBUGF ((socket, ", EINVAL"));
      SOCK_ERR (EINVAL);
      *namelen = 0;
      return (-1);
    }
    if (!(socket->so_state & SS_ISCONNECTED))
    {
      SOCK_DEBUGF ((socket, ", ENOTCONN"));
      SOCK_ERR (ENOTCONN);
      return (-1);
    }
    VERIFY_RW (name, *namelen);

    *namelen = sizeof (struct sockaddr);
    memcpy (name, socket->remote_addr, *namelen);

    SOCK_DEBUGF ((socket, ", %s (%d)",
                  inet_ntoa(socket->remote_addr->sin_addr),
                  ntohs(socket->remote_addr->sin_port)));
  }
  return (0);
}
#endif
