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
***** Find the last occurrence of 'ch' in 'string'.  Equivalent to strrchr().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsrchr,_fmbsrchr)( const unsigned char _FFAR *string, unsigned int ch )
{
    unsigned char _FFAR *   p = NULL;
    unsigned int            chTemp;

//    if( !__IsDBCS && !ch&0xFF00 )  return( strrchr( string, ch ) );

    /*** Search for the specified character ***/
    while( !_NEARFAR(_mbterm,_fmbterm)(string) ) {
        chTemp = _NEARFAR(_mbsnextc,_fmbsnextc)( string ); /* get character */
        if( chTemp == ch )                      /* is it the search char? */
            p = (unsigned char _FFAR*) string;  /* yes, store this address */
        string = _NEARFAR(_mbsinc,_fmbsinc)( string );  /* skip character */
    }

    if( ch != 0 )                               /* looking for NULL? */
        return( p );                            /* no, return last address */
    else
        return( (unsigned char _FFAR*) string );/* yes, return its address */
}
