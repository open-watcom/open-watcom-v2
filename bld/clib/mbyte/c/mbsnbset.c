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
#include "farfunc.h"



/****
***** Set all characters in 'string' to 'ch', overwriting any partial bytes
***** at the end with ' '.  Equivalent to strnset().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsnbset,_fmbsnbset)( unsigned char _FFAR *string, unsigned int ch, size_t n )
{
    size_t                  char_size;
    unsigned char _FFAR *   string_start;
    unsigned char           mbc[MB_LEN_MAX+1];

//    if( !__IsDBCS && !ch&0xFF00 )  return( strnset( string, ch, n ) );

    /*** Set every character in the string to 'ch' ***/
    _NEARFAR(_mbvtop,_fmbvtop)( ch, mbc );  /* convert from unsigned int */
    char_size = _NEARFAR(_mbclen,_fmbclen)( mbc );    /* get char size */
    string_start = string;                            /* save string start */
    while( n >= char_size ) {
        _NEARFAR(_mbccpy,_fmbccpy)( string, mbc );    /* copy the character */
        string = _NEARFAR(_mbsinc,_fmbsinc)( string );/* point to next spot */
        n -= char_size;                               /* update counter */
    }

    /*** Pad any remaining bytes with blanks ***/
    while( n-- > 0 ) {
        *(string++) = ' ';
    }

    return( string_start );
}
