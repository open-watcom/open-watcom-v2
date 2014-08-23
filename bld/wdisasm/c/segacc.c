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


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "disasm.h"

#define ADDR_BITS               32
#define PAGE_OFFSET_BITS        8
#define PAGE_TABLE_BITS         4
#define NUM_TABLES              ((ADDR_BITS-PAGE_OFFSET_BITS)/PAGE_TABLE_BITS)

static uint_8 *Access( uint_32 off, bool create )
{
    void        **table;
    void        *entry;
    unsigned    shift;
    unsigned    i;

    table = (void **)Segment->data;
    if( table == NULL ) {
        if( create ) {
            table = AllocNull( (1 << PAGE_TABLE_BITS) * sizeof( table ) );
            Segment->data = table;
        } else {
            return( NULL );
        }
    }
    shift = ADDR_BITS-PAGE_TABLE_BITS;
    for( i = 0; i < NUM_TABLES; ++i ) {
        table = &table[ ((unsigned)(off>>shift)) & ((1<<PAGE_TABLE_BITS)-1) ];
        entry = *table;
        if( entry == NULL ) {
            if( create ) {
                if( i == NUM_TABLES-1 ) {
                    entry = AllocNull( (1 << PAGE_OFFSET_BITS) );
                } else {
                    entry = AllocNull( (1<<PAGE_TABLE_BITS) * sizeof(table) );
                }
                *table = entry;
            } else {
                return( NULL );
            }
        }
        shift -= PAGE_TABLE_BITS;
        table = entry;
    }
    return( (uint_8 *)entry + ((unsigned)off & ((1<<PAGE_OFFSET_BITS) - 1)) );
}


uint_8 GetSegByte( uint_32 off )
{
    uint_8      *p;

    p = Access( off, false );
    if( p == NULL ) return( 0 );
    return( *p );
}


void PutSegByte( uint_32 off, uint_8 data )
{
    uint_8      *p;

    p = Access( off, data != 0 );
    if( p != NULL ) *p = data;
}


uint_16 GetSegWord( uint_32 off )
{
    unsigned    data;

    if( Segment->data == NULL ) return( 0 );
    data = GetSegByte( off );
    return( (GetSegByte( off + 1 ) << 8) + data );
}


uint_32 GetSegDWord( uint_32 off )
{
    unsigned    data;

    if( Segment->data == NULL ) return( 0 );
    data = GetSegWord( off );
    return( ((uint_32)GetSegWord( off + 2 ) << 16) + data );
}


void PutSegWord( uint_32 off, uint_16 data )
{
    PutSegByte( off + 0, data & 0xff );
    PutSegByte( off + 1, data >> 8 );
}


void PutSegDWord( uint_32 off, uint_32 data )
{
    PutSegByte( off + 0, data & 0xff );
    data >>= 8;
    PutSegByte( off + 1, data & 0xff );
    data >>= 8;
    PutSegByte( off + 2, data & 0xff );
    data >>= 8;
    PutSegByte( off + 3, data & 0xff );
}


void InitSegAccess( void )
/************************/
{
    Segment->curr = Segment->start;
}


uint_32 GetOffset( void )
/***********************/
{
    return( Segment->curr );
}


int_16  PeekDataByte( void )
/**************************/
{
    return( GetSegByte( Segment->curr ) );
}


int_16  GetDataByte( void )
/*************************/
{
    uint_16 value;

    value = 0;
    if( Segment->curr < Segment->size ) {
        value = (signed char) GetSegByte( Segment->curr++ );
        DataString[ DataBytes ] = value;
        ++DataBytes;
    }
    return( value );
}


int_16  GetDataWord( void )
/*************************/
{
    uint_16 byte1;
    uint_16 byte2;

    byte1 = (uint_8)GetDataByte();
    byte2 = (uint_8)GetDataByte();
    return( byte1 + ( byte2 << 8 ) );
}


int_32 GetDataLong( void )
/************************/
{
    uint_32 word1;
    uint_32 word2;

    word1 = (uint_16)GetDataWord();
    word2 = (uint_16)GetDataWord();
    return( word1 + ( word2 << 16 ) );
}


int_16  GetNextByte( void )
/*************************/
{
    int                 value;

    value = 0;
    if( Segment->curr < Segment->size ) {
        value = (signed char) GetSegByte( Segment->curr );
    }
    return( value );
}


char  EndOfSegment( void )
/************************/
{
    return( Segment->curr == Segment->size );
}
