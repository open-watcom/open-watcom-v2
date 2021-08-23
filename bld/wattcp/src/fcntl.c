/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : 19 Aug 1999 : by Claus Oberste-Brandenburg
 *                         <Claus.OBERSTE-BRANDENBURG@jacala.ensmp.fr>
 *
 *   0.6 : 19 Aug 1999 : Changed function prototype, added fd_duped (GV)
 */

#include "socket.h"

/*
 * Only djgpp have (some of) these F_?? commands.
 */
#if defined(USE_BSD_FUNC) && defined(__DJGPP__) 

int fcntlsocket (int s, int cmd, ...)
{
  Socket *socket = _socklist_find (s);
  long    arg;
  int     rc = 0;
  va_list va;

  va_start (va, cmd);
  arg = va_arg (va, long);

  SOCK_PROLOGUE (socket, "\nfcntlsocket:%d, ", s);

  /* fcntl is always file-ioctrl
   */
  switch (cmd)
  {
#ifdef F_DUPFD
    case F_DUPFD:
         socket->fd_duped++;
         SOCK_DEBUGF ((socket, "F_DUPFD (%d)", socket->fd_duped));
         return (0);
#endif

#ifdef F_GETFL
    case F_GETFL:
         /* to-do!!: handle O_TEXT and O_BINARY
          */
         if (socket->so_state & SS_NBIO)
         {
           SOCK_DEBUGF ((socket, "F_GETFL (non-blocking)"));
           return (O_NONBLOCK);
         }
         SOCK_DEBUGF ((socket, "F_GETFL (blocking)"));
         return (0);
#endif

#ifdef F_SETFL
    case F_SETFL:
         if (arg & O_NONBLOCK)  /* O_APPEND will not be served */
         {
           SOCK_DEBUGF ((socket, "F_SETFL (non-blocking)"));
           socket->so_state |=  SS_NBIO;
         }
         else
         {
           SOCK_DEBUGF ((socket, "F_SETFL (blocking)"));
           socket->so_state &= ~SS_NBIO;
         }
         return (0);
#endif

#ifdef F_GETFD
    case F_GETFD:
         SOCK_DEBUGF ((socket, "F_GETFD not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_SETFD
    case F_SETFD:
         SOCK_DEBUGF ((socket, "F_SETFD not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_GETLK
    case F_GETLK:
         SOCK_DEBUGF ((socket, "F_GETLK not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_SETLK
    case F_SETLK:
         SOCK_DEBUGF ((socket, "F_SETLK not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_SETLKW
    case F_SETLKW:
         SOCK_DEBUGF ((socket, "F_SETLKW not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_GETOWN   /* only defined in UNIX ? */
    case F_GETOWN:
         SOCK_DEBUGF ((socket, "F_GETOWN not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

#ifdef F_SETOWN   /* only defined in UNIX ? */
    case F_SETOWN:
         SOCK_DEBUGF ((socket, "F_SETOWN not supported"));
         SOCK_ERR (ESOCKTNOSUPPORT);
         break;
#endif

    default:
         _sock_enter_scope();
         if (ioctlsocket(s, cmd, (char*)arg) < 0)
         {
           SOCK_DEBUGF ((socket, ", unknown cmd %d", cmd));
           SOCK_ERR (ESOCKTNOSUPPORT);
           rc = -1;
         }
         _sock_leave_scope();
  }
  return (rc);
}
#endif  /* USE_BSD_FUNC && __DJGPP__ */
