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
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
#else
    #include <ctype.h>
#endif
#include "mbchar.h"
#include <mbstring.h>

/**
 * Convert a single character to uppercase.  Equivalent to toupper().
 */

_WCRTLINK unsigned int _mbctoupper( unsigned int ch )
{
#ifdef __NT__
    unsigned char       mbc[MB_LEN_MAX+1];

    mbc[1] = '\0';      // [0] and possibly [1] are set with chars
    mbc[2] = '\0';
    _mbvtop( ch, mbc );
    CharUpper( (char *)mbc ); // call the Win32 API
    return( _mbsnextc( mbc ) );
#else                               /* OS/2 and others */
    if( ch & 0xFF00 ) {
        if( __MBCodePage == 932 ) {
            return( _mbdtoupper( ch ) );
        }
        return( ch );   // we don't know what to do - just return it
    } else {
        return( toupper( ch ) );
    }
#endif
}
