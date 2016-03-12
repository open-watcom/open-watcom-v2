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
* Description:  Implementation of atoi() - convert string to int.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>
#endif
#include "bool.h"


_WCRTLINK int __F_NAME(atoi,_wtoi)( const CHAR_TYPE *p )  /* convert ASCII string to integer */
{
    int             value;
    bool            minus;

    __ptr_check( p, 0 );

    while( __F_NAME(isspace,iswspace)( (UCHAR_TYPE)*p ) )
        ++p;
    minus = false;
    switch( *p ) {
    case STRING( '-' ):
        minus = true;
        // fall down
    case STRING( '+' ):
        ++p;
        break;
    }
    value = 0;
    while( __F_NAME(isdigit,iswdigit)( (UCHAR_TYPE)*p ) ) {
        value = value * 10 + *p - STRING( '0' );
        ++p;
    }
    if( minus )
        value = - value;
    return( value );
}
