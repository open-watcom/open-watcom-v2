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

#include <dwarf.h>
#include <stdio.h>

#include "drmisc.h"

// Project includes -------------------------------------------------------

#include "mbrfile.h"
#include "errors.h"

MBRFile::MBRFile( char * filename )
/*********************************/
    : ElfFile( filename )
{
    if( !initSections() ) {
        throw Invalid;
    }

    for( int i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        _sectionData[ i ] = NULL;
    }
}

MBRFile::~MBRFile()
/*****************/
{
    for( int i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        if( _sectionData[ i ] != NULL ) {
            delete [] _sectionData[ i ];
        }
    }
}

void MBRFile::getSectionRawData( dr_section section, uint_8 * &pData,
                                 unsigned long * len )
/**************************************************************/
{
    uint_32 *   sizes;

    sizes = getDRSizes();

    if( section < DR_DEBUG_NUM_SECTS ) {
        if( _sectionData[ section ] == NULL ) {
            _sectionData[ section ] = new uint_8[ sizes[ section ]];
            if( !_sectionData[ section ] ) {
                throw ErrOutOfMemory;
            }
            seekSect( section, 0 );
            readSect( section, _sectionData[ section ], sizes[ section ] );
        }
        pData = _sectionData[ section ];
        if( len ) {
            *len = sizes[ section ];
        }
    }
}

#if 0
void MBRFile::getSectionFilePtr( dr_section section, MFile * file,
                                 unsigned long * len )
/****************************************************************/
{
    if( section < DR_DEBUG_NUM_SECTS ) {
        _file->seek( _sections[ section ]);

        file = _file;
        if( len != NULL ) {
            *len = _sectsizes[ section ];
        }
    } else {
        file = NULL;
        if( len != NULL ) {
            *len = 0;
        }
    }
}
#endif

void MBRFile::scanAbbreviations( CBAbbrev cb, void * data )
/*********************************************************/
// Call cb with :
//   - abbreviation code
//   - abbreviation data (including 0/0 att/form at end)
//   - length of abbreviation data
//   - caller-passed data

{
    uint_8 *pAbbrevs;

    getSectionRawData( DR_DEBUG_ABBREV, pAbbrevs );

    const uint_8 *p;
    const uint_8 *start;
    uint_32     code;
    uint_32     tmp;
    uint_32     attr;
    uint_32 *   sizes;

    sizes = getDRSizes();
    p = pAbbrevs;
    for(;;) {

        start = p;

        if( p > pAbbrevs + sizes[ DR_DEBUG_ABBREV ]) break;

        p = DecodeULEB128( p, &code );

        if( p >= pAbbrevs + sizes[ DR_DEBUG_ABBREV ]) break;

        p = DecodeULEB128( p, &tmp );

        p++;

        for(;;) {
            if( p > pAbbrevs + sizes[ DR_DEBUG_ABBREV ]) break;

            p = DecodeULEB128( p, &attr );

            if( p > pAbbrevs + sizes[ DR_DEBUG_ABBREV ]) break;

            p = DecodeULEB128( p, &tmp );

            if( attr == 0 ) break;
        }

        // length of abbrev = p - start
        //
        if( !cb( code, start, p - start, data )) {
            break;
        }
    }
}

void MBRFile::getLineInfo( LinePrologue & pro, unsigned long & length )
/*********************************************************************/
// Returns :
//      Prologue information for this MBRFile.
//      A file handle which points to the start of the statement program.
//      Length of .debug_line info *after* prologue.
//
// NB: it would be nice if we could add some type of locking to MFile to
//     guarantee that the caller doesn't try to access outside the statement
//     program
{
    uint_8 * data;
    uint_8 * current;
    int i;

    seekSect( DR_DEBUG_LINE, 0 );
    readSect( DR_DEBUG_LINE, &pro.unit_length, sizeof( pro.unit_length ));

    readSect( DR_DEBUG_LINE, &pro.version, sizeof( pro.version ));
    readSect( DR_DEBUG_LINE, &pro.prologue_length,
                sizeof( pro.prologue_length ));

    data = new uint_8[ pro.prologue_length ];
    if( data == NULL ) {
        throw ErrOutOfMemory;
    }

    readSect( DR_DEBUG_LINE, data, pro.prologue_length );

    length = pro.unit_length - pro.prologue_length - sizeof( pro.unit_length )
             - sizeof( pro.version ) - sizeof( pro.prologue_length );

    current = data;

    pro.min_instr = *(( uint_8 * ) current );
    current += sizeof( pro.min_instr );

    pro.default_is_stmt = *(( uint_8 * ) current );
    current += sizeof( pro.default_is_stmt );

    pro.line_base = *(( int_8 * ) current );
    current += sizeof( pro.line_base );

    pro.line_range = *(( uint_8 * ) current );
    current += sizeof( pro.line_range );

    pro.opcode_base = *(( uint_8 * ) current );
    current += sizeof( pro.opcode_base );

    pro.opcode_lengths = new uint_8[ pro.opcode_base ];
    if( pro.opcode_lengths == NULL ) {
        throw ErrOutOfMemory;
    }

    for( i = 0; i < pro.opcode_base - 1; ++i ) {
        pro.opcode_lengths[ i ] = *(( uint_8 * ) current);
        current += sizeof( uint_8 );
    }

#if 0
    pos = _file->tell() - ;

    if( pos >= length ) {
        ;                               // Do ??
    }
#endif

    /*
     * Get the directories
     */


    for( ;; ) {
        char * strTmp;
        DirInfo * dir;

        current = readString( current, &strTmp );

        if( *strTmp == '\0' ) {
            delete [] strTmp;
            break;
        }
        dir = new DirInfo ( strTmp );
        if( dir == NULL ) {
            throw ErrOutOfMemory;
        }

        pro.directories.insert( dir );
    }

    /*
     * Get the filenames
     */

    for( ;; ) {
        FileInfo * p;

        current = readFileInfo( current, p );

        if( p == NULL ) {
            break;
        }

        pro.filenames.insert( p );
    }

    length = pro.unit_length - getSectOff( DR_DEBUG_LINE );

    delete [] data;
}

uint_8 * MBRFile::readFileInfo( uint_8 * data,
                                    FileInfo * & filename )
/*************************************************************/
{
    char * str;
    uint_32 lastModified;
    uint_32 dirIndex;
    uint_32 length;

    data = readString( data, &str );

    if( *str == '\0' ) {
        delete [] str;
        filename = NULL;
        return data;
    }

    data = DecodeULEB128( data, &dirIndex );
    data = DecodeULEB128( data, &lastModified );
    data = DecodeULEB128( data, &length );

    filename = new FileInfo ( str, dirIndex, lastModified, length );
    if( filename == NULL ) {
        throw ErrOutOfMemory;
    }

    return data;
}

uint_32 MBRFile::readULEB128( dr_section sect )
/*********************************************/
// Returns the ULEB128 value
{
    uint_32     result;
    uint        shift;
    uint_8      b;

    result = 0;
    shift = 0;
    for(;;) {
        readSect( sect, &b, sizeof( uint_8 ) );
        result |= ( b & 0x7f ) << shift;
        if( ( b & 0x80 ) == 0 ) break;
        shift += 7;
    }
    return( result );
}
