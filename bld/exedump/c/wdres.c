/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "wdglb.h"
#include "wdfunc.h"


typedef struct os2_res_entry {
    unsigned_16         type_id;
    unsigned_16         name_id;
} os2_res_entry;

static  const_string_table resource_type[][2] = {
    { "Unknown resource type\n",        "000" },
    { "Cursor\n",                       "cur" },
    { "Bitmap\n",                       "bmp" },
    { "Icon\n",                         "ico" },
    { "Menu template\n",                "mnu" },
    { "Dialog-box template\n",          "dlg" },
    { "String table\n",                 "str" },
    { "Font directory\n",               "fdr" },
    { "Font\n",                         "fnt" },
    { "Keyboard-accelerator table\n",   "acc" },
    { "RC data resource\n",             "rcd" },
    { "Error message table\n",          "erm" },
    { "Cursor group header\n",          "cgr" },
    { "Unkown resource type\n",         "unk" },    /* #13 is not used - unlucky? */
    { "Icon group header\n",            "igr" },
    { "Nametable\n"                     "nmt" }
};

static  const_string_table resource_type_os2[][2] = {
    { "Unknown resource type\n",                "000" },
    { "Pointer\n",                              "ptr" },
    { "Bitmap\n",                               "bmp" },
    { "Menu template\n",                        "mnu" },
    { "Dialog template\n",                      "dlg" },
    { "String table\n",                         "str" },
    { "Font directory\n",                       "fdr" },
    { "Font\n",                                 "fnt" },
    { "Accelerator table\n",                    "acc" },
    { "RC data resource\n",                     "rcd" },
    { "Error message table\n",                  "erm" },
    { "Dialog include file name\n",             "dif" },
    { "Key to virtual key translation table\n", "kvt" },
    { "Key to UGL translation table\n",         "kut" },
    { "Glyph to character translation table\n", "gct" },
    { "Screen display information\n",           "sdi" },
    { "Function key area (short form)\n",       "fks" },
    { "Function key area (long form)\n",        "fkl" },
    { "Help table\n",                           "hlt" },
    { "Help subtable\n",                        "hls" },
    { "DBCS font driver directory\n",           "fdd" },
    { "DBCS font driver\n",                     "fdr" },
    { "Default icon\n"                          "icd" }
};


/*
 * copy out resource to a file
 */
static void resrc_to_file( uint32_t res_off, uint32_t res_len, uint16_t res_id, char *ext )
{
    char        out_name[16];
    unsigned_8  buf[512];
    int         ofh;
    int         nr;

    /* form the output file name */
    sprintf( out_name, "%d", res_id );
    strcat( out_name, "." );
    strcat( out_name, ext );

    Wdputs( "Dumping resource to file: " );
    Wdputs( out_name );
    Wdputslc( "\n" );
    ofh = open( out_name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, PMODE_USR_RW );
    if( ofh < 0 ) {
        Wdputslc( "Error! Failed to create file\n" );
        longjmp( Se_env, 1 );
    }

    /* copy over the raw data */
    Wlseek( res_off );
    do {
        if( res_len >= sizeof( buf ) ) {
            nr = sizeof( buf );
        } else {
            nr = res_len;
        }
        Wread( buf, nr );
        write( ofh, buf, nr );
        res_len -= nr;
    } while( res_len );

    close( ofh );
}

/*
 * get a resource type name
 */
static void dmp_resrc_name( unsigned_16 offset )
/**********************************************/
{
    Wlseek( New_exe_off + Os2_head.resource_off + offset );
    Dump_name();
}

/*
 * printout a resource name
 */
static void dmp_resrc_type( unsigned_16 res_type )
/***********************************************/
{
    Wdputc( ' ' );
    if( res_type & SEG_RESRC_HIGH ) {
        res_type &= ~SEG_RESRC_HIGH;
        Wdputs( "resource id: " );
        Putdec( res_type );
        Wdputslc( "\n" );
    } else {
        dmp_resrc_name( res_type );
        Wdputslc( "\n" );
    }
}

/*
 * Dump a Resource flags Word
 */
static void dmp_resrc_flags( unsigned_16 flag )
/*********************************************/
{
    if( flags & SEG_MOVABLE ) {
        Wdputs( "MOVABLE" );
    } else {
        Wdputs( "FIXED" );
    }
    if( flags & SEG_PURE ) {
        Wdputs( "|PURE" );
    } else {
        Wdputs( "|IMPURE" );
    }
    if( flags & SEG_PRELOAD ) {
        Wdputs( "|PRELOAD" );
    } else {
        Wdputs( "|LOADONCALL" );
    }
    Wdputs( " Prior " );
    Putdec( GET_SEG_DISCARD_PRIORITY( flags ) );
}

