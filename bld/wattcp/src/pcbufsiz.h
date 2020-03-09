#ifndef __PCBUFSIZ_H
#define __PCBUFSIZ_H

#if (DOSX)
  #define MAX_TCP_RECV_BUF  (1024*1024-1)  /* Max size for SO_RCVBUF */
#else
  #define MAX_TCP_RECV_BUF  (unsigned)TCP_MAXWIN  /* 65535 */
#endif
#define DEFAULT_RECV_WIN    (16*1024)      /* default receive window, 16kB */
#define MAX_TCP_SEND_BUF    (USHRT_MAX-1)
#define MAX_UDP_RECV_BUF    (USHRT_MAX-1)
#define MAX_UDP_SEND_BUF    (USHRT_MAX-1)
#define MAX_RAW_RECV_BUF    (USHRT_MAX-1)
#define MAX_RAW_SEND_BUF    (USHRT_MAX-1)

typedef unsigned    sock_size;

extern int  sock_recv_buf (sock_type *sk, sock_size size);
extern int  sock_setbuf (sock_type *sk, BYTE *buf, sock_size size);
#ifdef NOT_YET
extern int  sock_send_buf (sock_type *sk, sock_size size);
#endif

#endif
