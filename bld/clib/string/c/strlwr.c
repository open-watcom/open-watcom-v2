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
* Description:  Implementation of strlwr() and _wcslwr().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <ctype.h>
#include <string.h>
#include "riscstr.h"


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK CHAR_TYPE *__simple__wcslwr( CHAR_TYPE *str ) {
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strlwr,_wcslwr)( CHAR_TYPE *str ) {
#endif
    CHAR_TYPE    *p;
    UCHAR_TYPE   c;

    for( p = str; (c = *p) != NULLCHAR; ++p ) {
        if( IS_ASCII( c ) ) {
            c -= STRING( 'A' );
            if( c <= STRING( 'Z' ) - STRING( 'A' ) ) {
                c += STRING( 'a' );
                *p = c;
            }
        }
    }
    return( str );
}
