/*
 *  socket.h
 *
 *  BSD sockets functionality for Waterloo TCP/IP
 *
 *  by G. Vanem 1997
 */

#ifndef __SOCKET_H
#define __SOCKET_H

#if defined(__TURBOC__) && (__TURBOC__ <= 0x301)
  /*
   * Prevent tcc <= 2.01 from even looking at this.
   */
  #define BOOL int
#else

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <memory.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <io.h>

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_arp.h>
#include <net/if_ether.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
#include <netinet/in_pcb.h>
#include <netinet/in_var.h>
#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_time.h>

#include <arpa/inet.h>
#include <arpa/nameser.h>

#include "wattcp.h"
#include "wattcpd.h"
#include "chksum.h"
#include "wdpmi.h"
#include "misc.h"
#include "strings.h"
#include "sock_ini.h"
#include "language.h"
#include "pcconfig.h"
#include "pcqueue.h"
#include "pcdbug.h"
#include "pcsed.h"
#include "pcpkt.h"
#include "pcstat.h"
#include "pcmulti.h"
#include "pc_cbrk.h"
#include "pctcp.h"
#include "pcbuf.h"
#include "pcarp.h"
#include "pcrecv.h"
#include "ip_out.h"
#include "gettod.h"

#ifdef USE_LIBPCAP
#include "w32pcap.h"
#endif


typedef struct Socket {
        int                 fd;
        struct Socket      *next;
        struct sockaddr_in *local_addr;
        struct sockaddr_in *remote_addr;

        int                 timeout;
        int                 inp_flags;   /* misc. IP-protocol options */
        u_char              ip_tos;
        u_char              ip_ttl;

        struct ip_opts     *ip_opt;      /* for setsockopt(s,IP_OPTION,..) */
        int                 ip_opt_len;

        int                 so_options;  /* SO_KEEPALIVE, SO_ACCEPTCONN etc. */
        int                 so_proto;
        int                 so_type;     /* generic type, see socket.h */
        int                 so_state;    /* internal state flags SS_*, below */
        int                 so_error;    /* internal socket errno */
        time_t              close_time;  /* we closed at (SOCK_STREAM) */
        unsigned            linger_time; /* user specified linger-time (SOCK_STREAM) */
        unsigned            fd_duped;    /* FSEXT reference counting */
        DWORD               cookie;      /* memory cookie / marker */
        DWORD               keepalive;   /* keepalive timeout */
        udp_Socket         *udp_sock;    /* actual state and Rx/Tx data is in */
        tcp_Socket         *tcp_sock;    /*  one of these pointers */
        raw_Socket         *raw_sock;    /* !!to-do: make linked-list of bufs */
        recv_buf          **bcast_pool;  /* buffers for INADDR_ANY sockets */

        /* listen-queue for incoming tcp connections
         */
        int                 backlog;
        tcp_Socket         *listen_queue [SOMAXCONN];
        DWORD               syn_timestamp[SOMAXCONN]; /* got SYN at [msec] */

        /* low-water marks for send add receive
         */
        unsigned            send_lowat;
        unsigned            recv_lowat;

      } Socket;


/*
 * Various sizes
 */
#if (DOSX)
  #define DEFAULT_RCV_WIN   (16*1024)      /* default receive window, 16kB */
  #define MAX_DGRAMS        5              /* # of datagrams for broadcast */
  #define MAX_RAW_BUFS      2              /* # of raw_Socket in list      */
  #define MAX_SOCKETS       3000           /* # of sockets to handle */
  #define MAX_TCP_RECV_BUF  (1024*1024-1)  /* Max size for SO_RCVBUF */
  #define MAX_TCP_SEND_BUF  (USHRT_MAX-1)
  #define MAX_UDP_RECV_BUF  (USHRT_MAX-1)
  #define MAX_UDP_SEND_BUF  (USHRT_MAX-1)
  #define MAX_RAW_RECV_BUF  (USHRT_MAX-1)
  #define MAX_RAW_SEND_BUF  (USHRT_MAX-1)
