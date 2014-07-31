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
* Description:  PE resources dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>

#include "wdglb.h"
#include "wdfunc.h"


static  const_string_table pe_resource_msg[] = {
        "4          resource flags               = ",
        "4          time/date stamp              = ",
        "2          major version number         = ",
        "2          minor version number         = ",
        "2          number of name entries       = ",
        "2          number of ID integer entries = ",
        NULL
};

static  const_string_table pe_fixup_msg[] = {
        "4          page rva                       = ",
        "4          number of bytes in fixup block = ",
        NULL
};

/*
 * Dump the type and offsets.
 */
static void dmp_type_offset( unsigned_32 bsize )
/**********************************************/
{
    unsigned_32             i;
    unsigned                prev;
    pe_fixup_entry          entry;

    Wdputslc( "type:offset               type --\n" );
    Wdputslc( "0 = ABSOLUTE, 1 = LOW, 2 = HIGH, 3 = HIGHLOW, 4 = HIGHADJUST, 5 = MIPSJMPADDR\n" );
    prev = 0;
    for( i = 0; i < bsize; i++ ) {
        Wread( &entry, sizeof( pe_fixup_entry ) );
        if( i != 0 ) {
            if( (i) % 8 == 0 ) {
                Wdputslc( "\n" );
            } else {
                Wdputs( "     " );
            }
        }
        if( prev == 4 ) {
            Wdputc( ' ' );
            Puthex( entry, 4 );
            prev = 0;
        } else {
            prev = entry >> 12;
            Puthex( prev, 1 );
            Wdputc( ':' );
            Puthex( entry, 3 );
        }
    }
    Wdputslc( "\n" );
}

/*
 * Dump the Fixup Table.
 */
void Dmp_fixups( void )
/*********************/
{
    pe_fixup_header         pe_fixup;
    unsigned_32             offset;

    offset = Fix_off;
    Wdputslc( "\n" );
    Banner( "Fixup Block Tables" );
    for( ;; ) {
        Wlseek( offset );
        Wread( &pe_fixup, sizeof( pe_fixup_header ) );
        offset += pe_fixup.block_size;
        if( pe_fixup.block_size == 0 ) break;
        Dump_header( (char *)&pe_fixup.page_rva, pe_fixup_msg );
        if( Options_dmp & FIX_DMP ) {
            dmp_type_offset( ( pe_fixup.block_size -
                            sizeof( pe_fixup_header ) ) / 2 );
        }
        Wdputslc( "\n" );
    }
}

/*
 * Get the Resource Name.
 */
static res_name get_name( unsigned_32 offset )
/********************************************/
{
    unsigned_16             i;
    char                    name[2*SLEN];
    unsigned_16             uniname[SLEN];
    res_name                lname;

    Wlseek( offset );
    Wread( &lname.len, sizeof( unsigned_16 ) );
    if( lname.len > SLEN ) {
        lname.len = SLEN;
    }
    Wread( uniname, lname.len*2 );
    for( i = 0; i < lname.len; i++ ) {
        name[i] = uniname[i];
    }
    lname.rname = name;
    lname.rname[lname.len] = 0;
    return( lname );
}

/*
 * Dump the Resource Names.
 */
static void dmp_name_id( unsigned_32 offset, bool is_name )
/*********************************************************/
{
    unsigned_16             i, j, cnt;
    resource_dir_entry      pe_res_dir, tmp_dir;
    resource_dir_header     pe_res_lang;
    res_name                name;

    Wlseek( offset );
    Wread( &pe_res_dir, sizeof( resource_dir_entry ) );
    offset = Res_off +( pe_res_dir.id_name & PE_RESOURCE_MASK );
    if( is_name ) {
        name = get_name( offset );
        Wdputs( name.rname );
        for( j = name.len; j <= SLEN; j++ ) {
            Wdputc( ' ' );
        }
    } else {
        Puthex( pe_res_dir.id_name, 8 );
        Wdputs( "                       " );
    }
    offset = Res_off +( pe_res_dir.entry_rva & PE_RESOURCE_MASK );
    if( pe_res_dir.entry_rva & PE_RESOURCE_MASK_ON ) {
        Wlseek( offset );
        Wread( &pe_res_lang, sizeof( resource_dir_header ) );
        offset += sizeof( resource_dir_header );
        Wlseek( offset );
        cnt = pe_res_lang.num_id_entries;
        for( i = 0; i < cnt; i++ ) {
            Wread( &tmp_dir, sizeof( resource_dir_entry ) );
            Puthex( tmp_dir.id_name, 8 );
            if( i != cnt - 1 ) {
                Wdputslc( "\n                                                             " );
            }
        }
        offset = Res_off +( tmp_dir.entry_rva & PE_RESOURCE_MASK );
    }
    if( Data_off == 0 ) {
        Data_off = offset;
    }
}

/*
 * Dump the Resource Id's.
 */
