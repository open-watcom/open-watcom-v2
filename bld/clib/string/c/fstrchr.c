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
* Description:  Implementation of _fstrchr() - far strchr().
*
****************************************************************************/


#include "variety.h"
#include <string.h>

#ifdef  _M_I86

/* use scan1 to find the char we are looking for */

extern  char _WCFAR *_scan1( char _WCFAR *s, int c );
#pragma aux _scan1 = \
        "push ds"       \
        "mov  ds,dx"    \
    "L1: lodsw"         \
        "cmp  al,bl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,bl"    \
        "je short L4"   \
        "test ah,ah"    \
        "je short L2"   \
        "lodsw"         \
        "cmp  al,bl"    \
        "je short L3"   \
        "test al,al"    \
        "je short L2"   \
        "cmp  ah,bl"    \
        "je short L4"   \
        "test ah,ah"    \
        "jne short L1"  \
    "L2: xor  si,si"    \
        "mov  dx,si"    \
        "jmp short L5"  \
    "L3: dec  si"       \
    "L4: dec  si"       \
    "L5: pop  ds"       \
    __parm __caller [__dx __si] [__bl] \
    __value         [__dx __si] \
    __modify        [__ax __dx __si]
#endif


/* locate the first occurrence of c in the initial n characters of the
   string pointed to by s. The terminating null character is considered
   to be part of the string.
   If the character c is not found, NULL is returned.
*/
#undef  _fstrchr

_WCRTLINK char _WCFAR *_fstrchr( const char _WCFAR *s, int c )
{
//#if defined( _M_I86 )
    //return( _scan1( (char _WCFAR *)s, c ) );
//#else
    do {
        if( *s == c ) return( (char _WCFAR *)s );
    } while( *s++ != '\0' );
    return( NULL );
//#endif
}
