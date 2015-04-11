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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <mbstring.h>
#include <stdlib.h>
#include <string.h>
#include "farfunc.h"



/****
***** Convert a wide character string to a multibyte character string.
****/

_WCRTLINK size_t _NEARFAR(wcstombs,_fwcstombs)( char _FFAR *s, const wchar_t _FFAR *pwcs, size_t n )
{
    char                mbc[MB_LEN_MAX+1];
    size_t              numBytes = 0;
    size_t              rc;

    if( s != NULL ) {
        while( n > 0 ) {                /* convert the string */
            if( *pwcs != L'\0' ) {
                rc = wctomb( mbc, *pwcs );
                if( rc == -1 )  return( -1 );
                if( rc > n )  break;
                _NEARFAR(memcpy,_fmemcpy)( s, mbc, rc );
                s += rc;
                n -= rc;
                numBytes += rc;
                pwcs++;
            } else {
                *s = '\0';
                break;
            }
        }
    } else {                            /* get required size */
        for( ;; ) {
            if( *pwcs != L'\0' ) {
                rc = wctomb( mbc, *pwcs );
                if( rc == -1 )  return( -1 );
                numBytes += rc;
                pwcs++;
            } else {
                break;
            }
        }
    }
    return( numBytes );
}
