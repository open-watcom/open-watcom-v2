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
***** Compare two strings, up to a specified maximum number of characters.
***** Equivalent to strncmp().
****/

_WCRTLINK int _NEARFAR(_mbsncmp,_fmbsncmp)( const unsigned char _FFAR *s1, const unsigned char _FFAR *s2, size_t n )
{
    int                 status = 0;

//    if( !__IsDBCS )  return( strncmp( s1, s2, n ) );

    /*** Compare characters, one by one ***/
    while( n>0 && !_NEARFAR(_mbterm,_fmbterm)(s1) && !_NEARFAR(_mbterm,_fmbterm)(s2) ) {
        status = _NEARFAR((_mbccmp),(_fmbccmp))( s1, s2 ); /* compare chars */
        if( status != 0 )  return( status );    /* if error, break from loop */
        s1 = _NEARFAR(_mbsinc,_fmbsinc)( s1 );  /* skip over character */
        s2 = _NEARFAR(_mbsinc,_fmbsinc)( s2 );  /* skip over character */
        n--;                                    /* update counter */
    }

    if( n > 0  &&  ( _NEARFAR(_mbterm,_fmbterm)(s1) || _NEARFAR(_mbterm,_fmbterm)(s2) ) )
        return( *s1 - *s2 );
    else
        return( status );
}
