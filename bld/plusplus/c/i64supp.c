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


#include "plusplus.h"
#include "i64.h"


void Int64From32                // CREATE 64-BIT VALUE FROM 32-BIT VALUE
    ( TYPE type                 // - source integral type (signed or unsigned)
    , signed_32 value           // - integral value (signed or unsigned)
    , signed_64* result )       // - addr[ result (signed or unsigned) ]
{
    if( SignedIntType( type ) ) {
        I32ToI64( value, result );
    } else {
        U32ToU64( (unsigned_32)value, (unsigned_64*)result );
    }
}

void Int64FromU32               // CREATE 64-BIT VALUE FROM UNSIGNED 32-BIT VALUE
    ( unsigned_32 value         // - integral value (unsigned)
    , unsigned_64* result )     // - addr[ result (unsigned) ]
{
    U32ToU64( value, result );
}
