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
#include "clibi64.h"

/*
        Be careful of overlap between input parameters and output results.
*/

_WCRTLINK void __U64Mul( const UINT64_TYPE *a, const UINT64_TYPE *b, UINT64_TYPE *res )
{
    UINT64_TYPE         tmp_a;
    UINT64_TYPE         tmp_b;

    tmp_a = *a;
    tmp_b = *b;

    res->u._32[I64LO32] = 0;
    res->u._32[I64HI32] = 0;
    while( tmp_b.u._32[I64LO32] != 0 || tmp_b.u._32[0] != 0 ) {
        if( tmp_b.u._32[I64LO32] & 1 ) __U64Add( &tmp_a, res, res );
        __U64Shift( &tmp_a, -1, &tmp_a );
        __U64Shift( &tmp_b, 1, &tmp_b );
    }
}
