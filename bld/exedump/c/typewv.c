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
* Description:  Watcom debug format type processing.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "wio.h"
#include "wdglb.h"
#include "dumpwv.h"
#include "wdfunc.h"

/*
 * scalar_type - dump type of scalar item
 */
static void scalar_type( unsigned_8 type )
/****************************************/
{
    Puthex( type, 2 );
    switch( type >> 4 ) {
    case 0:
        Wdputslc( ", integer" );
        break;
    case 1:
        Wdputslc( ", unsigned" );
        break;
    case 2:
        Wdputslc( ", float" );
        break;
    case 3:
        Wdputslc( ", void" );
        break;
    case 4:
        Wdputslc( ", complex" );
        break;
    }

} /* scalar_type */

/*
 * attribute_byte - dump type of scalar item
 */
static void attribute_byte( unsigned_8 type )
/*******************************************/
{
    Puthex( type, 2 );
    if( type & 0x1 ) {
        Wdputslc( ", internal" );
    }
    if( type & 0x2 ) {
        Wdputslc( ", public" );
    }
    if( type & 0x4 ) {
        Wdputslc( ", protected" );
    }
    if( type & 0x8 ) {
        Wdputslc( ", private" );
    }

} /* attribute_byte */

/*
 * base_type_index - dump info
 */
static unsigned_8 *base_type_index( unsigned_8 *buff )
/****************************************************/
{
    unsigned_16 index;
    unsigned_8  *ptr;

    Wdputs( "   base type idx = " );
    ptr = Get_type_index( buff, &index );
    Putdec( index );
    Wdputslc( "\n" );
    return( ptr );
}

/*
 * near_ptr - dump info
 */
static void near_ptr( unsigned_8 *buff )
/**************************************/
{
    unsigned_16 index;
    unsigned_8  *ptr;

    index = 3;
    ptr = buff+2;
    if( *ptr & 0x80 ) {
        index = 4;
    }
    ptr = base_type_index( ptr );
    if( buff[0] > index ) {
        Wdputs( "          base locator = " );
        Dump_location_expression( ptr, "            " );
    }
}

/*
 * param_type_index - dump info
 */
static void param_type_index( unsigned_8 num_params, unsigned_8 *ptr )
/********************************************************************/
{
    unsigned_8  i;
    unsigned_16 index;

    if( *ptr & 0x80 ) {
        num_params /= 2;
    }
    for( i = 0; i < num_params; i++ ) {
        Wdputslc( "\n" );
        Wdputs( "            param " );
        Putdec( i+1 );
        Wdputs( ":  type idx = " );
        ptr = Get_type_index( ptr, &index );
        Putdec( index );
    }
    Wdputslc( "\n" );
}

/*
 * near_far_proc - dump info
 */
static void near_far_proc( unsigned_8 *buff )
/*******************************************/
{
    unsigned_8      *ptr;
    unsigned_16     index;
    unsigned_8      num_parms;

    ptr = buff + 2;
    Wdputs( "          return type = " );
    num_parms = buff[0] - 4;
    if( *ptr & 0x80 ) {
        num_parms--;
    }
    ptr = Get_type_index( ptr, &index );
    Putdec( index );
    ptr++;
    param_type_index( num_parms, ptr );
}

/*
 * array_index - dump info
 */
static void array_index( unsigned_8 *ptr, unsigned_8 size )
/*********************************************************/
{
    Wdputs( "          high bound = " );
    Puthex( *ptr, 2*size );
    base_type_index( ptr+size );
}

/*
 * bit_field_struct - dump info
 */
static void bit_field_struct( unsigned_8 *buff, unsigned_8 size, bool bit )
/*************************************************************************/
{
    unsigned_8  *ptr;
    unsigned_16 index;
    char        name[256];

    ptr = buff+2+size;
    if( bit ) {
        ptr += 2;
    }
    ptr = Get_type_index( ptr, &index );
    Get_local_name( name, ptr, buff );
    Wdputs( "          \"" );
    Wdputs( name );
    Wdputs( "\"  offset = " );
    ptr = buff+2;
    Puthex( *ptr, 2*size );
    Wdputs( "  type idx = " );
    Putdec( index );
    if( bit ) {
        Wdputslc( "\n        start bit = " );
        ptr++;
        Puthex( *ptr, 2 );
        Wdputslc( "  bit size = " );
        ptr++;
        Puthex( *ptr, 2 );
    }
    Wdputslc( "\n" );
}

