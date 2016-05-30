/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description: This file provides the definitions required to build tcplink.c
* on Netware. Because we do not have a native set of Netware headers within
* our source tree and do not want to rely on external developer kits we have
* provided just the bare Novell LIBC prototypes required to build the TCP
* based debuggers.
*
****************************************************************************/


#if !defined (_NOVHAX_H)
#define _NOVHAX_H

#if defined __cplusplus
extern "C" {
#endif

#if !defined ( __NETWARE_LIBC__ )
#error  This file is Netware LIBC only
#endif

#define SOL_SOCKET      0xffff       /* set self socket level */
#define SO_LINGER       0x0080      /* linger on close if data available */
#define TCP_NODELAY     1           /* turn off Nagle coalescing */
#define AF_INET         2           /* Address family internet : UDP, TCP, IP */
#define SOCK_STREAM     1           /* stream socket remarakably */
#define INADDR_ANY      (unsigned long)0

#include "pushpck4.h"

struct linger {
    int     l_onoff;        /* option on/off */
    int     l_linger;       /* linger time */
};

struct protoent {
    char *  p_name;         /* protocol name */
    char ** p_aliases;      /* alias list */
    short   p_proto;        /* number */
};

struct servent {
   char   * s_name;               /* official service name                  */
   char  ** s_aliases;            /* alias list                             */
   short    s_port;               /* port number                            */
   char   * s_proto;              /* protocol to use                        */
};

struct protoent *getprotobyname  ( const char * );
struct servent  *getservbyname   ( const char *, const char * );
struct hostent  *gethostbyname   ( const char * );

struct sockaddr {
   unsigned short sa_family;     /* address family */
   char           sa_data[14];   /* up to 14 bytes of direct address */
};

struct in_addr {
    union {
        struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
        struct { unsigned short s_w1,s_w2; } S_un_w;
        unsigned long S_addr;
    } S_un;
#define s_addr  S_un.S_addr             /* should be used for all code */
#define s_host  S_un.S_un_b.s_b2        /* OBSOLETE: host on imp */
#define s_net   S_un.S_un_b.s_b1        /* OBSOLETE: network */
#define s_imp   S_un.S_un_w.s_w2        /* OBSOLETE: imp */
#define s_impno S_un.S_un_b.s_b4        /* OBSOLETE: imp # */
#define s_lh    S_un.S_un_b.s_b3        /* OBSOLETE: logical host */
};

struct sockaddr_in {
    short           sin_family;
    unsigned short  sin_port;
    struct in_addr  sin_addr;
    char            sin_zero[8];
};

#ifndef FD_SETSIZE
#define FD_SETSIZE  64
#endif

typedef struct fd_set {
    int     fd_count;
    int     fd_array[FD_SETSIZE];
} fd_set;

#define FD_ISSET(fd, set)  ___fd_isset(fd, (fd_set *) (set))
#define FD_ZERO(set)       ((fd_set *) (set))->fd_count = 0

#define FD_SET(fd, set)                \
{                                      \
   register int   *slot, *end;         \
                                       \
   slot = (set)->fd_array;             \
   end  = slot + (set)->fd_count;      \
                                       \
   while (slot < end) {                \
      if (*slot == fd)                 \
         break;                        \
                                       \
      slot++;                          \
   }                                   \
                                       \
   if (slot == end                     \
      && (set)->fd_count < FD_SETSIZE) \
   {                                   \
      *slot = fd;                      \
      (set)->fd_count++;               \
   }                                   \
}

#define FD_CLR(fd, set)                \
{                                      \
   register int   *slot, *end;         \
                                       \
   slot = (set)->fd_array;             \
   end  = slot + (set)->fd_count;      \
                                       \
   while (slot < end) {                \
      if (*slot == fd) {               \
         *slot = *(--end);             \
         (set)->fd_count--;            \
         break;                        \
      }                                \
      slot++;                          \
   }                                   \
}

struct timeval {
   long     tv_sec;
   long     tv_usec;
};

#include "poppck.h"

#define ssize_t int

extern int              select( size_t nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval *timeout );

extern int              setsockopt( int s, int level, int optname, const void *optval, size_t optlen );
extern int              accept( int s, struct sockaddr *addr, int *len );   //size_t *len);

extern ssize_t          send( int s, const void *msg, size_t len, int flags );
extern int              setsockopt( int s, int level, int optname, const void *optval, size_t optlen );
extern ssize_t          recv( int s, void *buf, size_t len, int flags );
extern int              bind( int s, const struct sockaddr *addr, size_t );
extern int              getsockname( int s, struct sockaddr *addr, int *len );  //size_t *len);
extern int              listen( int s, int backlog );
extern int              socket( int domain, int type, int protocol );

extern char             *inet_ntoa( struct in_addr addr );

extern unsigned long    htonl( unsigned long  );
extern unsigned short   htons( unsigned short );
extern unsigned long    ntohl( unsigned long  );
extern unsigned short   ntohs( unsigned short );

#if defined __cplusplus
}
#endif

#endif /* _NOVHAH_H */
