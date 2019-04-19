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

extern void _snowmove( LP_PIXEL, LP_PIXEL, int );
#pragma aux _snowmove = \
        "shr  cx,1"         \
        "jz short L6"       \
        "push ds"           \
        "mov  ax,es"        \
        "cmp  ax,dx"        \
        "jnz short L1"      \
        "cmp  di,si"        \
        "jb short L1"       \
        "add  di,cx"        \
        "dec  di"           \
        "add  si,cx"        \
        "dec  si"           \
        "std"               \
    "L1: mov  ds,dx"        \
        "mov  dx,03dah"     \
    "L2: in   al,dx"        \
        "ror  al,1"         \
        "jb short L2"       \
        "cli"               \
    "L3: in   al,dx"        \
        "ror  al,1"         \
        "jae short L3"      \
        "lodsw"             \
        "sti"               \
        "mov  bx,ax"        \
    "L4: in   al,dx"        \
        "ror  al,1"         \
        "jb short L4"       \
        "cli"               \
    "L5: in   al,dx"        \
        "ror  al,1"         \
        "jae short L5"      \
        "mov  ax,bx"        \
        "stosw"             \
        "sti"               \
        "loop short L2"     \
        "pop  ds"           \
        "cld"               \
    "L6:"                   \
    __parm __caller [__es __di] [__dx __si] [__cx] \
    __modify        [__ax __dx __bx __cx]

#endif

#define ATTR_FLIP_MASK      0x77

void intern farfill( LP_PIXEL start, PIXEL fill, uisize len, bool snow )
{
    uisize      i;

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


void intern farcopy( LP_PIXEL src, LP_PIXEL dst, uisize len, bool snow )
{
#ifdef _M_I86
    if( snow ) {
        _snowmove( dst, src, len * sizeof( PIXEL ) );
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


void intern farstring( LP_PIXEL start, ATTR attr, LPC_STRING str, uisize str_len, bool snow )
{
    uisize      i;
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
            *start++ = p;
        }
#ifdef _M_I86
    }
#endif
    p.ch = ' ';
    farfill( start, p, str_len - i, snow );
}

void intern farattrib( LP_PIXEL start, ATTR attr, uisize len, bool snow )
{
    uisize      i;
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
            *start++ = p;
        }
#ifdef _M_I86
    }
#endif
}

void intern farattrflip( LP_PIXEL start, uisize len, bool snow )
{
    uisize      i;
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
            *start++ = p;
        }
#ifdef _M_I86
    }
#endif
}
