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


#include <wstd.h>
#include <dwarf.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <io.h>

#include "util.h"
#include "mrfile.h"
#include "chbffile.h"

static char MergeFile::_buffer[ MERGEFILESTRBUF ];

MergeFile::MergeFile( const char * filename )
//-------------------------------------
    : ElfFile( filename, TRUE )
{
}

uint_32 MergeFile::readULEB128( dr_section sect, uint_32& off )
//-------------------------------------------------------------
// Returns the ULEB128 value, updates off
{
    uint_32     result = 0;
    uint        shift = 0;
    uint_8      b;
    int         i;

    for( i = 0; ; i += 1) {
        b = readByte( sect, off );

        result |= ( b & 0x7f ) << shift;
        if( ( b & 0x80 ) == 0 ) break;
        shift += 7;
    }
    return( result );
}
int_32 MergeFile::readSLEB128( dr_section sect, uint_32& off )
//-------------------------------------------------------------
{
    int_32      result = 0;
    uint        shift = 0;
    uint_8      b;
    int         i;

    for( i = 0; ; i += 1) {
        b = readByte( sect, off );

        result |= ( b & 0x7f ) << shift;
        if( ( b & 0x80 ) == 0 ) break;
        shift += 7;
    }

    if( b & 0x40 ) {    // we have to sign extend
        result |= - ((signed_32)(1 << (shift + 7)));
    }
    return( result );
}

uint_16 MergeFile::readWord( dr_section sect, uint_32& off )
//----------------------------------------------------------
// reads word, updates off
{
    int_16 result;
    readBlock( sect, off, &result, sizeof( int_16 ) );
    return result;
}

uint_32 MergeFile::readDWord( dr_section sect, uint_32& off )
//-----------------------------------------------------------
// reads double word, updates off
{
    int_32 result;
    readBlock( sect, off, &result, sizeof( int_32 ) );
    return result;
}

const char * MergeFile::readString( dr_section sect, uint_32& off )
//-----------------------------------------------------------------
{
    int         i;
    uint_8      b;

    for( i = 0; ; i += 1 ) {
        b = readByte( sect, off );
        _buffer[ i ] = b;
        if( b == '\0' || i >= MERGEFILESTRBUF ) break;
    }

    InternalAssert( i < MERGEFILESTRBUF );      // NYI -- throw or truncate?

    if( _buffer[ 0 ] == 0 ) {
        return NULL;            // <-------- early return for empty string
    }

    return _buffer;
}

void MergeFile::skipForm( dr_section sect, uint_32& off, uint_32 form,
                            uint_8 addrSize )
//---------------------------------------------------------------------
{
    uint_32 value;

    switch( form ) {
    case DW_FORM_addr:
        off += addrSize;
        break;
    case DW_FORM_block1:
        off += readByte( sect, off );
        break;
    case DW_FORM_block2:
        off += readWord( sect, off );
        break;
    case DW_FORM_block4:
        off += readDWord( sect, off );
        break;
    case DW_FORM_block:
        value = readULEB128( sect, off );
        off += value;
        break;
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_ref1:
        off += 1;
        break;
    case DW_FORM_data2:
    case DW_FORM_ref2:
        off += 2;
        break;
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for form_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        off += 4;
        break;
    case DW_FORM_data8:
        off += 8;
        break;
    case DW_FORM_sdata:
        readSLEB128( sect, off );
        break;
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        readULEB128( sect, off );
        break;
    case DW_FORM_string:
        while( readByte( sect, off ) != 0 );
        break;
    case DW_FORM_indirect:
        value = readULEB128( sect, off );
        skipForm( sect, off, value, addrSize );
        break;
    default:
        #if DEBUG
        printf( "ACK -- form %#x\n", form );
        #endif
        InternalAssert( 0 );
    }
}

void MergeFile::copyFormTo( MergeFile& out, dr_section sect,
                            uint_32& off, uint_32 form, uint_8 addrSize )
//-----------------------------------------------------------------------
{
    uint_32 num;
    char *  buffer;
    uint_32 bufLen;
    uint_8  buf8[ 8 ];

    switch( form ) {
            /* do all simple numeric forms */
    case DW_FORM_addr:
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_ref1:
    case DW_FORM_data2:
    case DW_FORM_ref2:
    case DW_FORM_data4:
    case DW_FORM_ref4:
    case DW_FORM_sdata:
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
    case DW_FORM_ref_addr:
        num = readForm( sect, off, form, addrSize );
        out.writeForm( form, num, addrSize );
        break;
    case DW_FORM_block1:
        bufLen = readByte( sect, off );
        out.writeByte( (uint_8) bufLen );
        if( bufLen ) {
            buffer = new char[ bufLen ];
            readBlock( sect, off, buffer, bufLen );
            out.writeBlock( buffer, bufLen );
            delete [] buffer;
        }
        break;
    case DW_FORM_block2:
        bufLen = readWord( sect, off );
        out.writeWord( (uint_16) bufLen );
        if( bufLen ) {
            buffer = new char[ bufLen ];
            readBlock( sect, off, buffer, bufLen );
            out.writeBlock( buffer, bufLen );
            delete [] buffer;
        }
        break;
    case DW_FORM_block4:
        bufLen = readDWord( sect, off );
        out.writeDWord( bufLen );
        if( bufLen ) {
            buffer = new char[ bufLen ];
            readBlock( sect, off, buffer, bufLen );
            out.writeBlock( buffer, bufLen );
            delete [] buffer;
        }
        break;
    case DW_FORM_block:
        bufLen = readULEB128( sect, off );
        if( bufLen ) {
            buffer = new char[ bufLen ];
            readBlock( sect, off, buffer, bufLen );
            out.writeBlock( buffer, bufLen );
            delete [] buffer;
        }
        break;
    case DW_FORM_data8:
        readBlock( sect, off, buf8, 8 );
        out.writeBlock( buf8, 8 );
        break;
    case DW_FORM_string:
        do {
            num = readByte( sect, off );
            out.writeByte( (uint_8) num );
        } while( num != 0 );
        break;
    case DW_FORM_indirect:
        num = readULEB128( sect, off );
        out.writeULEB128( num );
        copyFormTo( out, sect, off, num, addrSize );
        break;
    default:
        #if DEBUG
        printf( "ACK -- form %#x\n", form );
        #endif
        InternalAssert( 0 );
    }
}

