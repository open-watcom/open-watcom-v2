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
#include <malloc.h>
#include <mbstring.h>
#include <string.h>
#include "farfunc.h"



/****
***** Reverse the order of characters in a string.  Equivalent to strrev().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsrev,_fmbsrev)( unsigned char _FFAR *string )
{
    size_t                  bytes_left, char_bytes;
    unsigned int            ch;
    unsigned char _FFAR *   start;
    unsigned char _FFAR *   end;
    unsigned char _FFAR *   endMarker;
    unsigned char           mbc[MB_LEN_MAX+1];

//    if( !__IsDBCS )  return( strrev( string ) );

    /*** Handle the empty string case ***/
    if( _NEARFAR(_mbterm,_fmbterm)(string) )  return( string );

    /*** Determine the size of the string in bytes ***/
    for( end=string; !_NEARFAR(_mbterm,_fmbterm)(end); end=_NEARFAR(_mbsinc,_fmbsinc)(end) )
        ;
    bytes_left = end - string;                  /* determine string size */
    endMarker = end;                            /* point to NULL char */

    /*** Move the characters from the end to the start, one by one ***/
    start = string;                             /* get string start */
    while( bytes_left > 0 ) {
        end = _NEARFAR(_mbsdec,_fmbsdec)( string, endMarker ); /* back up */
        ch = _NEARFAR(_mbsnextc,_fmbsnextc)( end ); /* get the character */
        _NEARFAR(_mbvtop,_fmbvtop)( ch, mbc );   /* convert from unsigned int */
        char_bytes = _NEARFAR(_mbclen,_fmbclen)( mbc ); /* get char size in bytes */
        bytes_left -= char_bytes;                /* update remaining bytes */
        _NEARFAR(memmove,_fmemmove)( start+char_bytes, start, bytes_left ); /* shift chars */
        _NEARFAR(_mbccpy,_fmbccpy)( start, mbc );/* copy the character */
        start += char_bytes;                     /* update start pointer */
    }

    return( string );
}
