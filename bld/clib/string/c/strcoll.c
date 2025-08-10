/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of strcoll() and wcscoll().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "xstring.h"

#undef  strcoll



/* This function is identical to strcmp, except it is suppose to use
   the current locale.  We only support the "C" locale, so this code
   is identical to strcmp.
*/
/* return <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */


_WCRTLINK int __F_NAME(strcoll,wcscoll)( const CHAR_TYPE *s1, const CHAR_TYPE *s2 )
{
    for( ; *s1 == *s2; s1++, s2++ ) {
        if( *s1 == NULLCHAR ) {
            return( 0 );
        }
    }
    return( *s1 - *s2 );
}