/*
 * dump a resource description
 */
static void dmp_resrc_desc( resource_record *res_ent, unsigned_16 res_type )
/**************************************************************************/
{
    unsigned_32         res_off;
    unsigned_32         res_len;
    unsigned_32         res_end;

    dmp_resrc_type( res_ent->name );
    Wdputs( " file offset " );
    res_off = (unsigned_32)res_ent->offset << Resrc_shift_cnt;
    Puthex( res_off, 8 );
    Wdputs( "H len " );
    res_len = (unsigned_32)res_ent->length << Resrc_shift_cnt;
    Puthex( res_len, 8 );
    Wdputs( " flags " );
    dmp_resrc_flags( res_ent->flags );
    Wdputslc( "\n" );
    if( Options_dmp & RESRC_DMP ) {
        Wdputslc( "    data =\n" );
        Dmp_seg_data( res_off, res_len );
    }
    if( Options_dmp & RSRC_FILE_DMP ) {
        char    ext[8];

        if( res_type < ARRAY_SIZE( resource_type ) ) {
            strcpy( ext, resource_type[res_type][1] );
        } else {
            sprintf( ext, "%X", res_type );
        }
        resrc_to_file( res_off, res_len, res_ent->name, ext );
    }
    res_end = res_off + res_len;
    if( res_end > Resrc_end ) {
        Resrc_end = res_end;
    }
}

/*
 * dump some resource entries
 */
static void dmp_resrc_ent( unsigned_16 num_resources, unsigned_16 res_type )
/**************************************************************************/
{
    resource_record *res_ent_tab;
    resource_record *res_ent;
    unsigned_16     res_group_size;
    unsigned_16     res_num;

    if( num_resources == 0 ) {
        return;
    }
    res_group_size = num_resources * sizeof( resource_record );
    res_ent = res_ent_tab = Wmalloc( res_group_size );
    Wread( res_ent_tab, res_group_size );
    for( res_num = 0; res_num != num_resources; res_num++ ) {
        Wdputs( " # " );
        Putdec( res_num + 1 );
        dmp_resrc_desc( res_ent++, res_type );
    }
    free( res_ent_tab );
}

/*
 * printout a resource type name
 */
static void dmp_resrc_type_name( unsigned_16 res_type )
/*****************************************************/
{
    Wdputc( ' ' );
    if( res_type & SEG_RESRC_HIGH ) {
        res_type &= ~SEG_RESRC_HIGH;
        if( res_type > ARRAY_SIZE( resource_type ) ) {
//        if( res_type > 15 ) {
            Wdputs( "Type number: " );
            Putdec( res_type );
            Wdputslc( "\n" );
        } else {
            Wdputslc( resource_type[res_type][0] );
//            Wdputslc( resource_type[res_type] );
        }
    } else {
        dmp_resrc_name( res_type );
        Wdputslc( "\n" );
    }
}

/*
 * Dump The Resource Table for Windows NE module
 */
static void dmp_resrc_tab_win( void )
/***********************************/
{
    unsigned_16             res_type;
    resource_type_record    res_group;
    unsigned_32             offset;

    Resrc_end = 0ul;
    Wread( &Resrc_shift_cnt, sizeof( unsigned_16 ) );
    offset = New_exe_off + Os2_head.resource_off + sizeof( unsigned_16 );
    Wdputs( "resource shift count: " );
    Putdec( Resrc_shift_cnt );
    Wdputslc( "\n" );
    for( ;; ) {
        Wlseek( offset );
        Wread( &res_group, sizeof( resource_type_record ) );
        offset += sizeof( resource_type_record );
        res_type = res_group.type;
        if( res_type == 0 ) {
            return;
        }
        dmp_resrc_type_name( res_type );
        Wlseek( offset );
        dmp_resrc_ent( res_group.num_resources, res_type );
        offset += res_group.num_resources * sizeof( resource_record );
        Wdputslc( "\n" );
    }
}


/*
 * Dump The Resource Table for OS/2 NE module
 */
