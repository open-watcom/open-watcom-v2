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
***** Find the first occurrence of 'ch' in 'string'.  Equivalent to strchr().
****/

_WCRTLINK unsigned char _FFAR *_NEARFAR(_mbschr,_fmbschr)( const unsigned char _FFAR *string, unsigned int ch )
{
    char                mbc[MB_LEN_MAX+1];

//    if( !__IsDBCS && !ch&0xFF00 )  return( strchr( string, ch ) );

    /*** Search for the specified character ***/
    _mbvtop( ch, mbc );
    mbc[_mbclen(mbc)] = '\0';
    #ifdef __FARFUNC__
        while( !_fmbterm(string) && _fmbccmp(string,mbc)!=0 )
            string = _fmbsinc( string );        /* skip over character */
    #else
        while( !_mbterm(string) && _mbccmp(string,mbc)!=0 )
            string = _mbsinc( string );         /* skip over character */
    #endif

    /*** Return character address, or NULL if not found ***/
    if( !_NEARFAR(_mbterm,_fmbterm)(string) || ch==0 )
        return( (unsigned char _FFAR*) string );
    else
        return( NULL );
}
