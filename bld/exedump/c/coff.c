/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


static  const_string_table coff_hdr_cpu32[] = {
    "2cpu type (32-bit)                               = ",
    NULL
};
static  const_string_table coff_hdr_cpu64[] = {
    "2cpu type (64-bit)                               = ",
    NULL
};
static  const_string_table coff_hdr_msg[] = {
    "2number of object entries                        = ",
    "4time/date stamp                                 = ",
    "4symbol table                                    = ",
    "4number of symbols                               = ",
    "2no. of bytes in nt header following flags field = ",
    "2flags                                           = ",
    NULL
};

static  const_string_table coff_imp_hdr_msg1[] = {
    "2version                                         = ",
    NULL
};
static  const_string_table coff_imp_hdr_msg2[] = {
    "4time/date stamp                                 = ",
    "4size of data                                    = ",
    "2ordinal/hint                                    = ",
    "2type                                            = ",
    NULL
};

static  const_string_table coff_imp_symbol[] = {
    "SSymbol name: ",
    "SDLL:         ",
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

const char *Coff_obj_name( const char *id )
/*****************************************/
{
    unsigned_32 offset;

    if( String_table == NULL )
        return( id );
    if( *id == '/' ) {
        offset = 0;
        for( id += 1; *id != '\0'; id++ ) {
            offset = offset * 10 + ( *id - '0' );
        }
        return( &String_table[offset - 4] );
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
    coff_symbol     *symtab;
    coff_symbol     *start;
    char            *strtab;
    unsigned_32     strsize;
    unsigned_32     symidx;
    unsigned        num_aux;
    char            *name;
    char            buff[9];
    int             i;

    if( num_syms == 0 ) {
        Wdputslc( "No symbols in object file\n" );
        Wdputslc( "\n" );
        return;
    }
    Wlseek( Coff_off + offset );
    Banner( "Symbol Table" );
    start = Wmalloc( num_syms * sizeof( coff_symbol ) );
    symtab = start;
    Wread( start, num_syms * sizeof( coff_symbol ) );
    Wread( &strsize, sizeof( unsigned_32 ) );
    if( strsize != 0 ) {
        strsize -= sizeof( unsigned_32 );
    }
    if( strsize != 0 ) {
        strtab = Wmalloc( strsize );
        Wread( strtab, strsize );
    } else {
        strtab = NULL;
    }
    buff[8] = '\0';
    Wdputs( "Idx  Value    Sec  Type Class # Aux Name\n" );
    for( symidx = 0; symidx < num_syms; symidx++ ) {
        if( symtab->name.non_name.zeros == 0 ) {
            name = strtab + symtab->name.non_name.offset - 4;
        } else if( symtab->name.name_string[8] == '\0' ) {
            name = symtab->name.name_string;
        } else {
            memcpy( buff, symtab->name.name_string, 8 );
            name = buff;
        }
        Puthex( symidx + 1, 4 );
        Wdputs( " " );
        Puthex( symtab->value, 8 );
        Wdputs( " " );
        Puthex( symtab->sec_num, 4 );
        Wdputs( " " );
        Puthex( symtab->type, 4 );
        Wdputs( " " );
        Puthex( symtab->storage_class, 2 );
        Wdputs( "    " );
        Putdec( symtab->num_aux );
        Wdputs( "     " );
        Wdputslc( name );
        num_aux = symtab->num_aux;
        symtab++;
        Wdputslc( "\n" );
        for( i = 0; i < num_aux; i++ ) {
            Wdputs( "aux" );
            dmp_data_line_bytes( (char *)symtab, (unsigned_16)sizeof( coff_symbol ) );
            symtab++;
            symidx++;
        }
    }
    Wdputslc( "\n" );
    if( strsize != 0 ) {
        Banner( "String Table" );
        dmp_mult_data_line( strtab, 0, (unsigned_16)strsize );
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
    union {
        coff_file_header            o;
        coff_import_object_header   i;
    }           header;
    bool        impfile;
    char        *p;
    const char  * const *templ;

    Wlseek( Coff_off );
    Wread( &header, sizeof( header ) );
    impfile = ( header.i.sig1 == COFF_IMPORT_OBJECT_HDR_SIG1 && header.i.sig2 == COFF_IMPORT_OBJECT_HDR_SIG2 );
    if( !impfile
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_I386
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_ALPHA
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_R3000
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_R4000
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_UNKNOWN
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_POWERPC
      && header.o.cpu_type != COFF_IMAGE_FILE_MACHINE_AMD64
        ) {
        return( false );
    }
    if( impfile ) {
        p = "COFF import file";
    } else {
        p = "COFF object file";
    }
    Banner( p );
    Wdputs( "file offset = " );
    Puthex( Coff_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    if( impfile ) {
        switch( header.i.machine ) {
        case COFF_IMAGE_FILE_MACHINE_AMD64:
            templ = coff_hdr_cpu64;
            break;
        default:
            templ = coff_hdr_cpu32;
            break;
        }
        Dump_header( (char *)&header + 4, coff_imp_hdr_msg1, 4 );
        Dump_header( (char *)&header + 6, templ, 4 );
        Dump_header( (char *)&header + 8, coff_imp_hdr_msg2, 4 );
        Wdputslc( "\n" );
        p = Wmalloc( header.i.size_of_data );
        Wread( p, header.i.size_of_data );
        Dump_header( p, coff_imp_symbol, 80 );
        Wdputslc( "\n" );
    } else {
        switch( header.o.cpu_type ) {
        case COFF_IMAGE_FILE_MACHINE_AMD64:
            templ = coff_hdr_cpu64;
            break;
        default:
            templ = coff_hdr_cpu32;
            break;
        }
        Dump_header( (char *)&header, templ, 4 );
        Dump_header( (char *)&header + 2, coff_hdr_msg, 4 );
        DumpCoffHdrFlags( header.o.flags );
        load_string_table( &header.o );
        Wlseek( Coff_off + sizeof(coff_file_header) + header.o.opt_hdr_size );
        dmp_objects( header.o.num_sections );
        unload_string_table();
        dmp_symtab( header.o.sym_table, header.o.num_symbols );
    }
    return( true );
}


bool Dmp_ar_head( void )
/**********************/
{
    char                sig[AR_IDENT_LEN];
    ar_header           hdr;
    unsigned long       filesize;
    unsigned long       size;

    Wlseek( 0 );
    Wread( sig, AR_IDENT_LEN );
    if( memcmp( sig, AR_IDENT, AR_IDENT_LEN ) != 0 )
        return( false );
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
    return( true );
}
