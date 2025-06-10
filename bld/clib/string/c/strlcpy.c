/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of BSD style strlcpy() and wcslcpy().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include "libwchar.h"


_WCRTLINK size_t __F_NAME(strlcpy,wcslcpy)( CHAR_TYPE *s, const CHAR_TYPE *t, size_t len )
{
    const CHAR_TYPE     *p;
    size_t              count;

    if( len ) {
        count = len;
        // leave space for terminating null
        for( --len; len != 0 && *t != NULLCHAR; --len ) {
            *s++ = *t++;
        }
        *s = NULLCHAR;          // terminate 's'
    } else {
        count = 1;              // account for not decrementing 'len'
    }

    if( len == 0 ) {            // source string was truncated
        for( p = t; *p != NULLCHAR; ++p ) {
            /* empty */;
        }
        count += p - t;         // find out how long 't' really is
    }
    return( count - len - 1 );  // final null doesn't count
}
