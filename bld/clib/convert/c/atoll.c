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
#include "widechar.h"
#include "watcom.h"
#include "clibi64.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

_WCRTLINK void __F_NAME(__clib_atoll,__clib_watoll)( const CHAR_TYPE *p, UINT64_TYPE *pv )  /* convert ASCII string to long integer */
    {
        UINT64_TYPE value;
        UINT64_TYPE radix;
        UINT64_TYPE digit;
        CHAR_TYPE    sign;

        __ptr_check( p, 0 );

        while( __F_NAME(isspace,iswspace)( *p ) ) ++p;
        sign = *p;
        if( sign == '+' || sign == '-' ) ++p;
        _clib_U32ToU64( 0, value );
        _clib_U32ToU64( 10, radix );
        _clib_U32ToU64( 0, digit );
        while( __F_NAME(isdigit,iswdigit)(*p) ) {
            _clib_U64Mul( value, radix, value );
            _clib_U32ToU64( *p - '0', digit );
            _clib_U64Add( value, digit, value );
            ++p;
        }
        if( sign == '-' ) _clib_U64Neg( value, value );
        *pv = value;
    }
