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
* Description:  Resource dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>

#include "wdglb.h"
#include "wdfunc.h"


typedef struct os2_res_entry {
    unsigned_16         type_id;
    unsigned_16         name_id;
} os2_res_entry;

static  char    *resource_type[] = {
    "Unkown resource type\n",
    "Cursor\n",
    "Bitmap\n",
    "Icon\n",
    "Menu template\n",
    "Dialog-box template\n",
    "String table\n",
    "Font directory\n",
    "Font\n",
    "Keyboard-accelerator table\n",
    "RC data resource\n",
    "Error message table\n",
    "Cursor group header\n",
    "Unkown resource type\n",       /* #13 is not used - unlucky? */
    "Icon group header\n",
    "Nametable\n"
};

static  char    *resource_type_os2[] = {
    "Unkown resource type\n",
    "Pointer\n",
    "Bitmap\n",
    "Menu template\n",
    "Dialog template\n",
    "String table\n",
    "Font directory\n",
    "Font\n",
    "Accelerator table\n",
    "RC data resource\n",
    "Error message table\n",
    "Dialog include file name\n",
    "Key to virtual key translation table\n",
    "Key to UGL translation table\n",
    "Glyph to character translation table\n",
    "Screen display information\n",
    "Function key area (short form)\n",
    "Function key area (long form)\n",
    "Help table\n",
    "Help subtable\n",
    "DBCS font driver directory\n",
    "DBCS font driver\n",
    "Default icon\n"
};


/*
 * get a resource type name
 */
static char *get_resrc_nam( unsigned_16 offset )
/**********************************************/
{
    unsigned_8      num_chars;
    char            *name;

    Wlseek( New_exe_off + Os2_head.resource_off + offset );
    Wread( &num_chars, sizeof( unsigned_8 ) );
    name = Wmalloc( num_chars + 1 );
    Wread( name, num_chars );
    name[ num_chars ] = '\0';
    return( name );
}

/*
 * printout a resource name
 */
static void dmp_resrc_nam( unsigned_16 res_type )
/***********************************************/
{
    char    *name;

    Wdputc( ' ' );
    if( res_type & SEG_RESRC_HIGH ) {
        res_type &= ~SEG_RESRC_HIGH;
        Wdputs( "resource id: " );
        Putdec( res_type );
        Wdputslc( "\n" );
    } else {
        name = get_resrc_nam( res_type );
        Wdputs( name );
        Wdputslc( "\n" );
        free( name );
    }
}

/*
 * Dump a Resource Flag Word
 */
static void dmp_resrc_flag( unsigned_16 flag )
/********************************************/
{
    if( flag & SEG_MOVABLE ) {
        Wdputs( "MOVABLE" );
    } else {
        Wdputs( "FIXED" );
    }
    if( flag & SEG_PURE ) {
        Wdputs( "|SHARE" );
    } else {
        Wdputs( "|NOSHARE" );
    }
    if( flag & SEG_PRELOAD ) {
        Wdputs( "|PRELOAD" );
    } else {
        Wdputs( "|LOADONCALL" );
    }
    Wdputs( " Prior " );
    Putdec( flag >> SEG_SHIFT_PRI_LVL );
}

/*
 * dump a resource description
 */
static void dmp_resrc_desc( struct resource_record * res_ent )
/************************************************************/
{
    unsigned_32                   res_off;
    unsigned_32                   res_len;
    unsigned_32                   res_end;

    dmp_resrc_nam( res_ent->name );
    Wdputs( " file offset " );
    res_off = (unsigned_32)res_ent->offset
        << Resrc_shift_cnt;
    Puthex( res_off, 8 );
    Wdputs( "H len " );
    res_len = (unsigned_32)res_ent->length
        << Resrc_shift_cnt;
    Puthex( res_len, 8 );
    Wdputs( " flag " );
    dmp_resrc_flag( res_ent->flags );
    Wdputslc( "\n" );
    if( Options_dmp & RESRC_DMP ) {
        Wdputslc( "    data =\n" );
        Dmp_seg_data( res_off, res_len );
    }
    res_end = res_off + res_len;
    if( res_end > Resrc_end ) {
        Resrc_end = res_end;
    }
}