/*
 * bit_field_class - dump info
 */
static void bit_field_class( unsigned_8 *buff, bool bit )
/*******************************************************/
{
    unsigned_8  *ptr;
    unsigned_16 index;
    char        name[256];

    ptr = buff+3;
    Wdputs( "          field locator = " );
    ptr = Dump_location_expression( ptr, "            " );
    if( bit ) {
        ptr += 2;
    }
    ptr = Get_type_index( ptr, &index );
    Get_local_name( name, ptr, buff );
    Wdputs( "          name = \"" );
    Wdputs( name );
    Wdputs( "\"  type idx = " );
    Putdec( index );
    Wdputslc( "\n          attribute = " );
    attribute_byte( buff[2] );
    if( bit ) {
        Wdputslc( "  start bit = " );
        ptr++;
        Puthex( *ptr, 2 );
        Wdputslc( "  bit size = " );
        ptr++;
        Puthex( *ptr, 2 );
    }
    Wdputslc( "\n" );
}

/*
 * range - dump info
 */
static void range( unsigned_8 *ptr, unsigned_8 size )
/***************************************************/
{
    Wdputs( "          low bound = " );
    Puthex( *ptr, 2*size );
    Wdputs( "   high bound = " );
    ptr += size;
    Puthex( *ptr, 2*size );
    ptr += size;
    base_type_index( ptr );
}

/*
 * enum_const - dump info
 */
static void enum_const( unsigned_8 *buff, unsigned_8 size )
/*********************************************************/
{
    char        name[256];

    Get_local_name( name, buff+2+size, buff );
    Wdputs( "          \"" );
    Wdputs( name );
    Wdputs( "\"   value = " );
    Puthex( *(buff+2), 2*size );
    Wdputslc( "\n" );
}

/*
 * desc_array - dump info
 */
static void desc_array( unsigned_8 *ptr, bool is386 )
/***************************************************/
{
    addr32_ptr  *p32;
    addr48_ptr  *p48;

    Wdputs( "          scalar type 1 = " );
    scalar_type( ptr[0] );
    Wdputs( "  scalar type 2 = " );
    scalar_type( ptr[1] );
    ptr += 2;
    Wdputslc( "\n          addr = " );
    if( is386 ) {
        p48 = (addr48_ptr *)ptr;
        ptr += sizeof(addr48_ptr);
        Puthex( p48->segment, 4 );
        Wdputc( ':' );
        Puthex( p48->offset, 8 );
    } else {
        p32 = (addr32_ptr *)ptr;
        ptr += sizeof(addr32_ptr);
        Puthex( p32->segment, 4 );
        Wdputc( ':' );
        Puthex( p32->offset, 4 );
    }
    base_type_index( ptr );

} /* desc_array */

static void StartType( char *name, int index ){
      Wdputs( name );
      Wdputs( "(" );
      Putdec( index );
      Wdputslc( ")\n" );
}
/*
 * Dump_types - dump all typing information
 */
