/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Routines accessing far memory.
*
****************************************************************************/


#include <string.h>
#ifdef __WATCOMC__
    #if defined( _M_I86 )
        #include <i86.h>
    #endif
    #include <conio.h>
#endif
#include "uidef.h"
#include "uifar.h"

#ifdef _M_I86

extern void _forward( void );
#pragma aux _forward = "cld" __modify []

extern void _backward( void );
#pragma aux _backward = "std" __modify []

extern PIXEL _snowget( LP_PIXEL );
#pragma aux _snowget = \
        "push ds"       \
        "mov  ds,dx"    \
        "mov  dx,3dah"  \
    "L1: in   al,dx"    \
        "ror  al,1"     \
        "jb short L1"   \
        "cli"           \
    "L2: in   al,dx"    \
        "ror  al,1"     \
        "jae short L2"  \
        "lodsw"         \
        "sti"           \
        "pop  ds"       \
    __parm __caller [__dx __si] \
    __value         [__ax] \
    __modify        [__ax __dx]

extern LP_PIXEL _snowput( LP_PIXEL, PIXEL );
#pragma aux _snowput = \
        "mov  dx,3dah" \
    "L1: in   al,dx"    \
        "ror  al,1"     \
        "jb short L1"   \
        "cli"           \
    "L2: in   al,dx"    \
        "ror  al,1"     \
        "jae short L2"  \
        "mov  ax,bx"    \
        "stosw"         \
        "sti"           \
    __parm __caller [__es __di] [__bx] \
    __value         [__es __di] \
    __modify        [__ax __dx]

extern void _snowcopy( LP_PIXEL, LP_PIXEL, int );
#pragma aux _snowcopy = \
        "push ds"           \
        "mov  ds,dx"        \
        "mov  dx,03dah"     \
    "top:"                  \
    "L1: in   al,dx"        \
        "ror  al,1"         \
        "jb short L1"       \
        "cli"               \
    "L2: in   al,dx"        \
        "ror  al,1"         \
        "jae short L2"      \
        "lodsw"             \
        "sti"               \
        "mov  bx,ax"        \
    "L3: in   al,dx"        \
        "ror  al,1"         \
        "jb short L3"       \
        "cli"               \
    "L4: in   al,dx"        \
        "ror  al,1"         \
        "jae short L4"      \
        "mov  ax,bx"        \
        "stosw"             \
        "sti"               \
        "loop short top"    \
        "pop  ds"           \
    __parm __caller [__es __di] [__dx __si] [__cx] \
    __modify        [__ax __dx __bx __cx]

#endif

#define ATTR_FLIP_MASK      0x77

intern void farfill( LP_PIXEL start, PIXEL fill, size_t len, bool snow )
{
    size_t      i;

#ifdef _M_I86
    if( snow ) {
        for( i = 0; i < len; ++i ) {
            start = _snowput( start, fill );
        }
    } else {
#else
    /* unused parameters */ (void)snow;
#endif
        for( i = 0; i < len; ++i ) {
            *start++ = fill;
        }
#ifdef _M_I86
    }
#endif
}


intern void farcopy( LP_PIXEL src, LP_PIXEL dst, size_t len, bool snow )
{
#ifdef _M_I86
    if( snow ) {
        if( FP_SEG(src) == FP_SEG(dst) && FP_OFF(src) < FP_OFF(dst) ) {
            src += len - 1;
            dst += len - 1;
            _backward();
        }
        _snowcopy( dst, src, len );
        _forward();
    } else {
#else
    /* unused parameters */ (void)snow;
#endif
#if defined( __NETWARE__ )
        // Netware compiled with "far" defined, but pointers aren't really
        // far, and there is no _fmemmove function, and we were getting
        // "pointer truncated" warnings before, so just cast. (SteveM)
        // ?? why it is compiled with "far" ??
        // LP_PIXEL should be declared as near
        // then these cast are useless
        memmove( (PIXEL *)dst, (PIXEL *)src, len * sizeof( PIXEL ) );
#elif !defined( _M_IX86 ) || defined( __UNIX__ )
        memmove( dst, src, len * sizeof( PIXEL ) );
#else
        _fmemmove( dst, src, len * sizeof( PIXEL ) );
#endif
#ifdef _M_I86
    }
#endif
}


intern void farstring( LP_PIXEL start, ATTR attr, LPC_STRING str, size_t str_len, bool snow )
{
    size_t      i;
    PIXEL       p;

    p.attr = attr;
#ifdef _M_I86
    if( snow ) {
        for( i = 0; i < str_len; ++i ) {
            p.ch = *str++;
            if( p.ch == '\0' )
                break;
            start = _snowput( start, p );
        }
    } else {
#else
    /* unused parameters */ (void)snow;
#endif
        for( i = 0; i < str_len; ++i ) {
            p.ch = *str++;
            if( p.ch == '\0' )
                break;
            *start = p;
            ++start;
        }
#ifdef _M_I86
    }
#endif
    p.ch = ' ';
    farfill( start, p, str_len - i, snow );
}

intern void farattrib( LP_PIXEL start, ATTR attr, size_t len, bool snow )
{
    size_t      i;
    PIXEL       p;

#ifdef _M_I86
    if( snow ) {
        for( i = 0; i < len; ++i ) {
            p = _snowget( start );
            p.attr = attr;
            start = _snowput( start, p );
        }
    } else {
#else
    /* unused parameters */ (void)snow;
#endif
        for( i = 0; i < len; ++i ) {
            p = *start;
            p.attr = attr;
            *start = p;
            ++start;
        }
#ifdef _M_I86
    }
#endif
}

intern void farattrflip( LP_PIXEL start, size_t len, bool snow )
{
    size_t      i;
    PIXEL       p;

#ifdef _M_I86
    if( snow ) {
        for( i = 0; i < len; ++i ) {
            p = _snowget( start );
            p.attr = p.attr ^ ATTR_FLIP_MASK;
            start = _snowput( start, p );
        }
    } else {
#else
    /* unused parameters */ (void)snow;
#endif
        for( i = 0; i < len; ++i ) {
            p = *start;
            p.attr = p.attr ^ ATTR_FLIP_MASK;
            *start = p;
            ++start;
        }
#ifdef _M_I86
    }
#endif
}
