/*-
 * Copyright (c) 1982, 1986, 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      @(#)types.h     7.17 (Berkeley) 5/6/91
 *      @(#)wtypes.h    Waterloo TCP/IP
 */

/*
 * the naming <sys/wtypes.h> is required for those compilers that
 * have <sys/types.h> in the usual place but doesn't define
 * the following types. This file is included from <sys/socket.h>,
 * <tcp.h> etc.
 */

#ifndef __SYS_WTYPES_H
#define __SYS_WTYPES_H

#if defined(__DJGPP__) || defined(__WATCOMC__)
#include <sys/types.h>
#endif

#if defined(__SMALL__) || defined(__LARGE__)
  typedef unsigned long u_int;    /* too many headers assume u_int is 32-bit */
#else
  typedef unsigned int  u_int;
#endif

typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned short  ushort;   /* Sys V compatibility */
typedef unsigned long   u_long;
typedef unsigned long   caddr_t;

typedef unsigned char   u_int8_t;
typedef unsigned short  u_int16_t;
typedef unsigned long   u_int32_t;
#ifndef __WATCOMC__
typedef short           int16_t;
typedef long            int32_t;
#endif

#define __BIT_TYPES_DEFINED__

#ifndef FD_SET
  #undef  FD_SETSIZE
  #define FD_SETSIZE    512
  #define FD_SET(n, p)  ((p)->fd_bits[(n)/8] |=  (1 << ((n) & 7)))
  #define FD_CLR(n, p)  ((p)->fd_bits[(n)/8] &= ~(1 << ((n) & 7)))
  #define FD_ISSET(n,p) ((p)->fd_bits[(n)/8] &   (1 << ((n) & 7)))
  #define FD_ZERO(p)    memset((void*)(p),0,sizeof(*(p)))

  typedef struct fd_set {
          unsigned char fd_bits [(FD_SETSIZE+7)/8];
        } fd_set;
#endif

#endif

