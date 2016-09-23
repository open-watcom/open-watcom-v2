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
* Description:  DWARF writer library test client.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>

#include "watcom.h"
#include "dw.h"
#include "dwarf.h"
#include "testcli.h"


dw_client       Client;

struct section_data Sections[DW_DEBUG_MAX];

uint_32         RelocValues[DW_W_MAX];
uint_32         SymHandles[20];


void CLIWrite( dw_sectnum sect, const void *block, size_t size )
{
    memcpy( &Sections[sect].data[Sections[sect].cur_offset], block, size );
    Sections[sect].cur_offset += size;
    if( Sections[sect].max_offset < Sections[sect].cur_offset ) {
        Sections[sect].max_offset = Sections[sect].cur_offset;
    }
}


void CLIReloc( dw_sectnum sect, dw_relocs reloc_type, ... )
{
    static char                 zeros[] = { 0, 0 };
    dw_sym_handle               sym;
    uint                        section;
    va_list                     args;

    va_start( args, reloc_type );
    switch( reloc_type ) {
    case DW_W_LOW_PC:
    case DW_W_HIGH_PC:
    case DW_W_LABEL:
    case DW_W_DEFAULT_FUNCTION:
    case DW_W_ARANGE_ADDR:
        CLIWrite( sect, &RelocValues[reloc_type], sizeof( uint_32 ) );
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    case DW_W_UNIT_SIZE:
        CLIWrite( sect, &RelocValues[reloc_type], sizeof( uint_32 ) );
        break;
    case DW_W_SECTION_POS:
        section = va_arg( args, uint );
        CLIWrite( sect, &Sections[section].cur_offset, sizeof( uint_32 ) );
        break;
    case DW_W_STATIC:
        sym = va_arg( args, dw_sym_handle );
        CLIWrite( sect, &SymHandles[(uint_32)sym], sizeof( uint_32 ) );
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    case DW_W_SEGMENT:
        sym = va_arg( args, dw_sym_handle );
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    default:
        va_end( args );
        abort();
        break;
    }
    va_end( args );
}


void CLISeek( dw_sectnum sect, long offs, uint type ) {

    switch( type ) {
    case DW_SEEK_CUR:
        Sections[sect].cur_offset += offs;
        break;
    case DW_SEEK_SET:
        Sections[sect].cur_offset = offs;
        break;
    case DW_SEEK_END:
        Sections[sect].cur_offset = Sections[sect].max_offset - offs;
        break;
    }
}

long CLITell( dw_sectnum sect ) {

    return( Sections[sect].cur_offset );
}


void *CLIAlloc( size_t size ) {

    void        *p;

    p = malloc( size );
    if( p == NULL && size != NULL ) {
        fputs( "out of memory!\n", stderr );
        exit( 1 );
    }
    return( p );
}


void CLIFree( void *p ) {

    free( p );
}


void main( void )
/***************/
{
    dw_init_info        info;
    dw_loc_handle       seg;
    dw_cu_info          cuinfo;
    static DWSetRtns( dw_cli_funcs, CLIReloc, CLIWrite, CLISeek, CLITell, CLIAlloc, CLIFree );

    info.language = DW_LANG_C89;
    info.compiler_options = DW_CM_BROWSER;
    info.producer_name = "testcli";
    if( setjmp( info.exception_handler ) == 0 ) {
        info.funcs = dw_cli_funcs;

        RelocValues[DW_W_LOW_PC] = 0x135;
        RelocValues[DW_W_HIGH_PC] = 0x34561ul;
        RelocValues[DW_W_UNIT_SIZE] = 0x34561ul - 0x135;
        Client = DWInit( &info );
        if( Client == NULL ) {
            fputs( "error in DWInit()\n", stderr );
            exit( 1 );
        }
        seg = DWLocFini( Client, DWLocInit( Client ) );
        memset( &cuinfo, 0, sizeof( cuinfo ));
        cuinfo.source_filename = "foo.bar";
        cuinfo.directory = "somewhere\\over\\the\\rainbow";

        DWBeginCompileUnit( Client, &cuinfo );
        Test();
        DWEndCompileUnit( Client );
        cuinfo.source_filename = "empty.unit";
        cuinfo.directory = "in\\a\\land\\far\\far\\away";
        DWBeginCompileUnit( Client, &cuinfo );
        DWEndCompileUnit( Client );
        DWLocTrash( Client, seg );
        DWFini( Client );
        DumpSections();
    } else {
        fputs( "fatal error %d in dwarf library\n", stderr );
        exit( 1 );
    }
    exit( 0 );
}