#else
  #define DEFAULT_RCV_WIN   (16*1024)
  #define MAX_DGRAMS        2
  #define MAX_SOCKETS       512
  #define MAX_TCP_RECV_BUF  (unsigned)TCP_MAXWIN  /* 64kB */
  #define MAX_TCP_SEND_BUF  (USHRT_MAX-1)
  #define MAX_UDP_RECV_BUF  (USHRT_MAX-1)
  #define MAX_UDP_SEND_BUF  (USHRT_MAX-1)
  #define MAX_RAW_RECV_BUF  (USHRT_MAX-1)
  #define MAX_RAW_SEND_BUF  (USHRT_MAX-1)
#endif

#define DEFAULT_SEND_LOWAT  0
#define DEFAULT_RECV_LOWAT  0

/*
 * Let first socket (non-djgpp) start above 0 in order not to
 * confuse sockets with stdin/stdout/stderr handles.
 */
#ifdef __DJGPP__
  #define SK_FIRST  0   /* first socket will always be >3 */
#else
  #define SK_FIRST  3   /* skip handles 0-2 */
#endif


/*
 * Number of 'fd_set' required to hold MAX_SOCKETS.
 */
#define NUM_SOCK_FDSETS  ((MAX_SOCKETS + sizeof(fd_set) - 1) / sizeof(fd_set))

/*
 * Misc. defines
 */
#ifndef SOCK_PACKET        /* promiscuous mode packet capture */
#define SOCK_PACKET        6
#endif

#ifndef SIGIO
#define SIGIO SIGUSR1      /* for asynchronous I/O support (not yet) */
#endif

/*
 * Socket state bits (in so_state).
 */
#define SS_NOFDREF         0x0001          /* no file table ref any more */
#define SS_UNCONNECTED     SS_NOFDREF      /* or just created socket */
#define SS_ISCONNECTED     0x0002          /* socket connected to a peer */
#define SS_ISCONNECTING    0x0004          /* in process of connecting */
#define SS_ISDISCONNECTING 0x0008          /* in process of disconnecting */
#define SS_CANTSENDMORE    0x0010          /* can't send more data */
#define SS_CANTRCVMORE     0x0020          /* can't receive more data */
#define SS_RCVATMARK       0x0040          /* at mark on input (no used) */

#define SS_PRIV            0x0080          /* privileged for broadcast */
#define SS_NBIO            0x0100          /* non-blocking operations */
#define SS_ASYNC           0x0200          /* async I/O notify (not used) */
#define SS_ISCONFIRMING    0x0400          /* accepting connection req */
#define SS_ISLISTENING     SS_ISCONFIRMING /* non standard */

#define SS_LOCAL_ADDR      0x0800          /* has local address/port (not used) */
#define SS_REMOTE_ADDR     0x1000          /* has remote address/port (not used) */
#define SS_CONN_REFUSED    0x2000          /* connection refused (ICMP_UNREACH) */


/*
 * Socket macros
 */
#define SOCK_CALLOC(size)  _sock_calloc (__FILE__, __LINE__, size)
#define SOCK_DEL_FD(fd)    _sock_del_fd (__FILE__, __LINE__, fd)

/*
 * Prologue code starting most BSD-socket functions
 */
#define SOCK_PROLOGUE(socket, fmt, fd)             \
        do {                                       \
          SOCK_DEBUGF (((Socket*)socket, fmt, fd));\
          if (!socket) {                           \
            if (_sock_dos_fd(fd)) {                \
              SOCK_DEBUGF ((NULL, ", ENOTSOCK"));  \
              SOCK_ERR (ENOTSOCK);                 \
              return (-1);                         \
            }                                      \
            SOCK_DEBUGF ((NULL, ", EBADF"));       \
            SOCK_ERR (EBADF);                      \
            return (-1);                           \
          }                                        \
        } while (0)

#if (DOSX)
  #define VERIFY_RW(ptr,len)                       \
          do {                                     \
            if (!valid_addr((DWORD)(ptr),len)) {   \
               SOCK_DEBUGF ((NULL, ", EFAULT"));   \
               SOCK_ERR (EFAULT);                  \
               return (-1);                        \
            }                                      \
          } while (0)
