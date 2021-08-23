#ifndef __POLL_H
#define __POLL_H

#ifdef __DJGPP__
#include <sys/config.h>  /* might have HAVE_SYS_POLL_H */
#endif

#ifdef HAVE_SYS_POLL_H   /* have .h-file and implementation */
  #include <sys/poll.h>

#else
  #define POLLIN   0x0001
  #define POLLPRI  0x0002   /* not used */
  #define POLLOUT  0x0004
  #define POLLERR  0x0008

  struct pollfd {
         int fd;
         int events;     /* in param: what to poll for */
         int revents;    /* out param: what events occured */
       };

  extern int poll (struct pollfd *p, int num, int timeout);

#endif /* HAVE_SYS_POLL_H */

#endif /* __POLL_H */
