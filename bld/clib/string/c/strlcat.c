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
* Description:  Implementation of BSD style strlcat() and wcslcat().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include "libwchar.h"


_WCRTLINK size_t __F_NAME(strlcat,wcslcat)( CHAR_TYPE *s, const CHAR_TYPE *t, size_t n )
{
    CHAR_TYPE   *p;
    size_t      len;

    // Find end of string in destination buffer but don't overrun
    for( p = s, len = n; len != 0 && *p != NULLCHAR; --len ) {
        ++p;
    }
    // If no null char was found in s, the buffer is messed up; don't
    // touch it
    if( *p == NULLCHAR ) {
        // Decrement len to leave space for terminating null
        for( --len; len != 0; ++p, ++t, --len ) {
            if( (*p = *t) == NULLCHAR ) {
                return( n - len - 1 );
            }
        }
        // Buffer not large enough. Terminate and figure out desired length
        *p = NULLCHAR;
        while( *t++ != NULLCHAR )
            ++n;
        --n;
    }
    return( n );
}
