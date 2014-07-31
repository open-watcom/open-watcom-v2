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
* Description:  Portable Executable tables dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "wdglb.h"
#include "wdfunc.h"


static  const_string_table pe_export_msg[] = {
    "4          export flags                      = ",
    "4          time/date stamp                   = ",
    "2          major version number              = ",
    "2          minor version number              = ",
    "4          rva of the Dll asciiz Name        = ",
    "4          first valid exported ordinal      = ",
    "4          no. of entries in Export Addr Tbl = ",
    "4          no. of entries in Name Ptr Tbl    = ",
    "4          rva of the Export Addr Tbl        = ",
    "4          rva of the Export Name Tbl Ptrs   = ",
    "4          rva of Export Ordinal Tbl Entry   = ",
    NULL
};

static  const_string_table pe_import_msg[] = {
    "4          rva of the start of import lookup tbl = ",
    "4          time/date stamp                       = ",
    "2          major version number                  = ",
    "2          minor version number                  = ",
    "4          rva of the Dll asciiz Name            = ",
    "4          rva of the start of import addresses  = ",
    NULL
};

extern char  Fname[ _MAX_FNAME ];

/*
 * Dump the Export address Table.
 */
static void dmp_exp_addr( unsigned_32 offset, unsigned_32 num_ent,
                          unsigned_32 base )
/****************************************************************/
{
    unsigned_32     *address;
    unsigned_32     addr_size;
    unsigned_32     i;

    Wlseek( offset );
    addr_size = num_ent * sizeof( unsigned_32 );
    address = Wmalloc( addr_size );
    Wread( address, addr_size );
    Wdputslc( "\n" );
    Wdputslc( "Export Address Table\n" );
    Wdputslc( "====================\n" );
    for( i = 0; i < num_ent; i++ ) {
        Putdecl( i + base, 4 );
        Wdputc( ':' );
        Puthex( address[i], 8 );
        if( (i+1) % 4 == 0 ) {
            Wdputslc( "\n" );
        } else {
            Wdputs( "     " );
        }
    }
}

/*
 * Dump the Export Name and Ordinal Tables.
 */
static void dmp_exp_ord_name( unsigned_32 nam_off, unsigned_32 ord_off,
                       unsigned_32 num_ptr, unsigned_32 base )
/*********************************************************************/
{
    unsigned_16     *ord_addr;
    unsigned_32     *nam_addr;
    unsigned_32     addr_size;
    unsigned_32     i;

    Wlseek( nam_off );
    addr_size = num_ptr * sizeof( unsigned_32 );
    nam_addr = Wmalloc( addr_size );
    Wread( nam_addr, addr_size );
    Wlseek( ord_off );
    addr_size = num_ptr * sizeof( unsigned_16 );
    ord_addr = Wmalloc( addr_size );
    Wread( ord_addr, addr_size );
    Wdputslc( "\n" );
    Wdputslc( "\n" );
    Wdputslc( "  ordinal     name ptr        name\n" );
    Wdputslc( "  =======     ========        ====\n" );
    for( i = 0; i < num_ptr; i++ ) {
        Putdecl( ord_addr[i] + base, 6 );
        Wdputs( "        " );
        Puthex( nam_addr[i], 8 );
        Wdputs( "        " );
        Dump_asciiz( nam_addr[i] - Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off );
        Wdputslc( "\n" );
    }
}

/*
 * Dump the Import address Table.
 */
static void dmp_imp_addr( unsigned_32 offset )
/********************************************/
{
    unsigned_32     address;
    unsigned_32     addr_size;
    int             i;

    Wlseek( offset );
    Wdputslc( "\n" );
    Wdputslc( "Import Address Table\n" );
    Wdputslc( "====================\n" );
    addr_size = sizeof( unsigned_32 );
    Wread( &address, addr_size );
    for( i = 0; address != 0; i++ ) {
        if( i != 0 ) {
            if( (i) % 4 == 0 ) {
                Wdputslc( "\n" );
            } else {
                Wdputs( "     " );
            }
        }
        Putdecl( i, 4 );
        Wdputc( ':' );
        Puthex( address, 8 );
        Wread( &address, addr_size );
    }
    Wdputslc( "\n" );
    Wdputslc( "\n" );
}

/*
 * Dump the Import lookup Table.
 */
