/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include <ctype.h>
#include "bool.h"
#include "wstrcmp.h"


int WresStrnicmp( const void *p1, const void *p2, size_t len )
/*************************************************************
 * comparision is handled specialy
 * if compare two alphabet characters then compare lower cased values
 * if compare non-alphabet character with alphabet character then
 * original values are used for comparision
 */
{
    char        ch1;
    char        ch2;
    size_t      i;
    bool        a1;
    bool        a2;

    for( i = 0; i < len; i++ ) {
        ch1 = ((const char *)p1)[i];
        if( ch1 >= 'A' && ch1 <= 'Z' ) {
            a1 = true;
            ch1 += 'a' - 'A';
        } else if( ch1 >= 'a' && ch1 <= 'z' ) {
            a1 = true;
        } else {
            a1 = false;
        }
        ch2 = ((const char *)p2)[i];
        if( ch2 >= 'A' && ch2 <= 'Z' ) {
            a2 = true;
            ch2 += 'a' - 'A';
        } else if( ch2 >= 'a' && ch2 <= 'z' ) {
            a2 = true;
        } else {
            a2 = false;
        }
        if( ch1 != ch2 ) {
            if( a1 != a2 ) {
                if( a1 ) {
                    ch1 = ((const char *)p1)[i];
                } else {
                    ch2 = ((const char *)p2)[i];
                }
            }
            return( ( ch1 < ch2 ) ? -1 : 1 );
        }
        if( ch1 == 0 ) {
            break;      /* equal to 0, end of string */
        }
    }
    return( 0 );
}