void Dmp_type( int cnt, unsigned_32 *offs )
/*****************************************/
{
    int         i;
    addr32_ptr  *p32;
    addr48_ptr  *p48;
    unsigned_8  *ptr;
    unsigned_16 index;
    unsigned_16 curr_index;
    unsigned_32 coff;
    char        name[256];
    unsigned_8  buff[256];

    for( i = 0; i < cnt; i++ ) {
        coff = 0;
        Wdputs( "      Data " );
        Putdec( i );
        Wdputs( ":  offset " );
        Puthex( offs[i], 8 );
        Wdputslc( "\n" );
        curr_index = 0;
        while( 1 ) {
            Wlseek( coff + Curr_sectoff + offs[i] );
            Wread( buff, sizeof( buff ) );
            Wdputs( "        " );
            Puthex( coff, 4 );
            Wdputs( ": " );
            ptr = buff+2;
            switch( buff[1] ) {
            case SCALAR:
                StartType( "SCALAR", ++curr_index );
                ptr = buff+3;
                Get_local_name( name, ptr, buff );
                Wdputs( "          \"" );
                Wdputs( name );
                Wdputs( "\"  scalar type = " );
                scalar_type( buff[2] );
                Wdputslc( "\n" );
                break;
            case SCOPE:
                StartType( "SCOPE", ++curr_index);
                Get_local_name( name, ptr, buff );
                Wdputs( "          \"" );
                Wdputs( name );
                Wdputslc( "\"\n" );
                break;
            case NAME:
                StartType( "NAME", ++curr_index);
                ptr = Get_type_index( ptr, &index );
                ptr = Get_type_index( ptr, &index );
                Get_local_name( name, ptr, buff );
                Wdputs( "          \"" );
                Wdputs( name );
                Wdputs( "\"  type idx = " );
                Putdec( index );
                Wdputs( "  scope idx = " );
                ptr = Get_type_index( buff+2, &index );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case CUE_TABLE:
                Wdputs( "cue table offset=" );
                Puthex( *(unsigned_32 *)ptr, 8 );
                Wdputslc( "\n" );
                break;
            case TYPE_EOF:
                return;
            case BYTE_INDEX:
                StartType( "BYTE_INDEX ARRAY", ++curr_index);
                array_index( ptr, 1 );
                break;
            case WORD_INDEX:
                StartType( "WORD_INDEX ARRAY", ++curr_index);
                array_index( ptr, 2 );
                break;
            case LONG_INDEX:
                StartType( "LONG_INDEX ARRAY", ++curr_index);
                array_index( ptr, 4 );
                break;
            case TYPE_INDEX:
                StartType( "TYPE_INDEX ARRAY", ++curr_index);
                Wdputs( "          index type = " );
                ptr = Get_type_index( ptr, &index );
                Putdec( index );
                base_type_index( ptr );
                break;
            case DESC_INDEX:
                StartType( "DESC_INDEX ARRAY", ++curr_index);
                desc_array( ptr, FALSE );
                break;
            case DESC_INDEX_386:
                StartType( "DESC_INDEX ARRAY", ++curr_index);
                desc_array( ptr, TRUE );
                break;
            case BYTE_RANGE:
                StartType( "BYTE_RANGE", ++curr_index);
                range( ptr, 1 );
                break;
            case WORD_RANGE:
                StartType( "WORD_RANGE", ++curr_index);
                range( ptr, 2 );
                break;
            case LONG_RANGE:
                StartType( "LONG_RANGE", ++curr_index);
                range( ptr, 4 );
                break;
            case NEAR:
                StartType( "NEAR PTR", ++curr_index);
                Wdputs( "       " );
                near_ptr( buff );
                break;
            case FAR:
                StartType( "FAR PTR", ++curr_index);
                Wdputs( "       " );
                base_type_index( ptr );
                break;
            case HUGE:
                StartType( "HUGE PTR", ++curr_index);
                Wdputs( "       " );
                base_type_index( ptr );
                break;
            case NEAR_DEREF:
                StartType( "NEAR_DEREF PTR", ++curr_index);
                Wdputs( "       " );
                near_ptr( buff );
                break;
            case FAR_DEREF:
                StartType( "FAR_DEREF PTR", ++curr_index);
                Wdputs( "       " );
                base_type_index( ptr );
                break;
            case HUGE_DEREF:
                StartType( "HUGE_DEREF PTR", ++curr_index);
                Wdputs( "       " );
                base_type_index( ptr );
                break;
            case NEAR386:
                StartType( "NEAR386 PTR", ++curr_index);
                Wdputs( "       " );
                near_ptr( buff );
                break;
            case FAR386:
                StartType( "FAR386 PTR", ++curr_index);
                Wdputs( "       " );
                base_type_index( ptr );
                break;
            case NEAR386_DEREF:
                StartType( "NEAR386_DEREF PTR", ++curr_index);
                Wdputs( "       " );
                near_ptr( buff );
                break;
            case FAR386_DEREF:
                StartType( "FAR386_DEREF PTR", ++curr_index);
                Wdputs( "\n       " );
                base_type_index( ptr );
                break;
            case CLIST:
                StartType( "ENUM_LIST", ++curr_index);
                Wdputs( "          number of consts = " );
                Puthex( *ptr, 4 );
                Wdputs( "   scalar type = " );
                scalar_type( buff[4] );
                Wdputslc( "\n" );
                break;
            case CONST_BYTE:
                Wdputslc( "CONST_BYTE\n" );
                enum_const( buff, 1 );
                break;
            case CONST_WORD:
                Wdputslc( "CONST_WORD\n" );
                enum_const( buff, 2 );
                break;
            case CONST_LONG:
                Wdputslc( "CONST_LONG\n" );
                enum_const( buff, 4 );
                break;
            case FLIST:
                StartType( "FIELD_LIST", ++curr_index);
                Wdputs( "          number of fields = " );
                Puthex( *ptr, 4 );
                if( buff[0] > 4 ) {
                    Wdputs( "   size = " );
                    ptr += 2;
                    Puthex( *ptr, 8 );
                }
                Wdputslc( "\n" );
                break;
            case FIELD_BYTE:
                Wdputslc( "FIELD_BYTE\n" );
                bit_field_struct( buff, 1, FALSE );
                break;
            case FIELD_WORD:
                Wdputslc( "FIELD_WORD\n" );
                bit_field_struct( buff, 2, FALSE );
                break;
            case FIELD_LONG:
                Wdputslc( "FIELD_LONG\n" );
                bit_field_struct( buff, 4, FALSE );
                break;
            case BIT_BYTE:
                Wdputslc( "BIT_BYTE\n" );
                bit_field_struct( buff, 1, TRUE );
                break;
            case BIT_WORD:
                Wdputslc( "BIT_WORD\n" );
                bit_field_struct( buff, 2, TRUE );
                break;
            case BIT_LONG:
                Wdputslc( "BIT_LONG\n" );
                bit_field_struct( buff, 4, TRUE );
                break;
            case FIELD_CLASS:
                Wdputslc( "FIELD_CLASS\n" );
                bit_field_class( buff, FALSE );
                break;
            case BIT_CLASS:
                Wdputslc( "BIT_CLASS\n" );
                bit_field_class( buff, TRUE );
                break;
            case INHERIT_CLASS:
                Wdputslc( "INHERIT_CLASS\n" );
                Wdputs( "          adjust locator = " );
                ptr = Dump_location_expression( ptr, "            " );
                Wdputs( "          ancestor type = " );
                Get_type_index( ptr, &index );
                Putdec( index );
                Wdputslc( "\n" );
                break;
            case PNEAR:
                StartType( "NEAR PROC", ++curr_index);
                near_far_proc( buff );
                break;
            case PFAR:
                StartType( "FAR PROC", ++curr_index);
                near_far_proc( buff );
                break;
            case PNEAR386:
                StartType( "NEAR386 PROC", ++curr_index);
                near_far_proc( buff );
                break;
            case PFAR386:
                StartType( "FAR386 PROC", ++curr_index);
                near_far_proc( buff );
                break;
            case EXT_PARMS:
                Wdputslc( "EXT_PARMS\n" );
                param_type_index( (unsigned_8)buff[0]-2, ptr );
                break;
            case CHAR_BYTE:
                StartType( "CHAR_BYTE", ++curr_index);
                Wdputs( "        length = " );
                Puthex( *ptr, 2 );
                Wdputslc( "\n" );
                break;
            case CHAR_WORD:
                StartType( "CHAR_WORD", ++curr_index);
                Wdputs( "        length = " );
                Puthex( *ptr, 4 );
                Wdputslc( "\n" );
                break;
            case CHAR_LONG:
                StartType( "CHAR_LONG", ++curr_index);
                Wdputs( "        length = " );
                Puthex( *ptr, 8 );
                Wdputslc( "\n" );
                break;
            case CHAR_IND:
                StartType( "CHAR_IND", ++curr_index);
                Wdputs( "       scalar type = " );
                scalar_type( buff[2] );
                p32 = (addr32_ptr *)ptr;
                Puthex( p32->segment, 4 );
                Wdputc( ':' );
                Puthex( p32->offset, 4 );
                Wdputslc( "\n" );
                break;
            case CHAR_IND_386:
                StartType( "CHAR_IND_386", ++curr_index);
                Wdputs( "       scalar type = " );
                scalar_type( buff[2] );
                p48 = (addr48_ptr *)ptr;
                Puthex( p48->segment, 4 );
                Wdputc( ':' );
                Puthex( p48->offset, 8 );
                Wdputslc( "\n" );
                break;
            case CHAR_LOCATION:
                StartType( "CHAR_LOC", ++curr_index);
                Wdputs( "       scalar type = " );
                scalar_type( buff[2] );
                Wdputs( "          size locator = " );
                ptr = Dump_location_expression( ptr + 1, "            " );
                Wdputslc( "\n" );
                break;
            }
            coff += buff[0];
            if( coff >= (offs[i+1] - offs[i]) ) {
                break;
            }
        }
    }

} /* Dmp_type */
