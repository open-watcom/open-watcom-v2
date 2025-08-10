/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of memmove().
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <string.h>


#if defined( _M_I86 )

#if defined(__SMALL_DATA__)

#define HAVE_INLINE_MOVE

extern void     movebwd( char _WCFAR *d, const char _WCNEAR *s, size_t len);
#pragma aux movebwd = \
        "std"           \
        "dec  si"       \
        "dec  di"       \
        "shr  cx,1"     \
        "rep  movsw"    \
        "adc  cx,cx"    \
        "inc  si"       \
        "inc  di"       \
        "rep  movsb"    \
        "cld"           \
    __parm              [__es __di] [__si] [__cx] \
    __value             \
    __modify __exact    [__di __si __cx]

extern void     movefwd( char _WCFAR *d, const char _WCNEAR *s, size_t len);
#pragma aux movefwd = \
        "shr  cx,1"     \
        "rep  movsw"    \
        "adc  cx,cx"    \
        "rep  movsb"    \
    __parm              [__es __di] [__si] [__cx] \
    __value             \
    __modify __exact    [__di __si __cx]

#else

#define HAVE_INLINE_MOVE

extern void     movebwd( char _WCFAR *d, const char _WCFAR *s, size_t len);
#pragma aux movebwd = \
        "std"           \
        "push ds"       \
        "mov  ds,dx"    \
        "dec  si"       \
        "dec  di"       \
        "shr  cx,1"     \
        "rep  movsw"    \
        "adc  cx,cx"    \
        "inc  si"       \
        "inc  di"       \
        "rep  movsb"    \
        "pop  ds"       \
        "cld"           \
    __parm              [__es __di] [__dx __si] [__cx] \
    __value             \
    __modify __exact    [__di __si __cx]

extern void     movefwd( char _WCFAR *d, const char _WCFAR *s, size_t len);
#pragma aux movefwd = \
        "push ds"       \
        "mov  ds,dx"    \
        "shr  cx,1"     \
        "rep  movsw"    \
        "adc  cx,cx"    \
        "rep  movsb"    \
        "pop  ds"       \
    __parm              [__es __di] [__dx __si] [__cx] \
    __value             \
    __modify __exact    [__di __si __cx]

#endif

#elif defined(__386__)

#if defined(__SMALL_DATA__)

#define HAVE_INLINE_MOVE

extern void     movefwd( char _WCFAR *d, const char _WCNEAR *s, size_t len);
#pragma aux movefwd = \
        "push es"       \
        "mov  es,edx"   \
        "push ecx"      \
        "shr  ecx,2"    \
        "rep  movsd"    \
        "pop  ecx"      \
        "and  ecx,3"    \
        "rep  movsb"    \
        "pop  es"       \
    __parm              [__dx __edi] [__esi] [__ecx] \
    __value             \
    __modify __exact    [__edi __esi __ecx]
extern void     movebwd( char _WCFAR *d, const char _WCNEAR *s, size_t len);
#pragma aux movebwd = \
        "std"           \
        "push es"       \
        "mov  es,edx"   \
        "dec  esi"      \
        "dec  edi"      \
        "shr  ecx,1"    \
        "rep  movsw"    \
        "adc  ecx,ecx"  \
        "inc  esi"      \
        "inc  edi"      \
        "rep  movsb"    \
        "pop  es"       \
        "cld"           \
    __parm              [__dx __edi] [__esi] [__ecx] \
    __value             \
    __modify __exact    [__edi __esi __ecx]

#else

#define HAVE_INLINE_MOVE

extern void     movefwd( char _WCFAR *d, const char _WCFAR *s, size_t len);
#pragma aux movefwd = \
        "push ds"       \
        "mov  ds,edx"   \
        "push ecx"      \
        "shr  ecx,2"    \
        "rep  movsd"    \
        "pop  ecx"      \
        "and  ecx,3"    \
        "rep  movsb"    \
        "pop  ds"       \
    __parm              [__es __edi] [__dx __esi] [__ecx] \
    __value             \
    __modify __exact    [__edi __esi __ecx]
extern void     movebwd( char _WCFAR *d, const char _WCFAR *s, size_t len);
#pragma aux movebwd = \
        "std"           \
        "push ds"       \
        "mov  ds,edx"   \
        "dec  esi"      \
        "dec  edi"      \
        "shr  ecx,1"    \
        "rep  movsw"    \
        "adc  ecx,ecx"  \
        "inc  esi"      \
        "inc  edi"      \
        "rep  movsb"    \
        "pop  ds"       \
        "cld"           \
    __parm              [__es __edi] [__dx __esi] [__ecx] \
    __value             \
    __modify __exact    [__edi __esi __ecx]

#endif

#else

// no pragma for non-x86

#endif


_WCRTLINK void *memmove( void *vd, const void *vs, size_t len )
{
    const char      *s = vs;
    char            *d = vd;

    if( s == d || len == 0 ) {
        return( d );
    }
    if( s < d && s + len > d ) {  /* if buffers are overlapped */
#if defined( __HUGE__ ) || !defined( HAVE_INLINE_MOVE )
        d += len;
        s += len;
        while( len != 0 ) {
            *--d = *--s;
            len--;
        }
#else
        movebwd(( d + len ) - 1, ( s + len ) - 1, len );
#endif
    } else {
#if !defined( HAVE_INLINE_MOVE )
        while( len != 0 ) {
            *d++ = *s++;
            len--;
        }
#else
        movefwd( d, s, len );
#endif
    }

#if defined(__HUGE__) || !defined( HAVE_INLINE_MOVE )
    return( vd );
#else
    return( d );
#endif
}
