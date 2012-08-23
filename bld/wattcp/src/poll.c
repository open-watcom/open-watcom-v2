/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   1.0 : Sep 3, 1999 : G. Vanem
 *
 *   This function is not in BSD, but Linux-specific (to aid
 *   porting Linux applications to Watt-32).
 *
 *   poll() taken from HTTP-tunnel:
 *
 *   Copyright (C) 1999 Lars Brinkhoff <lars@nocrew.org>.
 *
 */

#include "socket.h"
#include "poll.h"

#if defined(USE_BSD_FUNC) && !defined(HAVE_SYS_POLL_H)

int poll (struct pollfd *p, int num, int timeout)
{
  struct timeval tv;
  fd_set read, write, except;
  int    i, n, ret;

  FD_ZERO (&read);
  FD_ZERO (&write);
  FD_ZERO (&except);

  n = -1;
  for (i = 0; i < num; i++)
  {
    if (p[i].fd < 0)
       continue;
    if (p[i].events & POLLIN)
       FD_SET (p[i].fd, &read);
    if (p[i].events & POLLOUT)
       FD_SET (p[i].fd, &write);
    if (p[i].events & POLLERR)
       FD_SET (p[i].fd, &except);
    if (p[i].fd > n)
       n = p[i].fd;
  }

  if (n == -1)
     return (0);

  if (timeout < 0)
     ret = select_s (n+1, &read, &write, &except, NULL);
  else
  {
    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = 1000 * (timeout % 1000);
    ret = select_s (n+1, &read, &write, &except, &tv);
  }

  for (i = 0; (ret >= 0) && (i < n); i++)
  {
    p[i].revents = 0;
    if (FD_ISSET (p[i].fd, &read))
       p[i].revents |= POLLIN;
    if (FD_ISSET (p[i].fd, &write))
       p[i].revents |= POLLOUT;
    if (FD_ISSET (p[i].fd, &except))
       p[i].revents |= POLLERR;
  }
  return (ret);
}
#endif /* USE_BSD_FUNC && !HAVE_SYS_POLL_H */
