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
* Description: Compare two characters without case-sensitivity
*
****************************************************************************/


#include "variety.h"
#include <ctype.h>
#include <mbstring.h>
#include "farfunc.h"



_WCRTLINK int _NEARFAR((_mbcicmp),(_fmbcicmp))( const unsigned char _FFAR *c1, const unsigned char _FFAR *c2 )
{
    int                 retval;
    unsigned int        ch1, ch2;
    unsigned char       ch1hi, ch2hi;

    ch1 = _mbctoupper( _NEARFAR(_mbsnextc,_fmbsnextc)(c1) );
    ch2 = _mbctoupper( _NEARFAR(_mbsnextc,_fmbsnextc)(c2) );

    ch1hi = (unsigned char)(ch1 >> 8);
    ch2hi = (unsigned char)(ch2 >> 8);

    retval = ch1hi - ch2hi;
    if( retval == 0 )
        retval = (unsigned char)ch1 - (unsigned char)ch2;
    return retval;
}
