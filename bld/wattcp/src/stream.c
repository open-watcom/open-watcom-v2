/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#include "socket.h"
#include "stream.h"

#if defined(USE_BSD_FUNC)

/*
 *  How does this fit in with djgpp's FSextensions ?
 *  Does it support fdopen() ?
 */

int sock_fgets (char *buf, int max, FILE *stream)
{
  int     s    = fileno (stream);
  Socket *sock = _socklist_find (s);

  SOCK_PROLOGUE (sock, "\nsock_fgets:%d", s);

  /* !!to-do: this should read a single line only (use sock_gets?)
   */
  return read_s (s, buf, max);
}

int sock_fputs (char *text, FILE *stream)
{
  int     s    = fileno (stream);
  Socket *sock = _socklist_find (s);

  SOCK_PROLOGUE (sock, "\nsock_fputs:%d", s);
  return write_s (s, text, strlen(text));
}

#endif  /* USE_BSD_FUNC */
