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
#include <string.h>
#include "farfunc.h"



/****
***** Copy one string over another, up to a specified number of
***** bytes.  Equivalent to strncpy().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsnbcpy,_fmbsnbcpy)( unsigned char _FFAR *dst, const unsigned char _FFAR *src, size_t n )
{
    //int   len;
    unsigned char _FFAR *d = dst;

//    if( !__IsDBCS )  return( strncpy( dst, src, n ) );

    //len = _NEARFAR(strlen,_fstrlen)( src );
    //if( _ismbblead(*(src+n)) && n<len )  n--;     /* only whole chars */

    while (n > 0) {
        if (*src == 0x00) {
            break;
        }

        *d++ = *src++;
        --n;
    }

    if (*src && (d > dst)) {          // only whole chars
        if (_ismbblead(src[-1])) {
            d[-1] = 0x00;
        }
    }

    if (n > 0) {
        _NEARFAR(memset, _fmemset)(d, 0, n);
    }

    return dst;
}
