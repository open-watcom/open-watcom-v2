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


/*
    manage the .debug_info section
*/

#include "dwpriv.h"
#include "dwutils.h"
#include "dwabbrev.h"
#include "dwmem.h"
#include "dwinfo.h"


void InfoReloc(
    dw_client                   cli,
    uint                        reloc_type )
{
    CLIReloc2( DW_DEBUG_INFO, reloc_type );
}


void Info8(
    dw_client                   cli,
    uint_8                      value )
{
    CLIWrite( DW_DEBUG_INFO, &value, sizeof( uint_8 ) );
}


void Info16(
    dw_client                   cli,
    uint_16                     value )
{
    char                        buf[ sizeof( uint_16 ) ];

    WriteU16( buf, value );
    CLIWrite( DW_DEBUG_INFO, &buf, sizeof( buf ) );
}


void Info32(
    dw_client                   cli,
    uint_32                     value )
{
    char                        buf[ sizeof( uint_32 ) ];

    WriteU32( buf, value );
    CLIWrite( DW_DEBUG_INFO, &buf, sizeof( buf ) );
}


void InfoLEB128(
    dw_client                   cli,
    dw_sconst                   value )
{
    char                        buf[ MAX_LEB128 ];
    char *                      end;

    end = LEB128( buf, value );
    CLIWrite( DW_DEBUG_INFO, buf, end - buf );
}


void InfoULEB128(
    dw_client                   cli,
    dw_uconst                   value )
{
    char                        buf[ MAX_LEB128 ];
    char *                      end;

    end = ULEB128( buf, value );
    CLIWrite( DW_DEBUG_INFO, buf, end - buf );
}


void InfoBytes(
    dw_client                   cli,
    const void *                buf,
    dw_size_t                   size )
{
    CLIWrite( DW_DEBUG_INFO, buf, size );
}


void InfoString(
    dw_client                   cli,
    const char *                str )
{
    size_t                      len;

    len = strlen( str );
    InfoBytes( cli, str, len + 1 );
}


debug_ref InfoSkip(
    dw_client                   cli,
    dw_size_t                   amt )
{
    debug_ref                   ret;

    ret = CLITell( DW_DEBUG_INFO ) - cli->section_base[ DW_DEBUG_INFO ];
    CLISeek( DW_DEBUG_INFO, amt, DW_SEEK_CUR );
    return( ret );
}


void InfoPatch(
    dw_client                   cli,
    debug_ref                   offs,
    const void *                value,
    size_t                      len )
{
    CLISeek( DW_DEBUG_INFO, offs + cli->section_base[ DW_DEBUG_INFO ],
        DW_SEEK_SET );
    CLIWrite( DW_DEBUG_INFO, value, len );
    CLISeek( DW_DEBUG_INFO, 0, DW_SEEK_END );
}


void InitDebugInfo(  dw_client                   cli )
{

    /* leave room for the length field */
    CLISeek( DW_DEBUG_INFO, sizeof( uint_32 ), DW_SEEK_CUR );
    Info16( cli, 2 );   /* dwarf version */
    /* abbrev start */
    if( cli->compiler_options & DW_CM_ABBREV_PRE ){
        CLIReloc4( DW_DEBUG_INFO, DW_W_EXT_REF, cli->abbrev_sym, 0 );
    }else{
        CLISeek( DW_DEBUG_ABBREV, 0, DW_SEEK_SET );
        CLIReloc3( DW_DEBUG_INFO, DW_W_SECTION_POS, DW_DEBUG_ABBREV );
        CLISeek( DW_DEBUG_ABBREV, 0, DW_SEEK_END );
    }
    Info8( cli, cli->offset_size );
}


void FiniDebugInfo(
    dw_client                   cli )
{
    char                        buf[ sizeof( uint_32 ) ];
    long                        size;

    /* patch in the length of the .debug_info section */
    size = CLITell( DW_DEBUG_INFO );
    size -= sizeof( uint_32 );
    size -= cli->section_base[ DW_DEBUG_INFO ];
    CLISeek( DW_DEBUG_INFO, cli->section_base[ DW_DEBUG_INFO ], DW_SEEK_SET );
    WriteU32( buf, size );
    CLIWrite( DW_DEBUG_INFO, buf, sizeof( uint_32 ) );
    CLISeek( DW_DEBUG_INFO, 0, DW_SEEK_END );
}
