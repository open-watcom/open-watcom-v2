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
***** Return the address of the previous character in a string.  If
***** stringStart>=ch, returns NULL.
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbsdec,_fmbsdec)( const unsigned char _FFAR *str, const unsigned char _FFAR *ch )
{
    unsigned char _FFAR *p;

    if( str >= ch )  return( NULL );
//    if( !__IsDBCS )  return( (unsigned char*)ch - 1 );

    p = (unsigned char _FFAR*)ch - 1;       /* address of previous byte */
    if( _ismbblead( *p ) )  return( p - 1 );

    while( str <= --p  &&  _ismbblead( *p ) )
        ;
    return( (unsigned char _FFAR*) ch - 1 - (ch-p)%2 );
}
