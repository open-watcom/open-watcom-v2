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
#endif
#include <string.h>
#include <mbstring.h>
#include "farfunc.h"

/**
 * Convert a string to lowercase.  Equivalent to strlwr().
 */

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbslwr,_fmbslwr)( unsigned char _FFAR *string )
{
#if defined(__NT__) && !defined(__FARFUNC__)
    CharLower( string );    // call the Win32 API
#else
    unsigned int            ch;
    unsigned char _FFAR *   p;
    char                    mbc[MB_LEN_MAX+1];

    p = string;
    while( !_NEARFAR(_mbterm,_fmbterm)(p) ) {
        ch = _mbctolower( _NEARFAR(_mbsnextc,_fmbsnextc)(p) );
        _NEARFAR(_mbvtop,_fmbvtop)( ch, mbc );
        mbc[_NEARFAR(_mbclen,_fmbclen)(mbc)] = '\0';
        _NEARFAR(_mbccpy,_fmbccpy)( p, mbc );
        p = _NEARFAR(_mbsinc,_fmbsinc)( p );
    }
#endif
    return( string );
}
