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
***** Find the first occurrence in 'string' of any character from 'charset'.
***** Equivalent to strpbrk().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbspbrk,_fmbspbrk)( const unsigned char _FFAR *string, const unsigned char _FFAR *charset )
{
//    if( !__IsDBCS )  return( strpbrk( string, charset ) );

    /*** Find first char from 'charset' in 'string' ***/
    while( !_NEARFAR(_mbterm,_fmbterm)(string) ) {
        #ifdef __FARFUNC__
        if( _fmbschr(charset,_fmbsnextc(string)) == NULL )
        #else
        if( _mbschr(charset,_mbsnextc(string)) == NULL )
        #endif
            string = _NEARFAR(_mbsinc,_fmbsinc)( string );  /* not found */
        else
            return( (unsigned char _FFAR*) string );        /* found */
    }

    return( NULL );                             /* signal not found */
}
