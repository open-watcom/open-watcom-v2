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
***** Append one string to another, up to a specified maximum number of
***** characters.  Equivalent to strncat().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsncat,_fmbsncat)( unsigned char _FFAR *s1, const unsigned char _FFAR *s2, size_t n )
{
    unsigned char _FFAR *string_start = s1;
    size_t              bytes;

//    if( !__IsDBCS )  return( strncat( s1, s2, n ) );

    bytes = _NEARFAR(_mbsnbcnt,_fmbsnbcnt)( s2, n );/* bytes in src */
    s1 += _NEARFAR(strlen,_fstrlen)( (char _FFAR *)s1 );          /* point to end of string */
    _NEARFAR(memcpy,_fmemcpy)( s1, s2, bytes );     /* copy the data */
    *(s1+bytes) = '\0';                             /* add terminating null */
    return( string_start );
}
