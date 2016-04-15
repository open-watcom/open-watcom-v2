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
* Description:  Watcom debug format dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "walloca.h"
#include "wio.h"
#include "wdglb.h"
#include "dumpwv.h"
#include "wdfunc.h"


static  int     currSect;

static  const_string_table sdh_msg[] = {
    "4  Module info offset   = ",
    "4  Global info offset   = ",
    "4  Address info offset  = ",
    "4  Section size         = ",
    NULL
};

/*
 * print_info_title - print out a title for an info section
 */
static void print_info_title( const char *title )
/***********************************************/
{
    char        buff[80];

    sprintf( buff, "%s Info (section %d)", title, currSect );
    Banner( buff );

} /* print_info_title */

/*
 * get_len_prefix_string - make a length-prefixed string a 0 terminated string
 */
static void get_len_prefix_string( char *res, const char *str )
/*************************************************************/
{
    unsigned char   len;

    len = *(unsigned char *)str;
    memcpy( res, str + 1, len );
    res[len] = '\0';

} /* get_len_prefix_string */

/*
 * dump_line_numbers - dump line number info
 */
static void dump_line_numbers( mod_info *mi )
/*******************************************/
{
    int                 i,j;
    unsigned_32         *offs;
    int                 cnt;
    v3_line_segment     *li;
    unsigned_32         coff;
    unsigned_16         size;

    cnt = mi->di[DMND_LINES].u.entries;
    if( cnt == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wdputslc( "   *** Line Numbers ***\n" );
    Wdputslc( "   ====================\n" );

    offs = alloca( (cnt+1) * sizeof( unsigned_32 ) );
    if( offs == NULL ) {
        Wdputslc( "Error! Not enough stack.\n" );
        longjmp( Se_env, 1 );
    }
    Wlseek( Curr_sectoff + mi->di[DMND_LINES].info_off );
    Wread( offs, (cnt+1) * sizeof( unsigned_32 ) );
    Wdputs( "      " );
    Putdec( cnt );
    Wdputslc( " offset entries:\n" );
    for( i = 0; i <= cnt; i++ ) {
        Wdputs( "        offset " );
        Putdec( i );
        Wdputs( " = " );
        Puthex( offs[i], 8 );
        Wdputslc( "H\n" );
    }
    for( i = 0; i < cnt; i++ ) {
        Wlseek( Curr_sectoff + offs[i] );
        Wread( Wbuff, sizeof( v3_line_segment ) );
        li = (v3_line_segment *) Wbuff;
        coff = 0;
        for( ;; ) {
            size = (li->count - 1)* sizeof( line_info );
            Wread( Wbuff + sizeof( v3_line_segment ), size );
            Wdputslc( "      -------------------------------------\n" );
            Wdputs( "      Data " );
            Putdec( i );
            Wdputs( ": offset " );
            Puthex( offs[i], 8 );
            Wdputs( "H, addr info off = " );
            Puthex( li->segment, 8 );
            Wdputs( "H, num = " );
            Putdec( li->count );
            Wdputslc( "\n" );
            for( j = 0; j < li->count; j++ ) {
                Wdputs( "        number =" );
                Putdecl( li->line[j].line, 5 );
                Wdputs( ",  code offset = " );
                Puthex( li->line[j].code_offset, 8 );
                Wdputslc( "H\n" );
            }
            coff += sizeof( v3_line_segment ) + size;
            if( coff >= (offs[i+1] - offs[i]) ) {
                break;
            }
            Wread( Wbuff, sizeof( v3_line_segment ) );
        }
    }

} /* dump_line_numbers */

/*
 * Get_type_index - get a type index
 */
unsigned_8 *Get_type_index( unsigned_8 *ptr, unsigned_16 *index )
/***************************************************************/
{
    unsigned_16 idx;

    if( *ptr & 0x80 ) {
        idx = 0x7f & *ptr;
        idx *= 256;
        idx += *(ptr + 1);
        *index = idx;
        return( ptr + 2 );
    } else {
        *index = *ptr;
        return( ptr + 1 );
    }

} /* Get_type_index */

/*
 * Get_local_name - get a name from a local variable data structure
 */
void Get_local_name( char *name, unsigned_8 *data, unsigned_8 *start )
/********************************************************************/
{
    int len;

    len = start[0] - (data - start);
    memcpy( name, data, len );
    name[len] = 0;

} /* Get_local_name */

/*
 * dump_block - dump local block record
 */
static void dump_block( unsigned_8 *buff, bool is32 )
/***************************************************/
{
    block_386   *blk386;
    block       *blk;

    if( is32 ) {
        blk386 = (block_386 *) buff;
        Wdputs( "          start off = " );
        Puthex( blk386->start_offset, 8 );
        Wdputs( ", code size = " );
        Puthex( blk386->code_size, 8 );
        Wdputs( ", parent off = " );
        Puthex( blk386->parent_block_offset, 4 );
        Wdputslc( "\n" );
    } else {
        blk = (block *) buff;
        Wdputs( "          start off = " );
        Puthex( blk->start_offset, 4 );
        Wdputs( ", code size = " );
        Puthex( blk->code_size, 4 );
        Wdputs( ", parent off = " );
        Puthex( blk->parent_block_offset, 4 );
        Wdputslc( "\n" );
    }

} /* dump_block */

static const_string_table regLocStrs[] =
{
    "AL",
    "AH",
    "BL",
    "BH",
    "CL",
    "CH",
    "DL",
    "DH",
    "AX",
    "BX",
    "CX",
    "DX",
    "SI",
    "DI",
    "BP",
    "SP",
    "CS",
    "SS",
    "DS",
    "ES",
    "ST0",
    "ST1",
    "ST2",
    "ST3",
    "ST4",
    "ST5",
    "ST6",
    "ST7",
    "EAX",
    "EBX",
    "ECX",
    "EDX",
    "ESI",
    "EDI",
    "EBP",
    "ESP",
    "FS",
    "GS"
};

/*
 * dump_single_location_entry - dump a single location expression entry
 */
static unsigned_8 *dump_single_location_entry( unsigned_8 *buff )
/***************************************************************/
{
    unsigned_8  type;
    addr32_ptr  *p32;
    addr48_ptr  *p48;
    int         i;
    int         num;

    type = *buff;
    buff++;
    switch( type ) {
    case 0:
        Wdputslc( "<none>\n" );
        break;
    case BP_OFFSET_BYTE:
        Wdputs( "BP_OFFSET_BYTE( " );
        Puthex( *buff, 2 );
        Wdputslc( " )\n" );
        buff++;
        break;
    case BP_OFFSET_WORD:
        Wdputs( "BP_OFFSET_WORD( " );
        Puthex( *buff, 4 );
        Wdputslc( " )\n" );
        buff += sizeof( unsigned_16 );
        break;
    case BP_OFFSET_DWORD:
        Wdputs( "BP_OFFSET_DWORD( " );
        Puthex( *buff, 8 );
        Wdputslc( " )\n" );
        buff += sizeof( unsigned_32 );
        break;
    case CONST_ADDR286:
        p32 = (addr32_ptr *) buff;
        buff += sizeof( addr32_ptr );
        Wdputs( "CONST_ADDR286( " );
        Puthex( p32->segment, 4 );
        Wdputc( ':' );
        Puthex( p32->offset, 4 );
        Wdputslc( " )\n" );
        break;
    case CONST_ADDR386:
        p48 = (addr48_ptr *) buff;
        buff += sizeof( addr48_ptr );
        Wdputs( "CONST_ADDR386( " );
        Puthex( p48->segment, 4 );
        Wdputc( ':' );
        Puthex( p48->offset, 8 );
        Wdputslc( " )\n" );
        break;
    case CONST_INT_1:
        Wdputs( "CONST_INT_1( " );
        Puthex( *buff, 2 );
        Wdputslc( " )\n" );
        buff++;
        break;
    case CONST_INT_2:
        Wdputs( "CONST_INT_2( " );
        Puthex( *buff, 4 );
        Wdputslc( " )\n" );
        buff += sizeof( unsigned_16 );
        break;
    case CONST_INT_4:
        Wdputs( "CONST_INT_4( " );
        Puthex( *buff, 8 );
        Wdputslc( " )\n" );
        buff += sizeof( unsigned_32 );
        break;
    case IND_REG_CALLOC_NEAR:
        Wdputs( "IND_REG_CALLOC_NEAR( " );
        Wdputs( regLocStrs[ *buff ] );
        Wdputslc( " )\n" );
        buff++;
        break;
    case IND_REG_CALLOC_FAR:
        Wdputs( "IND_REG_CALLOC_FAR( " );
        Wdputs( regLocStrs[ *buff ] );
        Wdputc( ':' );
        Wdputs( regLocStrs[ *(buff+1) ] );
        Wdputslc( " )\n" );
        buff += 2;
        break;
    case IND_REG_RALLOC_NEAR:
        Wdputs( "IND_REG_RALLOC_NEAR( " );
        Wdputs( regLocStrs[ *buff ] );
        Wdputslc( " )\n" );
        buff++;
        break;
    case IND_REG_RALLOC_FAR:
        Wdputs( "IND_REG_RALLOC_FAR( " );
        Wdputs( regLocStrs[ *buff ] );
        Wdputc( ':' );
        Wdputs( regLocStrs[ *(buff+1) ] );
        Wdputslc( " )\n" );
        buff += 2;
        break;
    case OPERATOR_IND_2:
        Wdputslc( "OPERATOR_IND_2\n" );
        break;
    case OPERATOR_IND_4:
        Wdputslc( "OPERATOR_IND_4\n" );
        break;
    case OPERATOR_IND_ADDR286:
        Wdputslc( "OPERATOR_IND_ADDR286\n" );
        break;
    case OPERATOR_IND_ADDR386:
        Wdputslc( "OPERATOR_IND_ADDR386\n" );
        break;
    case OPERATOR_ZEB:
        Wdputslc( "OPERATOR_ZEB\n" );
        break;
    case OPERATOR_ZEW:
        Wdputslc( "OPERATOR_ZEW\n" );
        break;
    case OPERATOR_MK_FP:
        Wdputslc( "OPERATOR_MK_FP\n" );
        break;
    case OPERATOR_POP:
        Wdputslc( "OPERATOR_POP\n" );
        break;
    case OPERATOR_XCHG:
        Wdputs( "OPERATOR_XCHG: " );
        Puthex( *buff, 2 );
        Wdputslc( "\n" );
        buff++;
        break;
    case OPERATOR_ADD:
        Wdputslc( "OPERATOR_ADD\n" );
        break;
    case OPERATOR_DUP:
        Wdputslc( "OPERATOR_DUP\n" );
        break;
    case OPERATOR_NOP:
        Wdputslc( "OPERATOR_NOP\n" );
        break;
    default:
        if( type & 0x30 ) {
            num = (type & 0x0f)+1;
            Wdputs( "MULTI_REG(" );
            Putdec( num );
            Wdputs( "): " );
            for( i=0;i<num;i++ ) {
                Wdputs( regLocStrs[ *buff ] );
                if( i != num-1 ) {
                    Wdputs( ", " );
                } else {
                    Wdputslc( "\n" );
                }
                buff++;
            }
        } else if( type & 0x40 ) {
            Wdputs( "REG: " );
            Wdputs( regLocStrs[ type & 0x0f ] );
            Wdputslc( "\n" );
        } else {
            Wdputslc( "**** UNKNOWN LOCATION EXPRESSION!! ****\n" );
        }
        break;
    }

    return( buff );

} /* dump_single_location_entry */

/*
 * Dump_location_expression - dump out a location expression
 */
unsigned_8 *Dump_location_expression( unsigned_8 *buff, char *spacing )
/*********************************************************************/
{
    int         multi;
    unsigned_8  *end;

    if( *buff & 0x80 ) {
        end = buff + (*buff & 0x7f);
        buff++;
        multi = 1;
    } else {
        end = buff + 1;
        multi = 0;
    }
    while( buff < end ) {
        switch( multi ) {
        case 1:
            multi = 2;
            Wdputslc( "\n" );
            /* fall through */
        case 2:
            Wdputslc( spacing );
            break;
        }
        buff = dump_single_location_entry( buff );
    }
    return( buff );
} /* Dump_location_expression */

/*
 * dump_rtn - dump a near or far routine defn
 */
static void dump_rtn( unsigned_8 *buff )
/**************************************/
{
    int         pro,epi;
    unsigned_16 ret_off;
    unsigned_8  *ptr;
    int         num_parms;
    unsigned_16 index;
    char        name[256];
    int         i;

    dump_block( buff, false );

    ptr = buff + sizeof( block );
    pro = *ptr++;
    epi = *ptr++;
    Wdputs( "          prologue size = " );
    Putdec( pro );
    Wdputs( ",  epilogue size = " );
    Putdec( epi );
    Wdputslc( "\n" );

    ret_off = *(unsigned_16 *) ptr;
    ptr += sizeof( unsigned_16 );
    Wdputs( "          return address offset (from bp) = " );
    Puthex( ret_off, 4 );
    Wdputslc( "\n" );

    ptr = Get_type_index( ptr, &index );
    Wdputs( "          procedure type:  " );
    Putdec( index );
    Wdputslc( "\n" );

    Wdputs( "          return location: " );
    ptr = Dump_location_expression( ptr, "            " );

    num_parms = *ptr++;
    for( i = 0; i < num_parms; i++ ) {
        Wdputs( "          Parm " );
        Putdec( i );
        Wdputs( ": " );
        ptr = Dump_location_expression( ptr, "            " );
    }
    Get_local_name( name, ptr, buff );
    Wdputs( "          Name = \"" );
    Wdputs( name );
    Wdputslc( "\"\n" );

} /* dump_rtn */

/*
 * dump_rtn386 - dump a near or far routine defn (386)
 */
static void dump_rtn386( unsigned_8 *buff )
/*****************************************/
{
    int         pro,epi;
    unsigned_32 ret_off;
    unsigned_8  *ptr;
    int         num_parms;
    unsigned_16 index;
    char        name[256];
    int         i;

    dump_block( buff, true );

    ptr = buff + sizeof( block_386 );
    pro = *ptr++;
    epi = *ptr++;
    Wdputs( "          prologue size = " );
    Putdec( pro );
    Wdputs( ",  epilogue size = " );
    Putdec( epi );
    Wdputslc( "\n" );

    ret_off = *(unsigned_32 *) ptr;
    ptr += sizeof( unsigned_32 );
    Wdputs( "          return address offset (from bp) = " );
    Puthex( ret_off, 8 );
    Wdputslc( "\n" );

    ptr = Get_type_index( ptr, &index );
    Wdputs( "          return type:  " );
    Putdec( index );
    Wdputslc( "\n" );

    Wdputs( "          return value: " );
    ptr = Dump_location_expression( ptr, "            " );

    num_parms = *ptr++;
    for( i = 0; i < num_parms; i++ ) {
        Wdputs( "          Parm " );
        Putdec( i );
        Wdputs( ": " );
        ptr = Dump_location_expression( ptr, "            " );
    }
    Get_local_name( name, ptr, buff );
    Wdputs( "          Name = \"" );
    Wdputs( name );
    Wdputslc( "\"\n" );

} /* dump_rtn386 */

/*
 * dump_locals - dump all local variable information
 */
static void dump_locals( mod_info *mi )
/*************************************/
{
    int         i;
    unsigned_32 *offs;
    int         cnt;
    unsigned_32 coff;
    unsigned_8  buff[256];
    char        name[256];
    set_base    *sb;
    set_base386 *sb386;
    unsigned_8  *ptr;
    addr32_ptr  *p32;
    addr48_ptr  *p48;
    unsigned_16 index;

    cnt = mi->di[DMND_LOCALS].u.entries;
    if( cnt == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wdputslc( "   *** Locals ***\n" );
    Wdputslc( "   ==============\n" );

    offs = alloca( (cnt+1) * sizeof( unsigned_32 ) );
    if( offs == NULL ) {
        Wdputslc( "Error! Not enough stack.\n" );
        longjmp( Se_env, 1 );
    }
    Wlseek( Curr_sectoff + mi->di[DMND_LOCALS].info_off );
    Wread( offs, (cnt+1) * sizeof( unsigned_32 ) );
    for( i = 0; i < cnt; i++ ) {
        coff = 0;
        Wdputs( "      Data " );
        Putdec( i );
        Wdputs( ":  offset " );
        Puthex( offs[i], 8 );
        Wdputslc( "\n" );
        for( ;; ) {
            Wlseek( coff + Curr_sectoff + offs[i] );
            Wread( buff, sizeof( buff ) );
            Wdputs( "        " );
            Puthex( coff, 4 );
            Wdputs( ": " );
            switch( buff[1] ) {
            case MODULE:
                Wdputslc( "MODULE\n" );
                ptr = buff+2;
                p32 = (addr32_ptr *)ptr;
                ptr += sizeof( addr32_ptr );
                ptr = Get_type_index( ptr, &index );
                Get_local_name( name, ptr, buff );
                Wdputs( "          \"" );
                Wdputs( name );
                Wdputs( "\"  addr = " );
                Puthex( p32->segment, 4 );
                Wdputc( ':' );
                Puthex( p32->offset, 4 );
                Wdputs( ", type = " );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case LOCAL:
                Wdputslc( "LOCAL\n" );
                ptr = buff+2;
                Wdputs( "          address: " );
                ptr = Dump_location_expression( ptr, "            " );
                ptr = Get_type_index( ptr, &index );
                Get_local_name( name, ptr, buff );
                Wdputs( "          name = \"" );
                Wdputs( name );
                Wdputs( "\",  type = " );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case MODULE_386:
                Wdputslc( "MODULE_386\n" );
                ptr = buff+2;
                p48 = (addr48_ptr *)ptr;
                ptr += sizeof( addr48_ptr );
                ptr = Get_type_index( ptr, &index );
                Get_local_name( name, ptr, buff );
                Wdputs( "          \"" );
                Wdputs( name );
                Wdputs( "\" addr = " );
                Puthex( p48->segment, 4 );
                Wdputc( ':' );
                Puthex( p48->offset, 8 );
                Wdputs( ",  type = " );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case MODULE_LOC:
                Wdputslc( "MODULE_LOC\n" );
                ptr = buff+2;
                Wdputs( "          address: " );
                ptr = Dump_location_expression( ptr, "            " );
                ptr = Get_type_index( ptr, &index );
                Get_local_name( name, ptr, buff );
                Wdputs( "          name = \"" );
                Wdputs( name );
                Wdputs( "\",  type = " );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case BLOCK:
                Wdputslc( "BLOCK\n" );
                dump_block( buff, false );
                break;
            case NEAR_RTN:
                Wdputslc( "NEAR_RTN\n" );
                dump_rtn( buff );
                break;
            case FAR_RTN:
                Wdputslc( "FAR_RTN\n" );
                dump_rtn( buff );
                break;
            case BLOCK_386:
                Wdputslc( "BLOCK_386\n" );
                dump_block( buff, true );
                break;
            case NEAR_RTN_386:
                Wdputslc( "NEAR_RTN_386\n" );
                dump_rtn386( buff );
                break;
            case FAR_RTN_386:
                Wdputslc( "FAR_RTN_386\n" );
                dump_rtn386( buff );
                break;
            case MEMBER_SCOPE:
                Wdputslc( "MEMBER_SCOPE\n" );
                index = 5;
                ptr = buff+2;
                Wdputs( "          parent offset = " );
                Puthex( *ptr, 4 );
                ptr += 2;
                if( *ptr & 0x80 ) {
                    index = 6;
                }
                Wdputs( "  class type = " );
                ptr = Get_type_index( ptr, &index );
                Putdec( index );
                if( buff[0] > index ) {
                    Wdputslc( "\n          object ptr type = " );
                    Puthex( *ptr++, 2 );
                    Wdputs( "  object loc = " );
                    Dump_location_expression( ptr, "            " );
                }
                Wdputslc( "\n" );
                break;
            case ADD_PREV_SEG:
                Wdputslc( "ADD_PREV_SEG\n" );
                Wdputs( "          segment increment = " );
                Puthex( *(buff+2), 4 );
                Wdputslc( "\n" );
                break;
            case SET_BASE:
                Wdputslc( "SET_BASE\n" );
                sb = (set_base *) buff;
                Wdputs( "          base = " );
                Puthex( sb->seg, 4 );
                Wdputc( ':' );
                Puthex( sb->off, 4 );
                Wdputslc( "\n" );
                break;
            case SET_BASE_386:
                Wdputslc( "SET_BASE_386\n" );
                sb386 = (set_base386 *) buff;
                Wdputs( "          base = " );
                Puthex( sb386->seg, 4 );
                Wdputc( ':' );
                Puthex( sb386->off, 8 );
                Wdputslc( "\n" );
                break;
            }
            coff += buff[0];
            if( coff >= (offs[i+1] - offs[i]) ) {
                break;
            }
        }
    }

} /* dump_locals */

/*
 * dump_types - dump all typing information
 */
static void dump_types( mod_info *mi )
/************************************/
{
    unsigned_32 *offs;
    int         cnt;

    cnt = mi->di[DMND_TYPES].u.entries;
    if( cnt == 0 ) {
        return;
    }
    Wdputslc( "\n" );
    Wdputslc( "    *** Types ***\n" );
    Wdputslc( "    =============\n" );

    offs = alloca( (cnt+1) * sizeof( unsigned_32 ) );
    if( offs == NULL ) {
        Wdputslc( "Error! Not enough stack.\n" );
        longjmp( Se_env, 1 );
    }
    Wlseek( Curr_sectoff + mi->di[DMND_TYPES].info_off );
    Wread( offs, (cnt+1) * sizeof( unsigned_32 ) );
    Dmp_type( cnt, offs );
}

/*
 * dump_module_info - dump module info
 */
static void dump_module_info( section_dbg_header *sdh )
/*****************************************************/
{
    unsigned_32 bytes_read;
    mod_info    *mi;
    unsigned_32 total_bytes;
    long        cpos;
    char        name[256];
    unsigned_16 index;
    mod_info    *tmi;

    total_bytes = sdh->gbl_offset - sdh->mod_offset;
    print_info_title( "Module" );

    bytes_read = 0;
    mi = (mod_info *) Wbuff;
    tmi = alloca( sizeof( mod_info ) + 255 );
    if( tmi == NULL ) {
        Wdputslc( "Error! Not enough stack.\n" );
        longjmp( Se_env, 1 );
    }
    cpos = Curr_sectoff + sdh->mod_offset;
    index = 0;
    while( bytes_read < total_bytes ) {
        Wlseek( cpos );
        Wread( Wbuff, sizeof( mod_info ) + 255 );
        bytes_read += sizeof( mod_info ) + mi->name[0];
        cpos += sizeof( mod_info ) + mi->name[0];
        get_len_prefix_string( name, mi->name );
        Putdecl( index, 3 );
        Wdputs( ") Name:   ");
        Wdputs( name );
        Wdputslc( "\n" );
        Wdputs( "     Language is " );
        Wdputs( &Lang_lst[ mi->language ] );
        Wdputslc( "\n" );
        Wdputs( "     Locals: num = " );
        Putdec( mi->di[DMND_LOCALS].u.entries );
        Wdputs( ", offset = " );
        Puthex( mi->di[DMND_LOCALS].info_off, 8 );
        Wdputslc( "H\n" );
        Wdputs( "     Types:  num = " );
        Putdec( mi->di[DMND_TYPES].u.entries );
        Wdputs( ", offset = " );
        Puthex( mi->di[DMND_TYPES].info_off, 8 );
        Wdputslc( "H\n" );
        Wdputs( "     Lines:  num = " );
        Putdec( mi->di[DMND_LINES].u.entries );
        Wdputs( ", offset = " );
        Puthex( mi->di[DMND_LINES].info_off, 8 );
        Wdputslc( "H\n" );
        memcpy( tmi, mi, sizeof( mod_info ) + mi->name[0] );
        if( Debug_options & LOCALS ) {
            dump_locals( tmi );
        }
        if( Debug_options & LINE_NUMS ) {
            dump_line_numbers( tmi );
        }
        if( Debug_options & TYPES ) {
            dump_types( tmi );
        }
        Wdputslc( "\n" );
        index++;
    }
} /* dump_module_info */

/*
 * dump_global_info - dump out global info
 */
static void dump_global_info( section_dbg_header *sdh )
/*****************************************************/
{
    unsigned_32 total_bytes;
    unsigned_32 bytes_read;
    v3_gbl_info *gi;
    long        cpos;
    char        name[256];

    total_bytes = sdh->addr_offset - sdh->gbl_offset;
    print_info_title( "Global" );

    bytes_read = 0;
    gi = (v3_gbl_info *) Wbuff;
    cpos = Curr_sectoff + sdh->gbl_offset;
    while( bytes_read < total_bytes ) {
        Wlseek( cpos );
        Wread( Wbuff, sizeof( v3_gbl_info ) + 255 );
        bytes_read += sizeof( v3_gbl_info ) + gi->name[0];
        cpos += sizeof( v3_gbl_info ) + gi->name[0];
        get_len_prefix_string( name, gi->name );
        Wdputs( "  Name:  " );
        Wdputs(  name );
        Wdputslc( "\n" );
        Wdputs( "    address      = " );
        Puthex( gi->addr.segment, 4 );
        Wdputc( ':' );
        Puthex( gi->addr.offset, 8 );
        Wdputslc( "\n" );
        Wdputs( "    module index = " );
        Putdec( gi->mod );
        Wdputslc( "\n" );
        Wdputs( "    kind:         " );
        if( gi->kind & GBL_KIND_STATIC ) {
            Wdputs( " (static pubdef)" );
        }
        if( gi->kind & GBL_KIND_CODE ) {
            Wdputs( " (code)" );
        }
        if( gi->kind & GBL_KIND_DATA ) {
            Wdputs( " (data)" );
        }
        Wdputslc( "\n" );
    }
    Wdputslc( "\n" );

} /* dump_global_info */

/*
 * dump_addr_info - dump out address info
 */
static void dump_addr_info( section_dbg_header *sdh )
/***************************************************/
{
    unsigned_32 total_bytes;
    unsigned_32 bytes_read;
    seg_info    *si;
    long        cpos;
    long        basepos;
    unsigned_32 len;
    int         i;
    unsigned_32 seg_off;

    total_bytes = sdh->section_size - sdh->addr_offset;
    print_info_title( "Addr" );

    bytes_read = 0;
    si = (seg_info *) Wbuff;
    basepos = cpos = Curr_sectoff + sdh->addr_offset;
    while( bytes_read < total_bytes ) {
        Wlseek( cpos );
        Wread( Wbuff, sizeof( seg_info ) );
        Wlseek( cpos );
        len = sizeof( seg_info ) + (si->count - 1) * sizeof( addr_info );
        Wread( Wbuff, len );
        Wdputs( " Base:  fileoff = " );
        Puthex( cpos-basepos, 8 );
        Wdputs( "H   seg = " );
        Puthex( si->base.segment, 4 );
        Wdputs( "H,  off = " );
        Puthex( si->base.offset, 8 );
        Wdputslc( "H\n" );
        seg_off = si->base.offset;
        for( i = 0; i < si->count; i++ ) {
            Putdecl( i, 6 );
            Wdputs( ") fileoff = " );
            Puthex( (long) cpos - basepos + sizeof( seg_info) +
                        i * sizeof( addr_info ) - sizeof( addr_info ), 8 );
            Wdputs( "H,  Size = " );
            Puthex( si->addr[i].size, 8 );
            Wdputs( "H @" );
            Puthex( seg_off, 8 );
            Wdputs( "H,  mod_index = " );
            Putdec( si->addr[i].mod );
            Wdputslc( "\n" );
            seg_off += si->addr[i].size;
        }
        cpos += len;
        bytes_read += len;
    }
} /* dump_addr_info */

/*
 * dump_section - dump the current section
 */
void Dump_section( void )
/***********************/
{
    section_dbg_header  sdh;

    Wread( &sdh, sizeof( section_dbg_header ) );
    Wdputs( "Section " );
    Putdec( sdh.section_id );
    Wdputs( " (off=" );
    Puthex( Curr_sectoff, 8 );
    Wdputslc( ")\n" );
    Wdputslc( "=========================\n" );
    Dump_header( (char *)&sdh.mod_offset, sdh_msg );
    Wdputslc( "\n" );
    currSect = sdh.section_id;

    if( Debug_options & MODULE_INFO ) {
        dump_module_info( &sdh );
    }
    if( Debug_options & GLOBAL_INFO ) {
        dump_global_info( &sdh );
    }
    if( Debug_options & ADDR_INFO ) {
        dump_addr_info( &sdh );
    }
} /* dump_section */
