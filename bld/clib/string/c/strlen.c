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
* Description:  Implementation of strlen() and wcslen().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include "xstring.h"
#include "riscstr.h"
#undef  strlen


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK size_t __simple_wcslen( const CHAR_TYPE *s )    /* return length of s */
#else
 _WCRTLINK size_t __F_NAME(strlen,wcslen)( const CHAR_TYPE *s )    /* return length of s */
#endif
{

#if defined(__INLINE_FUNCTIONS__) && !defined(__WIDECHAR__) && defined(_M_IX86)
    return( _inline_strlen( s ) );
#else
    const CHAR_TYPE *p;

    p = s;
    while( *p != NULLCHAR )
        ++p;
    return( p - s );
#endif
}
