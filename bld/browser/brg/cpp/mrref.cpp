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
* Description:  Merge references.
*
****************************************************************************/


#include <wstd.h>
#include <dwarf.h>
#include <wcvector.h>

#include "mroffset.h"
#include "mrdie.h"
#include "mrinfo.h"
#include "mrline.h"
#include "mrref.h"
#include "mrreloc.h"
#include "mrfile.h"

#if INSTRUMENTS
#include <stdio.h>
#endif

void RefLineCol::codeBase( uint_8 opcode )
//----------------------------------------
{
    uint ld;

    opcode -= REF_CODE_BASE;

    ld = opcode / REF_COLUMN_RANGE;
    if( ld ) {
        line += ld;
        col = 0;
    }
    col += opcode % REF_COLUMN_RANGE;
}

uint_8 RefLineCol::makeCodeBase( uint_32 l, uint_32 c )
//-----------------------------------------------------
{
    uint_8 opcode;

    if( l == line ) {
        opcode = (uint_8) (c - col);
    } else {
        opcode = (uint_8) ( (l - line) * REF_COLUMN_RANGE );
        opcode += c;
    }

    line = l;
    col = c;

    return( (uint_8) (REF_CODE_BASE + opcode) );
}

MergeRefSection::MergeRefSection( MergeLineSection& line,
                                  MergeInfoSection& info, MergeFile * outFile )
                : _line( line )
                , _info( info )
                , _outFile( outFile )
//-----------------------------------------------------------------------------
{
}

void MergeRefSection::mergeRefs( WCPtrOrderedVector<MergeFile>& files )
//---------------------------------------------------------------------
{
    uint_8      i;
    uint_32 *   drSizes;
    uint_32     totLen = 0;
    uint_32     offset;
    RefLineCol  lnCol;

    _outFile->startWriteSect( DR_DEBUG_REF );

    _outFile->writeDWord( 1 );      // reserve a dword for length

    for( i = 0; i < files.entries(); i += 1 ) {
        drSizes = files[ i ]->getDRSizes();

        #if INSTRUMENTS
        Log.printf( "    File %s - %ld bytes\n", files[i]->getFileName(), drSizes[ DR_DEBUG_REF ] );
        totLen += drSizes[ DR_DEBUG_REF ];
        #endif

        scanFile( files[ i ], i, lnCol );
    }

    offset = _outFile->tell( DR_DEBUG_REF );
    _outFile->seek( DR_DEBUG_REF, 0 );
    _outFile->writeDWord( offset - sizeof(uint_32) );

    #if INSTRUMENTS
        Log.printf( "    %d files, %ld bytes\n", i, totLen );
    #endif

    _outFile->endWriteSect();
}

void MergeRefSection::scanFile( MergeFile * file, uint_8 indx,
                                RefLineCol & absLnCol )
