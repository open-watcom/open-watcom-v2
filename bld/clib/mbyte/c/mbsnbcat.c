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
***** bytes.  Equivalent to strncat().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsnbcat,_fmbsnbcat)( unsigned char _FFAR *s1, const unsigned char _FFAR *s2, size_t n )
{
    unsigned char _FFAR *   string_start = s1;
    unsigned char _FFAR *   p1;
    unsigned char _FFAR *   p2;
    size_t                  bytes;

//    if( !__IsDBCS )  return( strncat( s1, s2, n ) );

    /*** Abort if n==0 ***/
    if( n == 0 )  return( s1 );

    /*** Determine where to begin copying ***/
    if( *s1 != '\0' ) {
        p2 = _NEARFAR(strchr,_fstrchr)( s1, '\0' ) - 1; /* find null byte */
        p1 = s1;                                /* start at string start */
        while( *p1 != '\0' ) {                  /* until last character */
            if( p1 == p2  &&  _ismbblead(*p1) ) {/* s1 ends in a lead byte? */
                s1 = p2;                        /* yes, smite last byte */
                break;
            }
            p1 = _NEARFAR(_mbsinc,_fmbsinc)( p1 );
        }
        if( s1 != p2 )  s1 = p2+1;              /* no, point at null byte */
    }

    /*** Copy up to 'n' characters ***/
    while( *s2 != '\0' ) {
        bytes = _NEARFAR(_mbclen,_fmbclen)( s2 ); /* char size in bytes */
        if( n >= bytes ) {
            _NEARFAR(_mbccpy,_fmbccpy)( s1, s2 ); /* copy a character */
            s1 = _NEARFAR(_mbsinc,_fmbsinc)( s1 );/* skip over character */
            s2 = _NEARFAR(_mbsinc,_fmbsinc)( s2 );/* skip over character */
            n -= bytes;                           /* update counter */
        } else {
            while( n-- > 0 ) {                  /* partial char */
                *s1++ = '\0';
            }
            break;
        }
    }

    /*** Pad with a terminating NULL character ***/
    *s1 = '\0';                                 /* append NULL char */

    return( string_start );
}
