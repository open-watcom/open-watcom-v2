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
#include <string.h>
#include <ctype.h>

_WCRTLINK int _fmemicmp( const void _WCFAR *v1, const void _WCFAR *v2, size_t len )
    {
        unsigned char c1;
        unsigned char c2;
        const unsigned char _WCFAR *s1;
        const unsigned char _WCFAR *s2;

        for( s1 = v1, s2 = v2 ; len; --len )  {
            c1 = *s1;
            if( c1 >= 'A'  &&  c1 <= 'Z' )  c1 += 'a' - 'A';
            c2 = *s2;
            if( c2 >= 'A'  &&  c2 <= 'Z' )  c2 += 'a' - 'A';
            if( c1 != c2 ) return( c1 - c2 );
            ++s1;
            ++s2;
        }
        return( 0 );    /* both operands are equal */
    }
