/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Jan 21, 1998 : G. Vanem - created
 */

#include "socket.h"

#if defined(__DJGPP__) && defined(USE_BSD_FUNC) && defined(USE_FSEXT)

#include <sys/fsext.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(USE_DEBUG)
/*
 * Handle printing of function names
 */
struct func_list {
       int         func_val;
       const char *func_name;
     };

static __inline const char *lookup (int option, struct func_list *opt, int num)
{
  while (num)
  {
    if (opt->func_val == option)
       return (opt->func_name);
    num--;
    opt++;
  }
  return ("??");
}

static struct func_list fs_func[] = {
                      { __FSEXT_nop,   "nop"    },
                      { __FSEXT_open,  "open"   },
                      { __FSEXT_creat, "creat"  },
                      { __FSEXT_read,  "read"   },
                      { __FSEXT_write, "write"  },
                      { __FSEXT_ready, "ready"  },
                      { __FSEXT_close, "close"  },
                      { __FSEXT_fcntl, "fcntl"  },
                      { __FSEXT_ioctl, "ioctl"  },
                      { __FSEXT_lseek, "lseek"  },
                      { __FSEXT_link,  "link"   },
                      { __FSEXT_unlink,"unlink" },
#if (DJGPP_MINOR >= 2)
                      { __FSEXT_dup,   "dup"    },
                      { __FSEXT_dup2,  "dup2"   },
                      { __FSEXT_fstat, "fstat"  },
                      { __FSEXT_stat,  "stat"   },
#endif
                    };
#endif /* USE_DEBUG */

/*
 * Filesystem extensions for BSD-sockets and djgpp 2.x
 */
int _fsext_demux (__FSEXT_Fnumber func, int *rv, va_list _args)
{
  int     fd   = va_arg (_args, int);
  int     cmd  = va_arg (_args, int);
  int     arg  = va_arg (_args, int);
  Socket *sock = __FSEXT_get_data (fd); /* same as _socklist_find(fd) */

  SOCK_DEBUGF ((NULL,
     "\n_fsext_demux: fd %d%c, func %d = \"%s\", cmd %08lX, arg %08lX",
     fd, sock ? 's' : 'f', func, lookup (func, fs_func, DIM(fs_func)),
     (DWORD)cmd, (DWORD)arg));


  /* fd is not a valid socket, pass on to lower layer
   */
  if (!sock)
     return (0);

  switch (func)
  {
    case __FSEXT_read:                        /* read (fd,&buf,len) */
         *rv = read_s (fd, (char*)cmd, arg);
         return (1);

    case __FSEXT_write:                       /* write (fd,&buf,len) */
         *rv = write_s (fd, (char*)cmd, arg);
         return (1);

    case __FSEXT_close:                       /* close (fd) */
         if (sock->fd_duped == 0)
            *rv = close_s (fd);
         else
         {
           sock->fd_duped--;
           *rv = 0;
         }
         return (1);

    case __FSEXT_ioctl:   /* ioctl (fd,cmd,...) */
         *rv = ioctlsocket (fd, cmd, (char*)arg); 
         return (1);

    case __FSEXT_fcntl:   /* 'fcntl (fd,cmd)' or 'fcntl (fd,cmd,arg)' */
         *rv = fcntlsocket (fd, cmd, (long)arg);
         return (1);

    case __FSEXT_ready:   /* ready (fd) */
#if 0    /* avoid the overhead of select_s() */
         tcp_tick (NULL);
         *rv = 0;
         if (_sock_read_select(sock) > 0)
            *rv |= __FSEXT_ready_read;

         if (_sock_write_select(sock) > 0)
            *rv |= __FSEXT_ready_write;

         if (_sock_exc_select(sock) > 0)
            *rv |= __FSEXT_ready_error;

#else    /* include trace from select_s() */
         {
           struct  timeval timeout;
           fd_set  fd_read, fd_write, fd_err;

           FD_ZERO (&fd_read);
           FD_ZERO (&fd_write);
           FD_ZERO (&fd_err);
           FD_SET (fd, &fd_read);
           FD_SET (fd, &fd_write);
           FD_SET (fd, &fd_err);
           timeout.tv_sec  = 0;
           timeout.tv_usec = 0L;
           *rv = 0;

           if (select_s (fd+1, &fd_read, &fd_write, &fd_err, &timeout) >= 0)
           {
             if (FD_ISSET(fd,&fd_read))  *rv |= __FSEXT_ready_read;
             if (FD_ISSET(fd,&fd_write)) *rv |= __FSEXT_ready_write;
             if (FD_ISSET(fd,&fd_err))   *rv |= __FSEXT_ready_error;
           }
           else
             *rv = -1;
         }
#endif
         return (1);

    case __FSEXT_creat:
         return (0);

#if (DJGPP_MINOR >= 2)    /* functions added in v2.02 */
    case __FSEXT_dup:     /* dup (fd) */
         *rv = fd;
         sock->fd_duped++;
         return (1);

    case __FSEXT_dup2:    /* dup2 (oldfd,newfd) */
         {
           int type  = sock->so_type;  /* don't use 'sock' after close() */
           int proto = sock->so_proto;

           close (cmd);   /* close (newd) */
           *rv = socket (AF_INET, type, proto);
         }
         return (1);

    case __FSEXT_fstat:   /* Hmm. If user really want this, lets return it */
    case __FSEXT_stat:
#if defined(USE_STATISTICS)
         *rv = 0;
         switch (sock->so_proto)
         {
           int size;

           case IPPROTO_TCP:
                size = min (sizeof(tcpstats), sizeof(struct stat));
                memcpy (rv, &tcpstats, size);
                break;
           case IPPROTO_UDP:
                size = min (sizeof(udpstats), sizeof(struct stat));
                memcpy (rv, &udpstats, size);
                break;
           case IPPROTO_ICMP:
                size = min (sizeof(icmpstats), sizeof(struct stat));
                memcpy (rv, &icmpstats, size);
                break;
           case IPPROTO_IGMP:
                size = min (sizeof(igmpstats), sizeof(struct stat));
                memcpy (rv, &igmpstats, size);
                break;
           case IPPROTO_IP:
                size = min (sizeof(ipstats), sizeof(struct stat));
                memcpy (rv, &ipstats, size);
                break;
           default:
                *rv = -1;
         }
         return (1);
#else
         return (0);
#endif  /* USE_STATISTICS */

#endif  /* DJGPP_MINOR >= 2 */

    default:
         break;
  }
  return (0);    /* unhandled, pass on to lower layer */
}

#endif /* __DJGPP__ && USE_BSD_FUNC && USE_FSEXT */