static void dmp_res_name_id( unsigned_16 count, bool is_name )
/***********************************************************/
{
    unsigned_16             i, j, cnt;
    unsigned_32             offset, tmp_off;
    resource_dir_header     pe_res_name;
    resource_dir_entry      pe_res_dir;
    res_name                name;

    if( count == 0 ) return;
    offset = Res_off + sizeof( resource_dir_header );
    for( i = 0; i < count; i++ ) {
        Wlseek( offset );
        Wread( &pe_res_dir, sizeof( resource_dir_entry ) );
        offset += sizeof( resource_dir_entry );
        tmp_off = Res_off +( pe_res_dir.id_name & PE_RESOURCE_MASK );
        if( is_name ) {
            name = get_name( tmp_off );
        } else {
            name.len = 0;
            name.rname = NULL;
        }
        tmp_off = Res_off +( pe_res_dir.entry_rva & PE_RESOURCE_MASK );
        Wlseek( tmp_off );
        Wread( &pe_res_name, sizeof( resource_dir_header ) );
        tmp_off += sizeof( resource_dir_header );
        cnt = pe_res_name.num_name_entries + pe_res_name.num_id_entries;
        for( j = 0; j < cnt; j++ ) {
            if( is_name ) {
                Wdputs( name.rname );
                for( j = name.len; j <= SLEN; j++ ) {
                    Wdputc( ' ' );
                }
            } else {
                Puthex( pe_res_dir.id_name, 8 );
                Wdputs( "                       " );
            }
            if( j >= pe_res_name.num_name_entries ) {
                dmp_name_id( tmp_off, FALSE );
            } else {
                dmp_name_id( tmp_off, TRUE );
            }
            tmp_off += sizeof( resource_dir_entry );
            Wdputslc( "\n" );
        }
    }
}

/*
 * Dump the Resource Directory.
 */
static void dmp_res_dir( void )
/*****************************/
{
    resource_dir_header     res_head;
    resource_dir_entry      res_dir;
    unsigned_16             count, i;

    Wdputslc( "\n" );
    Wdputslc( "80000000H = name rva,        else integer id\n" );
    Wdputslc( "80000000H = sudirectory rva, else data entry rva\n" );
    Wdputslc( "  #id ents    #name ents    name rva/id    data/subdir rva\n" );
    Wdputslc( "    ====         ====         ========        ========\n" );
    Wlseek( Res_off );
    for( ;; ) {
        Wread( &res_head, sizeof( resource_dir_header ) );
        count = res_head.num_name_entries + res_head.num_id_entries;
        if( count == 0 ) break;
        Wdputs( "    " );
        Puthex( res_head.num_id_entries, 4 );
        Wdputs( "         " );
        Puthex( res_head.num_name_entries, 4 );
        Wdputs( "         " );
        for( i = 0; i < count; i++ ) {
            Wread( &res_dir, sizeof( resource_dir_entry ) );
            if( i != 0 ) {
                Wdputs( "                              " );
            }
            Puthex( res_dir.id_name, 8 );
            Wdputs( "        " );
            Puthex( res_dir.entry_rva, 8 );
            Wdputslc( "\n" );
        }
    }
}

/*
 * Dump the Resource Data.
 */
static void dmp_res_data( void )
/******************************/
{
    resource_entry          res_data;
    unsigned_16             i;

    Wdputslc( "\n" );
    Wdputslc( "       data rva     data size    codepage     reserved\n" );
    Wdputslc( "       ========     ========     ========     ========\n" );
    Wlseek( Data_off );
    for( i = 0; ; i++ ) {
        Wread( &res_data, sizeof( resource_entry ) );
        if( res_data.rsvd != 0 ) break;
        Putdecl( i, 3 );
        Wdputs( ":   " );
        Puthex( res_data.data_rva, 8 );
        Wdputs( "     " );
        Puthex( res_data.size, 8 );
        Wdputs( "     " );
        Puthex( res_data.code_page, 8 );
        Wdputs( "     " );
        Puthex( res_data.rsvd, 8 );
        Wdputslc( "\n" );
    }
}


/*
 * Dump the Resource Table.
 */
void Dmp_resources( void )
/************************/
{
    resource_dir_header             pe_res_type;

    Data_off = 0;
    Wlseek( Res_off );
    Wread( &pe_res_type, sizeof( resource_dir_header ) );
    Banner( "Resource Directory Table" );
    Dump_header( (char *)&pe_res_type.flags, pe_resource_msg );
    Wdputslc( "\n" );
    Wdputslc( "type id/string                 name id/string                 language id\n" );
    Wdputslc( "==============                 ==============                 ===========\n" );
    dmp_res_name_id( pe_res_type.num_name_entries, TRUE );
    dmp_res_name_id( pe_res_type.num_id_entries, FALSE );
    if( Options_dmp & RESRC_DMP ) {
        dmp_res_dir();
        dmp_res_data();
    }
}