#else
  #define VERIFY_RW(ptr,len) ((void)0)
#endif

/*
 * Yielding in connect/select/accept loops reduces CPU-loading.
 */
#if (DOSX & DJGPP)
  #define SOCK_YIELD()         __dpmi_yield()
#elif (DOSX & (DOS4GW|WDOSX))
  #define SOCK_YIELD()         dpmi_dos_yield(), kbhit()
#else
  #define SOCK_YIELD()         kbhit()
#endif

/*
 * Set errno/errno_s, see ../inc/sys/werrno.h for list.
 */
#define SOCK_ERR(err)    errno_s = errno = err

/*
 * Print error and exit at critical places. Should not be used
 * in "production" quality releases.
 */
#if defined(USE_BSD_FATAL)
  #define SOCK_FATAL(arg)  do {                        \
                             (*_printf) arg;           \
                             fflush (stdout);          \
                          /* _watt_fatal_error = 1; */ \
                             /*@-unreachable@*/        \
                             exit (-1);                \
                           } while (0)
#else
  #define SOCK_FATAL(arg)  ((void)0)
#endif

/*
 * Use Fortify to check for leaks in BSD-socket API.
 * Should not be used in "production" quality releases.
 */
#if defined(USE_BSD_FORTIFY)
  #include "fortify.h"
#endif

extern int errno_s;    /* set to same as `errno' */

extern int close_s  (int s);
extern int write_s  (int s, const char *buf, int num);
extern int read_s   (int s, char *buf, int num);
extern int select_s (int n, fd_set *r, fd_set *w, fd_set *x, struct timeval *t);

/*
 * Select sockets for read, write or exceptions
 *   Returns 1 if socket is ready or
 *   Returns 0 if socket isn't selectable
 */
extern int  _sock_read_select (Socket *socket);
extern int  _sock_write_select(Socket *socket);
extern int  _sock_exc_select  (Socket *socket);

/*
 * Setup for trapping signals and critical sections around loops.
 */
extern int  _sock_sig_setup   (void);
extern void _sock_sig_restore (void);
extern int  _sock_sig_epipe   (void);
extern void _sock_start_crit  (void);
extern void _sock_stop_crit   (void);

/*
 * Timing of "kernel" times for some socket calls
 */
extern void _sock_start_timer (void);
extern void _sock_stop_timer  (void);

/*
 * Allocation, `sk_list' stuff etc.
 */
extern void   *_sock_calloc   (const char *file, unsigned line, size_t size);
extern Socket *_sock_del_fd   (const char *file, unsigned line, int sock);
extern Socket *_socklist_find (int s);
extern int     _sock_dos_fd   (int s);
extern int     _sock_half_open(const tcp_Socket *tcp);
extern int     _sock_append   (tcp_Socket **tcp);

/*
 * Debugging of BSD-socket API. Writes to "sk_debug.device"
 * when dbug_init() is called or `SO_DEBUG' is set on socket.
 */
extern void _sock_dbug_on     (void);
extern void _sock_dbug_off    (void);
extern void _sock_dbug_init   (void);
extern void _sock_dbug_exit   (void);
extern void _sock_enter_scope (void);
extern void _sock_leave_scope (void);
extern void bsd_fortify_print (const char *buf);

extern void _sock_debugf (const Socket *sock, const char *fmt, ...)
#ifdef __GNUC__
  __attribute__((format(printf,2,3)))
#endif
;

#ifdef USE_DEBUG
  #define SOCK_DEBUGF(x)   _sock_debugf x
  extern void _sock_dbgflush (void);
#else
  #define SOCK_DEBUGF(x)   ((void)0)
  #define _sock_dbgflush() ((void)0)
#endif

/*
 * Handles UDP/TCP connect/listen calls
 */
extern int _TCP_open   (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port);
extern int _UDP_open   (Socket *socket, struct in_addr host, WORD loc_port, WORD rem_port);
extern int _TCP_listen (Socket *socket, struct in_addr host, WORD loc_port);
extern int _UDP_listen (Socket *socket, struct in_addr host, WORD port);

#endif  /* old __TURBOC__ */
#endif  /* __SOCKET_H     */