static void dmp_resrc_tab_os2( void )
/***********************************/
{
    unsigned_16     i;
    unsigned_16     type_id;
    unsigned_16     seg_no;
    unsigned_16     res_group_size;
    os2_res_entry   *res_tab;

    res_group_size = Os2_head.resource * sizeof( os2_res_entry );
    res_tab = Wmalloc( res_group_size );
    Wread( res_tab, res_group_size );

    type_id = 30;              /* if type_id > 22 a name won't be printed out */
    Wdputslc( "    seg#   type id   name id\n" );
    Wdputslc( "    ====   =======   =======\n" );
    for( i = 0; i < Os2_head.resource; i++, res_tab++ ) {
        if( (res_tab->type_id != type_id) || (Options_dmp & RESRC_DMP) ) {
            type_id = res_tab->type_id;
            if( type_id < ARRAY_SIZE( resource_type_os2 ) ) {
                Wdputs( "type:  " );
                Wdputslc( resource_type_os2[res_tab->type_id][0] );
            }
        }
        Wdputs( "    " );
        seg_no = i + Os2_head.segments - Os2_head.resource + 1;
        Puthex( seg_no, 4 );
        Wdputs( "   " );
        Puthex( res_tab->type_id, 4 );
        Wdputs( "      " );
        Puthex( res_tab->name_id, 4 );
        Wdputs( "      " );
        Wdputslc( "\n" );
        if( Options_dmp & RESRC_DMP ) {
            Segspec = seg_no;
            Wdputslc( "    data =\n" );
            Dmp_one_seg_data( seg_no );
        }
        if( Options_dmp & RSRC_FILE_DMP ) {
            char            ext[8];
            segment_record  *seg;

            if( res_tab->type_id < ARRAY_SIZE( resource_type_os2 ) ) {
                strcpy( ext, resource_type_os2[res_tab->type_id][1] );
            } else {
                sprintf( ext, "%X", res_tab->type_id );
            }
            if( seg_no > Os2_head.segments ) {
                Wdputslc( "resource segment not found\n" );
            } else {
                unsigned_32     res_off;
                unsigned_32     res_len;

                /* This replicates logic in wdata.c, dmp_segment() */
                seg = &Int_seg_tab[seg_no - 1];

                if( seg->address == 0 ) {
                    Wdputslc( "resource segment addr zero\n" );
                } else {
                    if( seg->size == 0 ) {
                        res_len = 0x00010000;
                    } else {
                        res_len = seg->size;
                    }
                    res_off = (unsigned_32)seg->address << Os2_head.align;

                    if( seg->info & SEG_ITERATED ) {
                        Wdputslc( "iterated segments NYI!" );
                    } else {
                        resrc_to_file( res_off, res_len, res_tab->name_id, ext );
                    }
                }
            }
        }
    }
    Wdputslc( "\n" );
}


/*
 * Dump The Resource Table for NE module
 */
void Dmp_resrc_tab( void )
/************************/
{
    if( Os2_head.resource_off == Os2_head.resident_off ) {
        return;
    }
    Banner( "Resource Table" );
    Wlseek( New_exe_off + Os2_head.resource_off );
    if( Os2_head.target == TARGET_OS2 ) {
        dmp_resrc_tab_os2();
    } else {
        dmp_resrc_tab_win();
    }
}

/*
 * Dump The Resource Table for OS/2 LX module
 */
void Dmp_resrc2_tab( void )
/*************************/
{
    unsigned_16     i;
    unsigned_16     type_id;
    flat_res_table  res_tab;

    if( Os2_386_head.num_rsrcs == 0 ) {
        return;
    }
    Banner( "Resource Table" );
    Wlseek( New_exe_off + Os2_386_head.rsrc_off );
    type_id = 20;           /* if type_id > 15 a name won't be printed out */
    Wdputslc( "     type id   name id    res size     object     offset\n" );
    Wdputslc( "     =======   =======    ========     ======     ======\n" );
    for( i = 0; i < Os2_386_head.num_rsrcs; i++ ) {
        Wread( &res_tab, sizeof( flat_res_table ) );
        if( res_tab.type_id != type_id ) {
            type_id = res_tab.type_id;
            if( type_id < 16 ) {
                Wdputs( "type:  " );
                Wdputslc( resource_type_os2[res_tab.type_id][0] );
            }
        }
        Wdputs( "      " );
        Puthex( res_tab.type_id, 4 );
        Wdputs( "      " );
        Puthex( res_tab.name_id, 4 );
        Wdputs( "      " );
        Puthex( res_tab.res_size, 8 );
        Wdputs( "      " );
        Puthex( res_tab.object, 4 );
        Wdputs( "      " );
        Puthex( res_tab.offset, 8 );
        Wdputslc( "\n" );
    }
    Wdputslc( "\n" );
}
