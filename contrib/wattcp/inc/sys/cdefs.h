/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	@(#)cdefs.h	8.7 (Berkeley) 1/21/94
 */

#ifndef __SYS_CDEFS_H
#define __SYS_CDEFS_H

#ifdef __cplusplus
  #define __BEGIN_DECLS  extern "C" {
  #define __END_DECLS    };
#else
  #define __BEGIN_DECLS
  #define __END_DECLS
#endif

#if defined(_MSC_VER) && (MSC_VER <= 600) /* MSC C <= 6.0 isn't STD-C ? */
  #undef  __STDC__
  #define __STDC__
#endif

#if defined(__CCDL__)  /* Ladsoft C isn't STD-C by default */
  #undef  __STDC__
  #define __STDC__
#endif

struct mbuf {
       struct mbuf  *next;    /* Links mbufs belonging to single packets */
       struct mbuf  *anext;   /* Links packets on queues */
       unsigned      size;    /* Size of associated data buffer */
       int           refcnt;  /* Reference count */
       struct mbuf  *dup;     /* Pointer to duplicated mbuf */
       char         *data;    /* Active working pointers */
       unsigned      cnt;
     };

/*
 * The __CONCAT macro is used to concatenate parts of symbol names, e.g.
 * with "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo.
 * The __CONCAT macro is a bit tricky -- make sure you don't put spaces
 * in between its arguments.  __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 */
#if defined(__STDC__) || defined(__cplusplus) || defined(__TURBOC__)

  #define __P(protos)     protos        /* full-blown ANSI C */
  #define __CONCAT(x,y)   x ## y
  #define __STRING(x)     #x

  #define __const         const         /* define reserved names to standard */
  #define __signed        signed
  #define __volatile      volatile
  #ifdef __cplusplus
    #define __inline      inline        /* convert to C++ keyword */
  #else
    #ifndef __GNUC__
      #define __inline                  /* delete GCC keyword */
    #endif /* !__GNUC__ */
  #endif /* !C++ */

#else
  #define __P(protos)     ()            /* traditional C preprocessor */
  #define __CONCAT(x,y)   x/**/y
  #define __STRING(x)     "x"

  #ifndef __GNUC__
    #define __const                     /* delete pseudo-ANSI C keywords */
    #define __inline
    #define __signed
    #define __volatile
/*
 * In non-ANSI C environments, new programs will want ANSI-only C keywords
 * deleted from the program and old programs will want them left alone.
 * When using a compiler other than gcc, programs using the ANSI C keywords
 * const, inline etc. as normal identifiers should define -DNO_ANSI_KEYWORDS.
 * When using "gcc -traditional", we assume that this is the intent; if
 * __GNUC__ is defined but __STDC__ is not, we leave the new keywords alone.
 */
    #ifndef NO_ANSI_KEYWORDS
      #define const                     /* delete ANSI C keywords */
      #define inline
      #define signed
      #define volatile
    #endif
  #endif  /* !__GNUC__ */
#endif  /* !C++ */

/*
 * GCC1 and some versions of GCC2 declare dead (non-returning) and
 * pure (no side effects) functions using "volatile" and "const";
 * unfortunately, these then cause warnings under "-ansi -pedantic".
 * GCC2 uses a new, peculiar __attribute__((attrs)) style.  All of
 * these work for GNU C++ (modulo a slight glitch in the C++ grammar
 * in the distribution version of 2.5.5).
 */
#if !defined(__GNUC__) || __GNUC__ < 2 || __GNUC_MINOR__ < 5
  #undef  __attribute__
  #define __attribute__(x)        /* delete __attribute__ if non-gcc or gcc1 */
  #if defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define __dead          __volatile
    #define __pure          __const
  #endif
#endif

/* Delete pseudo-keywords wherever they are not available or needed.
 */
#ifndef __dead
#define __dead
#endif

#ifndef __pure
#define __pure
#endif

#undef _BCC_SMALL

/*
 * min() & max() macros
 */
#ifdef __HIGHC__
  #undef  min
  #undef  max
  #define min(a,b) _min(a,b)  /* intrinsic functions */
  #define max(a,b) _max(a,b)
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

/*
 * from NetBSD's <sys/cdefs_aout.h>
 *
 * Written by J.T. Conklin <jtc@wimsey.com> 01/17/95.
 * Public domain.
 */

#define	_C_LABEL(x)	__CONCAT(_,x)

#ifdef __GNUC__
  #ifdef __STDC__
    #define __indr_reference(sym,alias)                \
            __asm__(".stabs \"_" #alias "\",11,0,0,0");\
            __asm__(".stabs \"_" #sym "\",1,0,0,0");

    #define __warn_references(sym,msg)                 \
            __asm__(".stabs \"" msg "\",30,0,0,0");    \
            __asm__(".stabs \"_" #sym "\",1,0,0,0");
  #else
    #define __indr_reference(sym,alias)                \
            __asm__(".stabs \"_/**/alias\",11,0,0,0"); \
            __asm__(".stabs \"_/**/sym\",1,0,0,0");

    #define __warn_references(sym,msg)                 \
            __asm__(".stabs msg,30,0,0,0");            \
            __asm__(".stabs \"_/**/sym\",1,0,0,0");
  #endif
#else
  #define __warn_references(sym,msg)
#endif

#ifdef __GNUC__
  #define __IDSTRING(name,string)  \
          static const char name[] __attribute__((__unused__)) = string
#else
  #define __IDSTRING(name,string)  \
          static const char name[] = string
#endif

#define __RCSID(_s)                 __IDSTRING(rcsid,_s)
#define __COPYRIGHT(_s)             __IDSTRING(copyright,_s)

#define __KERNEL_RCSID(_n, _s)      __IDSTRING(__CONCAT(rcsid,_n),_s)
#define __KERNEL_COPYRIGHT(_n, _s)  __IDSTRING(__CONCAT(copyright,_n),_s)

#endif