void MergeFile::writeWord( uint_16 w )
//------------------------------------
{
    writeBlock( &w, sizeof( uint_16 ) );
}

void MergeFile::writeDWord( uint_32 dw )
//--------------------------------------
{
    writeBlock( &dw, sizeof( uint_32 ) );
}

void MergeFile::writeULEB128( uint_32 uleb )
//------------------------------------------
{
    const int   MaxULEBLen = 5;
    uint_8      buffer[ MaxULEBLen ];
    uint_8 *    buf = buffer;
    uint_8      b;

    for(;;) {
        b = (uint_8) (uleb & 0x7f);
        uleb >>= 7;
        if( uleb == 0 ) break;
        *buf++ = (uint_8) (b | 0x80);
    }
    *buf++ = b;

    writeBlock( buffer, buf - buffer );
}

void MergeFile::writeSLEB128( int_32 sleb )
//------------------------------------------
{
    const int   MaxULEBLen = 5;
    uint_8      buffer[ MaxULEBLen ];
    uint_8 *    buf = buffer;
    uint_8      b;

    if( sleb >= 0 ) {           // we can only handle an arithmetic right shift
        for(;;) {
            b = (uint_8) (sleb & 0x7f);
            sleb >>= 7;
            if( sleb == 0 && ( b & 0x40 ) == 0 ) break;
            *buf++ = (uint_8) (b | 0x80);
        }
    } else {
        for(;;) {
            b = (uint_8) (sleb & 0x7f);
            sleb >>= 7;
            if( sleb == -1 && ( b & 0x40 ) ) break;
            *buf++ = (uint_8) (b | 0x80);

            #if INSTRUMENTS
                if( (buf - buffer) > MaxULEBLen ) {
                    Log.printf( "** WriteSLEB128 buffer overrun! **\n" );
                    InternalAssert( 0 );
                }
            #endif
        }
    }
    *buf++ = b;

    writeBlock( buffer, buf - buffer );
}

static uint MergeFile::ULEB128Len( uint_32 uleb )
//-----------------------------------------------
{
    uint_8  len = 0;
    uint_8  b;

    do {
        b = (uint_8) (uleb & 0x7f);
        uleb >>= 7;
        len += 1;
    } while( uleb );

    return len;
}

void MergeFile::writeString( const char * str )
//---------------------------------------------
{
    uint_32 len = strlen( str ) + 1;
    writeBlock( (const void *) str, len );
}

void MergeFile::writeForm( uint_32 form, uint_32 num, uint_8 addrSize )
//---------------------------------------------------------------------
{
    switch( form ) {
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_ref1:
        writeByte( (uint_8) num );
        break;
    case DW_FORM_data2:
    case DW_FORM_ref2:
        writeWord( (uint_16) num );
        break;
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for form_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        writeDWord( num );
        break;
    case DW_FORM_sdata:
        writeSLEB128( num );
        break;
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        writeULEB128( num );
        break;
    case DW_FORM_addr:
        switch( addrSize ) {
        case 2:
            writeWord( (uint_16) num );
            break;
        case 4:
            writeDWord( num );
            break;
        default:
            InternalAssert( 0 /* can't handle address size not 2 or 4 */ );
        }
        break;
    default:
        InternalAssert( 0 /* not a numeric type */ );
    }
}

uint_32 MergeFile::readForm( dr_section sect, uint_32& off, uint_32 form,
                             uint_8 addrSize )
//-----------------------------------------------------------------------
{
    switch( form ) {
    case DW_FORM_flag:
    case DW_FORM_data1:
    case DW_FORM_ref1:
        return readByte( sect, off );
    case DW_FORM_data2:
    case DW_FORM_ref2:
        return readWord( sect, off );
    case DW_FORM_ref_addr:      // NYI: non-standard behaviour for form_ref
    case DW_FORM_data4:
    case DW_FORM_ref4:
        return readDWord( sect, off );
    case DW_FORM_sdata:
        return readSLEB128( sect, off );
    case DW_FORM_udata:
    case DW_FORM_ref_udata:
        return readULEB128( sect, off );
    case DW_FORM_addr:
        switch( addrSize ) {
        case 2:
            return readWord( sect, off );
        case 4:
            return  readDWord( sect, off );
        default:
            InternalAssert( 0 /* can't handle address size not 2 or 4 */ );
        }
        break;
    default:
        InternalAssert( 0 /* not a numeric type */ );
    }
    return 0;
}


// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9

MergeFile::~MergeFile()
//---------------------
{
}
