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
* Description:  Free segment
*               (16-bit code only)
*
****************************************************************************/


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#if defined(__QNX__)
    #include <sys/seginfo.h>
#elif defined(__OS2__)
    #include <dos.h>
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined(__WINDOWS__)
    #include <dos.h>
    #include <windows.h>
#else
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "rtdata.h"
#include "crwd.h"
#include "heap.h"
#include "seterrno.h"


#if defined(__OS2__)
  #if defined(__BIG_DATA__)
    #define MODIFIES ds es
  #else
    #define MODIFIES es
  #endif
#elif defined(__WINDOWS__)
    #define MODIFIES es
#endif

#if defined(__WINDOWS__) || defined(__OS2__)

#pragma aux DoFreeSeg modify [MODIFIES]
static int DoFreeSeg( __segment seg )
{
  #if defined(__WINDOWS__)
    HANDLE hmem;

    hmem = (HANDLE)GlobalHandle( seg );
    if( hmem == NULL ) {
        return( -1 );
    }
    GlobalUnlock( hmem );
    if( GlobalFree( hmem ) == hmem ) {
        return( -1 );
    }
    return( 0 );
  #else
    return( DosFreeSeg( seg ) );
  #endif
}

extern int tricky_free_seg( int, int );
#if defined(__OS2__) && defined(__BIG_DATA__)
#pragma aux tricky_free_seg = \
        "mov    cx,es"          \
        "cmp    cx,ax"          \
        "jl     L1"             \
        "cmp    cx,bx"          \
        "jg     L1"             \
        "xor    cx,cx"          \
        "L1:"                   \
        "mov    dx,ds"          \
        "cmp    dx,ax"          \
        "jl     L2"             \
        "cmp    dx,bx"          \
        "jg     L2"             \
        "xor    dx,dx"          \
        "L2:"                   \
        "call   DoFreeSeg"      \
        "mov    es,cx"          \
        "mov    ds,dx"          \
        parm [ax] [bx] value [ax] modify [cx dx]
#else
#pragma aux tricky_free_seg = \
        "mov    cx,es"          \
        "cmp    cx,ax"          \
        "jl     L1"             \
        "cmp    cx,bx"          \
        "jg     L1"             \
        "xor    cx,cx"          \
        "L1:"                   \
        "call   DoFreeSeg"      \
        "mov    es,cx"          \
        parm [ax] [bx] value [ax] modify [cx]
#endif

#pragma aux __DoFreeSeg modify [MODIFIES]
static int __DoFreeSeg( __segment first )
{
    __segment     last;
    long          segments;
    unsigned long size;

#ifdef __OS2__
    DosSizeSeg( first, &size );
#else
    size = GlobalSize( (HGLOBAL)first );
#endif
    segments = (size + 65535L) >> 16;
    last = first + (__segment)((segments - 1L) << _RWD_HShift);
    return( tricky_free_seg( first, last ) );
}
#endif


#if defined(__WINDOWS__) || defined(__OS2__)
#pragma aux __FreeSeg modify [MODIFIES]
#endif
int __FreeSeg( __segment seg )
{
#if defined(__QNX__)
    if( qnx_segment_free( seg ) == -1 ) {
        return( -1 );
    }
#elif defined(__WINDOWS__)
    if( __DoFreeSeg( seg ) ) {
        return( -1 );
    }
#elif defined(__OS2__)
    APIRET  rc;

    rc = __DoFreeSeg( seg );
    if( rc ) {
        return( __set_errno_dos( rc ) );
    }
#else
    tiny_ret_t rc;

    rc = TinyFreeBlock( seg );
    if( TINY_ERROR( rc ) ) {
        return( __set_errno_dos( TINY_INFO( rc ) ) );
    }
#endif
    return( 0 );
}