/*
 * dump some resource entries
 */
static void dmp_resrc_ent( unsigned_16 num_resources )
/****************************************************/
{
    struct resource_record      *res_ent_tab;
    struct resource_record      *res_ent;
    unsigned_16                 res_group_size;
    unsigned_16                 res_num;

    if( num_resources == 0 ) {
        return;
    }
    res_group_size = num_resources * sizeof( struct resource_record );
    res_ent = res_ent_tab = Wmalloc( res_group_size );
    Wread( res_ent_tab, res_group_size );
    for( res_num = 0; res_num != num_resources; res_num++ ) {
        Wdputs( " # " );
        Putdec( res_num + 1 );
        dmp_resrc_desc( res_ent++ );
    }
    free( res_ent_tab );
}

/*
 * printout a resource type name
 */
static void dmp_resrc_type_nam( unsigned_16 res_type )
/****************************************************/
{
    char    *name;

    Wdputc( ' ' );
    if( res_type & SEG_RESRC_HIGH ) {
        res_type &= ~SEG_RESRC_HIGH;
        if( res_type > 15 ) {
            Wdputs( "Type number: " );
            Putdec( res_type );
            Wdputslc( "\n" );
        } else {
            Wdputslc( resource_type[ res_type ] );
        }
    } else {
        name = get_resrc_nam( res_type );
        Wdputs( name );
        Wdputslc( "\n" );
        free( name );
    }
}

/*
 * Dump The Resource Table for Windows NE module
 */
static void dmp_resrc_tab_win( void )
/***********************************/
{
    unsigned_16                             res_type;
    auto struct resource_type_record        res_group;
    unsigned_32                             offset;

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
        dmp_resrc_type_nam( res_type );
        Wlseek( offset );
        dmp_resrc_ent( res_group.num_resources );
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
    unsigned_16     id;
    os2_res_entry   res_tab;

    id = 30;                /* if id > 22 a name won't be printed out */
    Wdputslc( "    seg#   type id   name id\n" );
    Wdputslc( "    ====   =======   =======\n" );
    for( i = 0; i < Os2_head.resource; i++ ) {
        Wread( &res_tab, sizeof( os2_res_entry ) );
        if( res_tab.type_id != id ) {
            id = res_tab.type_id;
            if( id < 23 ) {
                Wdputs( "type:  " );
                Wdputslc( resource_type_os2[ res_tab.type_id ] );
            }
        }
        Wdputs( "    " );
        Puthex( i + Os2_head.segments - Os2_head.resource + 1, 4 );
        Wdputs( "   " );
        Puthex( res_tab.type_id, 4 );
        Wdputs( "      " );
        Puthex( res_tab.name_id, 4 );
        Wdputs( "      " );
        Wdputslc( "\n" );
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
    if( Os2_head.target == TARGET_OS2 )
        dmp_resrc_tab_os2();
    else
        dmp_resrc_tab_win();
}

/*
 * Dump The Resource Table for OS/2 LX module
 */
void Dmp_resrc2_tab( void )
/*************************/
{
    unsigned_16     i;
    unsigned_16     id;
    flat_res_table  res_tab;

    if( Os2_386_head.num_rsrcs == 0 ) {
        return;
    }
    Banner( "Resource Table" );
    Wlseek( New_exe_off + Os2_386_head.rsrc_off );
    id = 20;                /* if id > 15 a name won't be printed out */
    Wdputslc( "     type id   name id    res size     object     offset\n" );
    Wdputslc( "     =======   =======    ========     ======     ======\n" );
    for( i = 0; i < Os2_386_head.num_rsrcs; i++ ) {
        Wread( &res_tab, sizeof( flat_res_table ) );
        if( res_tab.type_id != id ) {
            id = res_tab.type_id;
            if( id < 16 ) {
                Wdputs( "type:  " );
                Wdputslc( resource_type_os2[ res_tab.type_id ] );
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
