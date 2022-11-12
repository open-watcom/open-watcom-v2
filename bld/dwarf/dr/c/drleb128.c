/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  signed/unsigned LEB128 reading procedures
*
****************************************************************************/


#include "drpriv.h"
#include "drleb128.h"


int_64 SLEB128( void **h, uint_8 (*ifn)(void **h) )
/*************************************************/
{
    uint_64     value;
    uint        shift;
    uint_8      byte;

    value = 0;
    shift = 0;
    do {
        byte = ifn( h );
        value |= (uint_64)( byte & 0x7f ) << shift;
        shift += 7;
    } while( byte & 0x80 );
    if( (byte & 0x40) && ( shift < 64 ) ) {
        value |= ~0ULL << shift;
    }
    return( (int_64)value );
}

uint_64 ULEB128( void **h, uint_8 (*ifn)(void **h) )
/**************************************************/
{
    uint_64     value;
    uint        shift;
    uint_8      byte;

    value = 0;
    shift = 0;
    do {
        byte = ifn( h );
        value |= (uint_64)( byte & 0x7f ) << shift;
        shift += 7;
    } while( byte & 0x80 );
    return( value );
}
