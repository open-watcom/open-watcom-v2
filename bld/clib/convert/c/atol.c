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
* Description:  Implementation of atol() - convert string to long.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#else
    #include <ctype.h>    
#endif


_WCRTLINK long int __F_NAME(atol,_wtol)( const CHAR_TYPE *p )
{
    long int        value;
    CHAR_TYPE       sign;

    __ptr_check( p, 0 );

    while( __F_NAME(isspace,iswspace)( *p ) )
        ++p;
    sign = *p;
    if( sign == '+' || sign == '-' )
        ++p;
    value = 0;
    while( __F_NAME(isdigit,iswdigit)(*p) ) {
        value = value * 10 + *p - '0';
        ++p;
    }
    if( sign == '-' )
        value = - value;
    return( value );
}