static void dmp_imp_lookup( unsigned_32 offset )
/**********************************************/
{
    unsigned_32             address;
    unsigned_32             addr_size;
    int                     i;
    unsigned_16             hint;

    Wlseek( offset );
    Wdputslc( "\n" );
    Wdputslc( "Import Lookup Table\n" );
    Wdputslc( "===================\n" );
    Wdputslc( "       import       hint       name/ordinal\n" );
    Wdputslc( "       ======       ====       ============\n" );

    addr_size = sizeof( unsigned_32 );
    Wread( &address, addr_size );
    for( i = 0; address != 0; ++i ) {
        Wdputs( "       " );
        Puthex( address, 8 );
        if( address & PE_IMPORT_BY_ORDINAL ) {
            Wdputs( "          " );
	    Putdecl( address & ~PE_IMPORT_BY_ORDINAL, 8 );
        } else {
            Wlseek( address - Pe_head.table[ PE_TBL_IMPORT ].rva + Imp_off );
            Wread( &hint, sizeof( hint ) );
            Putdecl( hint, 8 );
            Wdputs( "        " );
            Dump_asciiz( address - Pe_head.table[ PE_TBL_IMPORT ].rva + Imp_off + sizeof( hint ) );
        }
        Wdputslc( "\n" );
        offset += sizeof( unsigned_32 );
        Wlseek( offset );
        Wread( &address, addr_size );
    }
}

/*
 * Dump the Export Name and Ordinal Tables.
 */
static void dmp_ord_name( unsigned_32 nam_off, unsigned_32 ord_off,
                   unsigned_32 num_ptr, unsigned_32 base )
/*****************************************************************/
{
    unsigned_16     *ord_addr;
    unsigned_32     *nam_addr;
    unsigned_32     addr_size;
    int             i;

    Wlseek( nam_off );
    addr_size = num_ptr * sizeof( unsigned_32 );
    nam_addr = Wmalloc( addr_size );
    Wread( nam_addr, addr_size );
    Wlseek( ord_off );
    addr_size = num_ptr * sizeof( unsigned_16 );
    ord_addr = Wmalloc( addr_size );
    Wread( ord_addr, addr_size );
    Wdputslc( "\n" );
    for( i = 0; i < num_ptr; i++ ) {
        Dump_asciiz( nam_addr[i] - Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off );
        Wdputc( '.' );
        Wdputc( '\'' );
        Wdputs( Fname );
        Wdputs( ".DLL\'." );
        Putdec( ord_addr[i] + base );
        Wdputslc( "\n" );
    }
}

/*
 * Dump the Export Table.
 */
void Dmp_exp_tab( void )
/**********************/
{
    pe_export_directory     pe_export;

    strupr( Fname );
    Wlseek( Exp_off );
    Wread( &pe_export, sizeof( pe_export_directory ) );
    dmp_ord_name( pe_export.name_ptr_table_rva -
            Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off,
            pe_export.ordinal_table_rva -
            Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off,
            pe_export.num_name_ptrs, pe_export.ordinal_base );
}

/*
 * Dump the Export Table.
 */
void Dmp_exports( void )
/**********************/
{
    pe_export_directory     pe_export;

    Wlseek( Exp_off );
    Wread( &pe_export, sizeof( pe_export_directory ) );
    Banner( "Export Directory Table" );
    Dump_header( (char *)&pe_export.flags, pe_export_msg );
    dmp_exp_addr( pe_export.address_table_rva -
            Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off,
            pe_export.num_eat_entries, pe_export.ordinal_base );
    dmp_exp_ord_name( pe_export.name_ptr_table_rva -
            Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off,
            pe_export.ordinal_table_rva -
            Pe_head.table[ PE_TBL_EXPORT ].rva + Exp_off,
            pe_export.num_name_ptrs, pe_export.ordinal_base );
    Wdputslc( "\n" );
}

/*
 * Dump the Import Table.
 */
void Dmp_imports( void )
/**********************/
{
    pe_import_directory     pe_import;
    unsigned_32             offset;

    offset = Imp_off;
    for( ;; ) {
        Wlseek( offset );
        Wread( &pe_import, sizeof( pe_import_directory ) );
        if( pe_import.import_lookup_table_rva == 0 ) break;
        Banner( "Import Directory Table" );
        Dump_header( (char *)&pe_import.import_lookup_table_rva, pe_import_msg );
        Wdputs( "          DLL name = <" );
        Dump_asciiz( pe_import.name_rva - Pe_head.table[ PE_TBL_IMPORT ].rva + Imp_off );
        Wdputslc( ">\n" );
        dmp_imp_lookup( pe_import.import_lookup_table_rva -
                Pe_head.table[ PE_TBL_IMPORT ].rva + Imp_off );
        dmp_imp_addr( pe_import.import_address_table_rva -
                Pe_head.table[ PE_TBL_IMPORT ].rva + Imp_off );
        offset += sizeof( pe_import_directory );
    }
}
