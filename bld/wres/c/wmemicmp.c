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


#include <stddef.h>
#include <ctype.h>
#include "wmemicmp.h"


int WresMemicmp( const void *p1, const void *p2, size_t len )
/************************************************************
 * Kludge to get around memicmp behavior where comparing upper case letters
 * against characters in the range z-A would return the negative
 * result */
{
    char        ch1;
    char        ch2;
    size_t      i;

    for( i = 0; i < len; i++ ) {
        ch1 = (char)toupper( ((const char *)p1)[i] );
        ch2 = (char)toupper( ((const char *)p2)[i] );
        if( ch1 != ch2 ) {
            return( ch1 - ch2 );
        }
    }
    return( 0 );
}
