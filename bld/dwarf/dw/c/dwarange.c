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


#include "dwpriv.h"
#include "dwutils.h"
#include "dwarange.h"

void DWENTRY DWAddress(
    dw_client                   cli,
    uint_32                     len )
{
    char                        buf[ sizeof( uint_32 ) ];

    CLIReloc2( DW_DEBUG_ARANGES, DW_W_ARANGE_ADDR );
    if( cli->offset_size  == sizeof( uint_32 ) ){
        WriteU32( buf, len );
    }else{
        WriteU16( buf, len );
    }
    CLIWrite( DW_DEBUG_ARANGES, buf, cli->offset_size );
}


void InitDebugAranges(
    dw_client                   cli )
{
    char                        buf[ sizeof( uint_32 )];

    CLISeek( DW_DEBUG_ARANGES, sizeof( uint_32 ), DW_SEEK_CUR );
    WriteU32( buf, 2 );
    CLIWrite( DW_DEBUG_ARANGES, buf, sizeof( uint_16 ) );
    CLIReloc3( DW_DEBUG_ARANGES, DW_W_SECTION_POS, DW_DEBUG_INFO );
    buf[0] = cli->offset_size;
    buf[1] = cli->segment_size;
    CLIWrite( DW_DEBUG_ARANGES, buf, 2 );
}


void FiniDebugAranges(
    dw_client                   cli )
{
    static char const  zeros[ sizeof( uint_32 ) ] = { 0 };
    long                        size;
    char                        buf[ sizeof( uint_32 ) ];

    CLIWrite( DW_DEBUG_ARANGES, zeros, cli->offset_size );
    CLIWrite( DW_DEBUG_ARANGES, zeros, cli->segment_size  );
    CLIWrite( DW_DEBUG_ARANGES, zeros, cli->offset_size );

    size = CLITell( DW_DEBUG_ARANGES ) - sizeof( uint_32 )
        - cli->section_base[ DW_DEBUG_ARANGES ];
    WriteU32( buf, size );
    CLISeek( DW_DEBUG_ARANGES, cli->section_base[ DW_DEBUG_ARANGES ],
        DW_SEEK_SET );
    CLIWrite( DW_DEBUG_ARANGES, buf, sizeof( buf ) );
    CLISeek( DW_DEBUG_ARANGES, 0, DW_SEEK_END );
}
