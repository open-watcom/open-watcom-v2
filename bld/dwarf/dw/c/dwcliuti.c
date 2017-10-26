/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Client independent utility functions.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwutils.h"
#include "dwcliuti.h"


void CLIWriteLEB128( dw_client cli, dw_sectnum sect, dw_sconst value )
{
    uint_8              buf[MAX_LEB128];

    CLIWrite( cli, sect, buf, LEB128( buf, value ) - buf );
}


void CLIWriteULEB128( dw_client cli, dw_sectnum sect, dw_uconst value )
{
    uint_8              buf[MAX_LEB128];

    CLIWrite( cli, sect, buf, ULEB128( buf, value ) - buf );
}

void CLISectionSetSize( dw_client cli, dw_sectnum sect )
/* backpatch the section length */
{
    dw_sect_offs    size;

    size = CLISectionOffset( cli, sect ) - sizeof( size );
    CLISectionSeekOffset( cli, sect, 0 );
    CLIWriteU32( cli, sect, size );
    CLISectionSeekEnd( cli, sect );
}

void CLISectionWriteZeros( dw_client cli, dw_sectnum sect, size_t len )
{
    // the zeros array length must be big enought for all calls, now 16 bytes is OK
    static const uint_8     zeros[16] = { 0 };

    CLIWrite( cli, sect, zeros, len );
}

void CLIWriteU8( dw_client cli, dw_sectnum sect, uint_8 data )
{
    CLIWrite( cli, sect, &data, sizeof( data ) );
}

void CLIWriteU16( dw_client cli, dw_sectnum sect, uint_16 data )
{
    char            buf[sizeof( uint_16 )];

    WriteU16( buf, data );
    CLIWrite( cli, sect, buf, sizeof( buf ) );
}

void CLIWriteU32( dw_client cli, dw_sectnum sect, uint_32 data )
{
    char            buf[sizeof( uint_32 )];

    WriteU32( buf, data );
    CLIWrite( cli, sect, buf, sizeof( buf ) );
}

void CLIWriteString( dw_client cli, dw_sectnum sect, const char *str )
{
    CLIWrite( cli, sect, str, strlen( str ) + 1 );
}
