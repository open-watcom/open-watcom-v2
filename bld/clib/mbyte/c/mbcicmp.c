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
#include <ctype.h>
#include <mbstring.h>
#include "farfunc.h"



/****
***** Compare two characters, without case-sensitivity.
****/

_WCRTLINK int _NEARFAR((_mbcicmp),(_fmbcicmp))( const unsigned char _FFAR *c1, const unsigned char _FFAR *c2 )
{
    unsigned char       s1[MB_LEN_MAX+1];
    unsigned char       s2[MB_LEN_MAX+1];

//    if( !__IsDBCS )  return( toupper(*c1) - toupper(*c2) );

    /*** Initialize 's1' and 's2' ***/
    _NEARFAR(_mbccpy,_fmbccpy)( s1, c1 );
    s1[_NEARFAR(_mbclen,_fmbclen)(c1)] = '\0';
    _NEARFAR(_mbccpy,_fmbccpy)( s2, c2 );
    s2[_NEARFAR(_mbclen,_fmbclen)(c2)] = '\0';

    /*** Compare characters without case-sensitivity ***/
    _NEARFAR(_mbsupr,_fmbsupr)( s1 );
    _NEARFAR(_mbsupr,_fmbsupr)( s2 );
    return( _NEARFAR(_mbccmp,_fmbccmp)(s1,s2) );
}