//------------------------------------------------------------
{
    uint_32     unitLength;     // length of ref info for this compunit
    uint_32     user;           // referencing die
    uint_32     dependant;      // referenced die
    uint_32     fileIdx;        // containing file
    int_32      linecoldelta;   // line / column delta
    uint_32     linecol;        // absolute line / column
    uint_8      opcode;         // state-machine op-code
    uint_32     offset = 0;     // offset into the ref-section for this file
    MergeDIE *  die;            // the user / dependent die
    RefLineCol  currLnCol;      // registers if this is the only file
    bool        replaced;       // true if the scope is dead
    MergeOffset keyOff(indx,0); // key to search in reloc table

    unitLength = file->readDWord( DR_DEBUG_REF, offset );

    while( offset < unitLength + sizeof(uint_32) ) {
        opcode = file->readByte( DR_DEBUG_REF, offset );

        switch( opcode ) {
        case REF_BEGIN_SCOPE:
            user = file->readDWord( DR_DEBUG_REF, offset );

            keyOff.offset = user;
            die = _info.getReloc().getReloc( keyOff );

            #if INSTRUMENTS
            if( die == NULL ) {
                Log.printf( "Could not find user DIE <Target: %hd, source: %#lx>!\n", indx, user );
            }
            #endif

            InfoAssert( die != NULL );

            replaced = (die->offset().offset != keyOff.offset);

            if( replaced ) {
                skipDeadScope( file, offset, unitLength + sizeof(uint_32),
                                currLnCol );
            } else {
                _outFile->writeByte( opcode );
                _outFile->writeDWord( die->getNewOff() );
            }
            break;

        case REF_SET_FILE:
            _outFile->writeByte( opcode );
            fileIdx = file->readULEB128( DR_DEBUG_REF, offset );
            fileIdx = _line.getNewFileIdx( indx, fileIdx );
            _outFile->writeULEB128( fileIdx );
            break;

        case REF_SET_LINE:
            _outFile->writeByte( opcode );
            linecol = file->readULEB128( DR_DEBUG_REF, offset );
            _outFile->writeULEB128( linecol );

            absLnCol.setLine( linecol );
            currLnCol.setLine( linecol );
            break;

        case REF_SET_COLUMN:
            _outFile->writeByte( opcode );
            linecol = file->readULEB128( DR_DEBUG_REF, offset );
            _outFile->writeULEB128( linecol );

            absLnCol.setColumn( linecol );
            currLnCol.setColumn( linecol );
            break;

        case REF_ADD_LINE:
            _outFile->writeByte( opcode );
            linecoldelta = file->readSLEB128( DR_DEBUG_REF, offset );
            currLnCol.addLine( linecoldelta );
            linecoldelta = currLnCol.line - absLnCol.line;
            absLnCol.addLine( linecoldelta );
            _outFile->writeSLEB128( linecoldelta );
            break;

        case REF_ADD_COLUMN:
            _outFile->writeByte( opcode );
            linecoldelta = file->readSLEB128( DR_DEBUG_REF, offset );
            currLnCol.addColumn( linecoldelta );
            linecoldelta = currLnCol.col - absLnCol.col;
            absLnCol.addColumn( linecoldelta );
            _outFile->writeSLEB128( linecoldelta );
            break;

        case REF_COPY:
        case REF_END_SCOPE:
            _outFile->writeByte( opcode );
            break;

        case REF_CODE_BASE:
        default:
            /* special opcode */

            dependant = file->readDWord( DR_DEBUG_REF, offset );

            keyOff.offset = dependant;

            die = _info.getReloc().getReloc( keyOff );

#if 0
            if( die == NULL ) {
                die = _info.find( MergeOffset( indx, dependant ) );
            }
#endif

            #if INSTRUMENTS
            if( die == NULL ) {
                Log.printf( "Could not find dependant die <Target: %hd, dependent: %#lx>!\n", indx, dependant );
                Log.printf( "   user == <Target: %hd, source: %#lx>!", indx, user );

                MergeDIE *  usedie;
                usedie = _info.getReloc().getReloc( MergeOffset( indx, user ) );
#if 0
                if( !usedie ) {
                    usedie = _info.find( MergeOffset( indx, user ) );
                }
#endif
                if( usedie ) {
                    Log.printf( " %s\n", (const char *)(*usedie) );
                } else {
                    Log.printf( "\n" );
                }

                fprintf( stderr, "Could not find dependant die!\n" );
                continue;
            }
            #endif

            InfoAssert( die != NULL );

            currLnCol.codeBase( opcode );
            if( currLnCol.line < absLnCol.line
                || (currLnCol.line - absLnCol.line) >
                    (255 - REF_CODE_BASE) / REF_COLUMN_RANGE ) {

                absLnCol.setLine( currLnCol.line );
                _outFile->writeByte( REF_SET_LINE );
                _outFile->writeULEB128( currLnCol.line );
            }

            if( currLnCol.line == absLnCol.line ) {
                if( (currLnCol.col < absLnCol.col)
                    || (currLnCol.col - absLnCol.col) >= REF_COLUMN_RANGE ) {
                    absLnCol.setColumn( currLnCol.col );
                    _outFile->writeByte( REF_SET_COLUMN );
                    _outFile->writeULEB128( currLnCol.col );
                }
            } else {
                if( (currLnCol.col) >= REF_COLUMN_RANGE ) {
                    absLnCol.setColumn( currLnCol.col );
                    _outFile->writeByte( REF_SET_COLUMN );
                    _outFile->writeULEB128( currLnCol.col );
                }
            }

            opcode = absLnCol.makeCodeBase( currLnCol.line, currLnCol.col );
            _outFile->writeByte( opcode );
            _outFile->writeDWord( die->getNewOff() );
        }
    }
}

void MergeRefSection::skipDeadScope( MergeFile * file,
                                     uint_32 & off, uint_32 maxOff,
                                     RefLineCol & currLnCol )
//-----------------------------------------------------------------
{
    int     scopeLevel = 1;
    uint_8  opcode;
    uint_32 linecol;
    int_32  delta;

    #if INSTRUMENTS
    uint_32 startOff = off;
    #endif

    while( scopeLevel > 0 && off < maxOff ) {
        opcode = file->readByte( DR_DEBUG_REF, off );
        switch( opcode ) {
        case REF_BEGIN_SCOPE:
            file->readDWord( DR_DEBUG_REF, off );
            scopeLevel += 1;
            break;

        case REF_SET_LINE:
            linecol = file->readULEB128( DR_DEBUG_REF, off );
            currLnCol.setLine( linecol );
            break;

        case REF_SET_COLUMN:
            linecol = file->readULEB128( DR_DEBUG_REF, off );
            currLnCol.setColumn( linecol );
            break;

        case REF_SET_FILE:
            file->readULEB128( DR_DEBUG_REF, off );
            break;

        case REF_ADD_LINE:
            delta = file->readSLEB128( DR_DEBUG_REF, off );
            currLnCol.addLine( delta );
            break;

        case REF_ADD_COLUMN:
            delta = file->readSLEB128( DR_DEBUG_REF, off );
            currLnCol.addColumn( delta );
            break;

        case REF_END_SCOPE:
            scopeLevel -= 1;
            break;

        case REF_COPY:
            /* only one byte opcode, already skipped */
            break;
        case REF_CODE_BASE:
        default:
            currLnCol.codeBase( opcode );
            file->readDWord( DR_DEBUG_REF, off );
            break;
        }
    }

    #if INSTRUMENTS
    if( scopeLevel != 0 ) {
        Log.printf( "Tried to skip past the end of ref section -- level is %d\n", scopeLevel );
        Log.printf( "  I was _trying_ to skip from %lx, but I hit %lx!\n", startOff, maxOff );
    }
    #endif

    InfoAssert( scopeLevel == 0 );      // ie we didn't hit EOF
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9

MergeRefSection::~MergeRefSection()
//--------------------------------
{
}
