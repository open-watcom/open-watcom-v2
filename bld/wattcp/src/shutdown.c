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

#define SHUT_RD   0    /* Further receives are disallowed */
#define SHUT_WR   1    /* Further sends are disallowed */
#define SHUT_RDWR 2    /* Further sends and receives are disallowed */

int shutdown (int s, int how)
{
  Socket *socket = _socklist_find (s);

#if defined(USE_DEBUG)
  static char fmt[] = "\nshutdown:%d/??";

  if (how == SHUT_RD)
     strcpy (fmt+sizeof(fmt)-3, "r ");
  else if (how == SHUT_WR)
     strcpy (fmt+sizeof(fmt)-3, "w ");
  else if (how == SHUT_RDWR)
     strcpy (fmt+sizeof(fmt)-3, "rw");
#endif

  SOCK_PROLOGUE (socket, fmt, s);

#if 0
  /* if not connected, let close_s() do it
   */
  if (!(socket->so_state & SS_ISCONNECTED))
     return close_s (s);
#endif

  switch (how)
  {
    case SHUT_RD:
         socket->so_state   |=  SS_CANTRCVMORE;
         socket->so_options &= ~SO_ACCEPTCONN;
         return (0);

    case SHUT_WR:
         socket->so_state   |=  SS_CANTSENDMORE;
         socket->so_state   &= ~SS_ISLISTENING;
         socket->so_options &= ~SO_ACCEPTCONN;
         return (0);
      // return close_s (s);

    case SHUT_RDWR:
         socket->so_state |=  SS_CANTRCVMORE;
         socket->so_state |=  SS_CANTSENDMORE;
         socket->so_state &= ~SS_ISLISTENING;
         return close_s (s);
  }
  SOCK_ERR (EINVAL);
  return (-1);
}

#endif  /* USE_BSD_FUNC */
