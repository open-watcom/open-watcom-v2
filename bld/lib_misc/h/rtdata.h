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
* Description:  Internal CLIB structures and variables.
*
****************************************************************************/


#ifndef _RTDATA_H_INCLUDED
#define _RTDATA_H_INCLUDED

#include <stdio.h>
#include "errorno.h"

/* DOS based platforms have stdaux/stdprn in addition to stdin/out/err */
#if defined(__DOS__) || defined(__WINDOWS__) || defined(__OSI__)
    #define NUM_STD_STREAMS 5
#else
    #define NUM_STD_STREAMS 3
#endif

#if defined(__NT__) || defined(__OS2__) || defined(__UNIX__)
    struct __pipe_info {
        int                 isPipe;     /* non-zero if it's a pipe */
        int                 pid;        /* PID of spawned process */
    };
#endif

typedef struct __stream_link {
    struct __stream_link *  next;
    struct __iobuf *        stream;
    unsigned char *         _base;          /* location of buffer */
    int                     _orientation;   /* wide/byte/not oriented */
    int                     _extflags;      /* extended flags */
    unsigned char           _tmpfchar;      /* tmpfile number */
    unsigned char           _filler[sizeof(int)-1];/* explicit padding */
#if defined(__NT__) || defined(__OS2__) || defined(__UNIX__)
    struct __pipe_info      pipeInfo;       /* pipe-related fields */
#endif
} __stream_link;

#if defined(_M_IX86)
typedef struct _87state {
#if defined( __386__ )
    char data[108];         /* 32-bit save area size */
#else
    char data[94];          /* 16-bit save area size */
#endif
} _87state;
#endif

typedef void _WCI86FAR      FPEhandler( int );

#define _FP_BASE(__fp)          ((__fp)->_link->_base)
#ifndef __NETWARE__
    #define _FP_ORIENTATION(__fp)   ((__fp)->_link->_orientation)
    #define _FP_EXTFLAGS(__fp)      ((__fp)->_link->_extflags)
#endif
    #define _FP_TMPFCHAR(__fp)      ((__fp)->_link->_tmpfchar)
#ifndef __NETWARE__
    #define _FP_PIPEDATA(__fp)      ((__fp)->_link->pipeInfo)
#endif

extern __stream_link        *__OpenStreams;
extern __stream_link        *__ClosedStreams;
extern      char            * _WCNEAR __env_mask;  /* ptr to char array of flags */
extern      FPEhandler      *__FPE_handler;
extern      void            (*__FPE_handler_exit)( void );
#if !defined(__NETWARE__)
    extern  int             _cbyte;
    extern  int             _cbyte2;
    extern  int             _child;
    extern  int             __umaskval;
    extern  unsigned        _curbrk;
    extern  int             _commode;
#if defined(_M_IX86) && defined(__WATCOMC__)
    #pragma aux             _child "_*";
#endif
#endif
#if !defined(__SW_BM) && !defined( __MT__ )
    extern  unsigned        _STACKLOW;
#endif
#if !defined (_NETWARE_LIBC)
extern      unsigned        _STACKTOP;
#endif
/* alternate stack for F77 compiler */
#if !defined( _M_I86 )
extern  unsigned            __ASTACKSIZ;
extern  char                *__ASTACKPTR;
#if defined( _M_IX86 ) && defined(__WATCOMC__)
 #pragma aux                __ASTACKPTR "*"
 #pragma aux                __ASTACKSIZ "*"
#endif
#endif
#if !defined( _M_I86 )
#if defined( __DOS__ )
extern char     _WCFAR *_Envptr;
#elif defined( __LINUX__ )
extern char     **_Envptr;
#else
extern char     *_Envptr;
#endif
#endif

#if !defined(__QNX__) && !defined(__LINUX__) && defined(_M_IX86)
    extern void         (*__Save8087)(_87state *);/* Ptr to FP state save rtn (spawn) */
    extern void         (*__Rest8087)(_87state *);/* Ptr to FP state restore rtn (spawn) */
#endif
extern unsigned short       _8087cw;    /* control word initializer */
extern unsigned char        _no87;      /* NO87 environment var defined */
extern unsigned char        _8087;      /* type of 8087/emulator present */
extern unsigned char        _real87;    /* 8087 coprocessor hardware present */
#if defined(_M_IX86) && defined(__WATCOMC__)
    #pragma aux             _8087cw "_*";
    #pragma aux             _no87 "_*";
    #pragma aux             _8087 "_*";
    #pragma aux             _real87 "_*";
