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


// System includes --------------------------------------------------------

#include <watcom.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <dwarf.h>

// Project includes -------------------------------------------------------

extern void dumpMemory( uint_8 * data, unsigned long length, char * desc )
/************************************************************************/
{
    const int RowSize = 16;
    unsigned long pos;
    uint_8 * lineStart;

    printf( "\n[ %s ]\n", desc );
    lineStart = data;
    for( pos = 0; pos < length; pos += 1 ) {
        printf( "%02x ", data[ pos ]);

        if( pos % RowSize == 0 ) {
            for( int i = 0; i < RowSize; i += 1 ) {
                if( isprint( lineStart[ i ])) {
                    printf( "%c", lineStart[ i ]);
                } else {
                    printf( "." );
                }
            }
            printf( "\n" );
        }
    }
    printf( "\n" );
}

extern uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value )
/*****************************************************************/
{

    uint_32     result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    for(;;) {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        if( ( byte & 0x80 ) == 0 ) break;
        shift += 7;
    }
    *value = result;
    return( (uint_8 *)input );
}

#if 0
extern uint_8 *DecodeLEB128( const uint_8 *input, int_32 *value )
/***************************************************************/
{

    int_32      result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    for(;;) {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        shift += 7;
        if( ( byte & 0x80 ) == 0 ) break;
    }
    if( ( shift < 32 ) && ( byte & 0x40 ) ) {
        result |= - ( 1 << shift );
    }
    *value = result;
    return( (uint_8 *)input );
}
#endif

extern uint_8 *DecodeLEB128( const uint_8 *input, int_32 *value )
/***************************************************************/
{
    uint_32     result;
    uint_8      inbyte;
    unsigned    shift;

    shift = 0;
    inbyte = *input++;
    result = inbyte & 0x7F;

    while( inbyte & 0x80 ) {
        shift += 7;
        inbyte = *input++;
        result |= (uint_32)(inbyte & 0x7F) << shift;
    }

    if( inbyte & 0x40 ) {       // we have to sign extend
        result |= - ((int_32)(1 << (shift + 7)));
    }

    *value = result;

    return( (uint_8 *)input );
}

extern uint_8 *readString( const uint_8 *input, char ** pString )
/***************************************************************/
{
    int length;

    length = strlen( ( char const * )input );

    *pString = new char[ length + 1 ];
    memcpy( *pString, input, length + 1 );

    return ( uint_8 * )input + length + 1;
}

extern uint_8 *EncodeLEB128( uint_8 * buf, int_32 value )
/*******************************************************/
{
    uint_8                      byte;

    /* we can only handle an arithmetic right shift */
    if( value >= 0 ) {
        for(;;) {
            byte = value & 0x7f;
            value >>= 7;
            if( value == 0 && ( byte & 0x40 ) == 0 ) break;
            *buf++ = byte | 0x80;
        }
    } else {
        for(;;) {
            byte = value & 0x7f;
            value >>= 7;
            if( value == -1 && ( byte & 0x40 ) ) break;
            *buf++ = byte | 0x80;
        }
    }
    *buf++ = byte;
    return( buf );
}

extern uint_8 *EncodeULEB128( uint_8 * buf, uint_32 value )
/*********************************************************/
{
    uint_8                      byte;

    for(;;) {
        byte = value & 0x7f;
        value >>= 7;
        if( value == 0 ) break;
        *buf++ = byte | 0x80;
    }
    *buf++ = byte;
    return( buf );
}

extern uint_8 * SkipLEB128( uint_8 * p )
/**************************************/
// just advance the vm pointer past the leb128 (works on both signed & unsigned)
{
    while( *p & 0x80 ) {
        p++;
    }
    p++;
    return p;
}

struct GlobalParms {
    uint_8 addressSize;
};

static struct GlobalParms _globalParameters = { 4 };

extern void SetGlobalParms( uint_8 addressSize )
/**********************************************/
{
    _globalParameters.addressSize = addressSize;
}

extern uint_8 * SkipForm( uint_8 * p, uint_16 form )
/**************************************************/
{
    uint_32 ui32;

    switch( form ) {
        case DW_FORM_addr:
#ifdef __ADDR_IS_32
            p += sizeof( uint_32 );
            p += sizeof( uint_16 );
#else
            p += sizeof( uint_16 );
#endif
            break;
        case DW_FORM_block1:
            p += *p + sizeof(unsigned_8);
            break;
        case DW_FORM_block2:
            p += *p + sizeof(unsigned_16);
            break;
        case DW_FORM_block4:
            p += *p + sizeof(unsigned_32);
            break;
        case DW_FORM_block:
            p  = DecodeULEB128( p, &ui32 );
            p += ui32;
            break;
        case DW_FORM_flag:
        case DW_FORM_data1:
        case DW_FORM_ref1:
            p += 1;
            break;
        case DW_FORM_data2:
        case DW_FORM_ref2:
            p += 2;
            break;
        case DW_FORM_ref_addr:  // NYI: non-standard behaviour for form_ref
        case DW_FORM_data4:
        case DW_FORM_ref4:
            p += 4;
            break;
        case DW_FORM_data8:
            p += 8;
            break;
        case DW_FORM_sdata:
        case DW_FORM_udata:
        case DW_FORM_ref_udata:
            p = SkipLEB128( p );
            break;
        case DW_FORM_string:
            while( *p != 0 ) {
                p += 1;
            }
            p += 1;
            break;
        case DW_FORM_indirect:
            p = DecodeULEB128( p, &ui32 );
            p = SkipForm( p, ui32 );
            break;
    }

    return p;
}
