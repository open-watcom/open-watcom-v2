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
* Description:  Implementation of fmemchr().
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include "xstring.h"

/* locate the first occurrence of c in the initial n characters of the
   object pointed to by s.
   If the character c is not found, NULL is returned.
*/

#undef  _fmemchr

_WCRTLINK void _WCFAR *_fmemchr( const void _WCFAR *vs, int c, size_t n )
{
#if defined(__INLINE_FUNCTIONS__)
    return( _inline__fmemchr( vs, c, n ) );
#else
    const char _WCFAR *s = vs;
    while( n ) {
        if( *s == c ) return( (void _WCFAR *)s );
        ++s;
        --n;
    }
    return( NULL );
#endif
}
