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
***** Compare two strings, without case sensitivity, up to a specified
***** maximum number of bytes.  Equivalent to strnicmp().
****/

_WCRTLINK int _NEARFAR(_mbsnbicmp,_fmbsnbicmp)( const unsigned char _FFAR *s1, const unsigned char _FFAR *s2, size_t n )
{
    int                 status = 0;
    size_t              bytes;

//    if( !__IsDBCS )  return( strnicmp( s1, s2, n ) );

    /*** Compare characters, one by one ***/
    while( n>0 && !_NEARFAR(_mbterm,_fmbterm)(s1) && !_NEARFAR(_mbterm,_fmbterm)(s2) ) {
        if( n >= MB_LEN_MAX ) {
            status = _NEARFAR(_mbcicmp,_fmbcicmp)( s1, s2 );    /* compare */
        } else {
            status = _NEARFAR(strnicmp,_fstrnicmp)( (const char _FFAR *)s1, (const char _FFAR *)s2, n );/* compare */
        }
        if( status != 0 )  break;                   /* if error, exit loop */
        bytes = _NEARFAR(_mbclen,_fmbclen)( s1 );   /* char size in bytes */
        n = n>bytes ? n-bytes : 0;                  /* update counter */
        s1 = _NEARFAR(_mbsinc,_fmbsinc)( s1 );      /* skip over character */
        s2 = _NEARFAR(_mbsinc,_fmbsinc)( s2 );      /* skip over character */
    }

    return( status );
}
