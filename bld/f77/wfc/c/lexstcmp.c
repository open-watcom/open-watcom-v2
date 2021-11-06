/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  compare 2 strings
*
****************************************************************************/


#include "ftnstd.h"
#include "lexstcmp.h"


static  int     ChkBlanks( char *ptr, size_t len )
//================================================
{
    for(;;) {
        --len;
        if( len == 0 )
            break;
        if( *ptr != ' ' )
            break;
        ++ptr;
    }
    return( *ptr - ' ' );
}


int     LexStrCmp( char *ptr1, size_t len1, char *ptr2, size_t len2 )
//===================================================================
// Compare two strings (compile-time entry point).
{
    int         result;

    for(;;) {
        if( *ptr1 != *ptr2 )
            break;
        ++ptr1;
        ++ptr2;
        --len1;
        --len2;
        if( len1 == 0 )
            break;
        if( len2 == 0 ) {
            break;
        }
    }
    if( len1 == 0 ) {
        if( len2 == 0 )
            return( 0 );
        result = -ChkBlanks( ptr2, len2 );
    } else if( len2 == 0 ) {
        result = ChkBlanks( ptr1, len1 );
    } else {
        result = *ptr1;
        result -= *ptr2;
    }
    if( result < 0 ) {
        result = -1;
    } else if( result > 0 ) {
        result = 1;
    }
    return( result );
}