#endif
extern unsigned char        __uselfn;   /* LFN support available flag */

#define _RWD_ostream            __OpenStreams
#define _RWD_cstream            __ClosedStreams
#define _RWD_iob                __iob
#if !defined(__NETWARE__)
    #define _RWD_threadid       _threadid
#endif
#if !defined( _M_I86 )
    #define _RWD_Envptr         _Envptr
#endif
#define _RWD_environ            environ
#define _RWD_wenviron           _wenviron
#define _RWD_env_mask           __env_mask
#define _RWD_abort              __abort
#define _RWD_sigtab             __SIGNALTABLE
#define _RWD_FPE_handler_exit   __FPE_handler_exit
#define _RWD_FPE_handler        __FPE_handler
#define _RWD_fmode              _fmode
#if !defined(__NETWARE__)
    #define _RWD_umaskval       __umaskval
    #define _RWD_cbyte          _cbyte
    #define _RWD_cbyte2         _cbyte2
    #define _RWD_child          _child
    #define _RWD_amblksiz       _amblksiz
    #define _RWD_curbrk         _curbrk
    #define _RWD_dynend         _dynend
    #define _RWD_psp            _psp
#endif
#define _RWD_stacklow           _STACKLOW
#if !defined (_NETWARE_LIBC)
#define _RWD_stacktop           _STACKTOP
#endif
#if !defined(__QNX__) && !defined(__LINUX__)
    #define _RWD_Save8087       __Save8087
    #define _RWD_Rest8087       __Rest8087
#endif
#define _RWD_8087cw             _8087cw
#define _RWD_no87               _no87
#define _RWD_8087               _8087
#define _RWD_real87             _real87
#if !defined(__NETWARE__)
    #define _RWD_HShift         _HShift
    #define _RWD_osmajor        _osmajor
    #define _RWD_osminor        _osminor
    #define _RWD_osmode         _osmode
    #if defined(__NT__)
        #define _RWD_osbuild    _osbuild
        #define _RWD_osver      _osver
        #define _RWD_winmajor   _winmajor
        #define _RWD_winminor   _winminor
        #define _RWD_winver     _winver
    #endif
    #define _RWD_doserrno       _DOSERRNO
#endif
#define _RWD_tmpfnext           __tmpfnext
#if !defined(_RWD_errno)
    #define _RWD_errno          _ERRNO
#endif
#define _RWD_nexttok            _NEXTTOK
#define _RWD_nextftok           _NEXTFTOK
#define _RWD_nextmbtok          _NEXTMBTOK
#define _RWD_nextmbftok         _NEXTMBFTOK
#define _RWD_nextwtok           _NEXTWTOK
#define _RWD_tzname             tzname
#define _RWD_timezone           timezone
#define _RWD_daylight           daylight
#define _RWD_dst_adjust         __dst_adjust
#define _RWD_start_dst          __start_dst
#define _RWD_end_dst            __end_dst
#define _RWD_asctime            _RESULT
#if defined( __SW_BM ) || defined( __MT__ )
    #define _RWD_cvtbuf         __THREADDATAPTR->__cvt_buffer
#else
    #define _RWD_cvtbuf         cvt_buffer
#endif
#if defined(__NETWARE__)
    #define _RWD_ioexit         __ioexit
    #define _RWD_tmpnambuf      (__THREADDATAPTR->__tmpnambuf)
    #define _RWD_randnextinit   (__THREADDATAPTR->__randnextinit)
#else
    #define _RWD_tmpnambuf      _tmpname
    #define _RWD_randnextinit   THREAD_PTR.__randnextinit
#endif
#define _RWD_randnext           _RANDNEXT
#define _RWD_ThreadData         _ThreadData
#define _RWD_StaticInitSema     _StaticInitSema
#define _RWD_PureErrorFlag      _PureErrorFlag
#define _RWD_UndefVfunFlag      _UndefVfunFlag
#define _RWD_ModuleInit         _ModuleInit

#define _RWD_uselfn             __uselfn

/*
    For the sake of efficiency, tell the compiler
    that the __exit... routines never return.
*/
_WCRTLINK   extern  void    __exit( unsigned );
#if defined(_M_IX86) && defined(__WATCOMC__)
    #pragma aux     __exit aborts;
#endif

extern      void    (*__abort)( void );     // Defined in abort.c
extern      void    __terminate( void );    // Defined in abort.c

#endif // _RTDATA_H_INCLUDED
