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
* Description:  COFF dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>

#include "wdglb.h"
#include "wdfunc.h"


static  char    *coff_hdr_msg[] = {
    "2cpu type                                        = ",
    "2number of object entries                        = ",
    "4time/date stamp                                 = ",
    "4symbol table                                    = ",
    "4number of symbols                               = ",
    "2no. of bytes in nt header following flags field = ",
    "2flags                                           = ",
    NULL
};

/*
 * Load in the coff string table so we can put out
 * some decent names.
 */

static char *String_table;

static void load_string_table( coff_file_header *header )
/*******************************************************/
{
    unsigned_32 string_pos;
    unsigned_32 table_size;

    string_pos = Coff_off + header->sym_table
                        + header->num_symbols * sizeof( coff_symbol );
    Wlseek( string_pos );
    Wread( &table_size, sizeof( table_size ) );
    if( table_size > 4 ){
        table_size -= sizeof(unsigned_32);
        String_table = Wmalloc( table_size );
        Wread( String_table, table_size );
    } else {
        String_table = NULL;
    }
}

extern char *Coff_obj_name( char *id )
/************************************/
{
    unsigned_32 offset;

    if( String_table == NULL ) return( id );
    if( *id == '/' ) {
        offset = 0;
        id += 1;
        while( *id ) {
            offset = offset * 10 + ( *id - '0' );
            id += 1;
        }
        return( &String_table[ offset-4 ] );
    }
    return( id );
}

static void unload_string_table( void )
/*************************************/
{
    free( String_table );
}

static void dmp_symtab( unsigned long offset, unsigned long num_syms )
/********************************************************************/
{
    coff_symbol *symtab;
    coff_symbol *start;
    char *      strtab;
    unsigned_32 strsize;
    unsigned_32 symidx;
    unsigned    num_aux;
    char *      name;
    char        buff[9];

    if( num_syms == 0 ) {
        Wdputslc( "No symbols in object file\n" );
        Wdputslc( "\n" );
        return;
    }
    Wlseek( Coff_off + offset );
    Banner( "Symbol Table" );
    start = Wmalloc( num_syms * sizeof(coff_symbol) );
    symtab = start;
    Wread( start, num_syms * sizeof(coff_symbol) );
    Wread( &strsize, sizeof(unsigned_32) );
    if( strsize != 0 ) {
        strsize -= sizeof(unsigned_32);
    }
    if( strsize != 0 ) {
        strtab = Wmalloc( strsize );
        Wread( strtab, strsize );
    } else {
        strtab = NULL;
    }
    buff[8] = '\0';
    for( symidx = 0; symidx < num_syms; symidx++ ) {
        if( symtab->name.non_name.zeros == 0 ) {
            name = strtab + symtab->name.non_name.offset - 4;
        } else if( symtab->name.name_string[8] == '\0' ) {
            name = symtab->name.name_string;
        } else {
            memcpy( buff, symtab->name.name_string, 8 );
            name = buff;
        }
        Wdputs( "Idx: " );
        Puthex( symidx, 8 );
        Wdputs( " Name: " );
        Wdputs( name );
        Wdputslc( "\n" );
        Wdputs( "Value: " );
        Puthex( symtab->value, 8 );
        Wdputs( " Sec #: " );
        Puthex( symtab->sec_num, 4 );
        Wdputs( " Type: " );
        Puthex( symtab->type, 4 );
        Wdputs( " Class: " );
        Puthex( symtab->storage_class, 2 );
        Wdputs( " # Aux Syms: " );
        Putdec( symtab->num_aux );
        Wdputslc( "\n" );
        num_aux = symtab->num_aux;
        symtab++;
        if( num_aux > 0 ) {
            dmp_mult_data_line((char *)symtab, 0, num_aux * sizeof(coff_symbol));
            symtab += num_aux;
            symidx += num_aux;
        }
        Wdputslc( "\n" );
    }
    Wdputslc( "\n" );
    if( strsize != 0 ) {
        Banner( "String Table" );
        dmp_mult_data_line( strtab, 0, strsize );
        Wdputslc( "\n" );
    }
    free( start );
    free( strtab );
}

/*
 * Dump the coff object, if any.
 */
bool Dmp_coff_head( void )
/************************/
{
    coff_file_header    header;

    Wlseek( Coff_off );
    Wread( &header, sizeof( coff_file_header ) );
    if( header.cpu_type != IMAGE_FILE_MACHINE_I386
        && header.cpu_type != IMAGE_FILE_MACHINE_ALPHA
        && header.cpu_type != IMAGE_FILE_MACHINE_UNKNOWN
        && header.cpu_type != IMAGE_FILE_MACHINE_POWERPC ) {
        return 0;
    }
    Banner( "COFF object file" );
    Wdputs( "file offset = " );
    Puthex( Coff_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    Dump_header( (char *)&header, coff_hdr_msg );
    DumpCoffHdrFlags( header.flags );
    load_string_table( &header );
    Wlseek( Coff_off + sizeof(coff_file_header) + header.opt_hdr_size );
    dmp_objects( header.num_sections );
    unload_string_table();
    dmp_symtab( header.sym_table, header.num_symbols );
    return 1;
}


bool Dmp_ar_head( void )
/**********************/
{
    char                sig[AR_IDENT_LEN];
    ar_header           hdr;
    long                filesize;
    unsigned long       size;

    Wlseek( 0 );
    Wread( sig, AR_IDENT_LEN );
    if( memcmp( sig, AR_IDENT, AR_IDENT_LEN ) != 0 ) return 0;
    filesize = WFileSize();
    Coff_off = AR_IDENT_LEN;
    for(;;) {
        if( Coff_off + sizeof(ar_header) >= filesize ) break;
        Wread( &hdr, sizeof(ar_header) );
        Coff_off += sizeof(ar_header);
        hdr.date[0]='\0';
        Wdputs( "ar name = " );
        Wdputs( hdr.name );
        Wdputslc( "\n" );
        hdr.header_ident[0] = '\0';
        size = strtoul( hdr.size, NULL, 10 );
        if( strcmp( hdr.name, "/               " ) == 0 ||
                        strcmp( hdr.name, "//              " ) == 0 ) {
            Dmp_seg_data( Coff_off, size );
        } else if( !Dmp_coff_head() ) {
            // then try and see if it's ELF
            Wlseek( Coff_off );
            if( !Dmp_elf_header( Coff_off ) ) {
                Wdputslc( "archive entry not identified\n" );
                Dmp_seg_data( Coff_off, size );
                Wdputslc( "\n" );
            }
        }
        if( size & 1 ) {
            size++;
        }
        Coff_off += size;
        Wlseek( Coff_off );
    }
    return 1;
}
