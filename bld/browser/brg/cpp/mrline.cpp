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


#include <wchash.h>
#include <wchiter.h>
#include <wcvector.h>
#include <wstd.h>
#include <dwarf.h>
#include <string.h>

#include "mroffset.h"
#include "mrline.h"
#include "mrfile.h"

#if INSTRUMENTS
#  include "dwmerger.h"
#endif

/*---------------------- MergeFileRelocKey ------------------------*/

unsigned MergeFileRelocKey::hashVal() const
//-----------------------------------------
{
    return (uint) mbrFile << ( ( sizeof(uint) - sizeof(uint_8) ) * 8) ^ oldIdx;
}

/*---------------------- MergeFileEntry ------------------------*/

MergeFileEntry::MergeFileEntry()
                : name( NULL )
                , directoryIdx( 0 )
                , modTime( 0 )
                , length( 0 )
//---------------------------------
{
}

MergeFileEntry::MergeFileEntry( const MergeFileEntry& other )
//-----------------------------------------------------------
                : name( other.name )
                , directoryIdx( other.directoryIdx )
                , modTime( other.modTime )
                , length( other.length )
{
}

bool MergeFileEntry::operator==( const MergeFileEntry& other ) const
//------------------------------------------------------------------
{
    return (name == other.name);
}

MergeFileEntry& MergeFileEntry::operator=( const MergeFileEntry& other )
//----------------------------------------------------------------------
{
    name.operator= ( other.name );
    directoryIdx =  other.directoryIdx;
    modTime =       other.modTime;
    length =        other.length;

    return *this;
}

uint MergeFileEntry::getLen()
//---------------------------
{
    return( strlen( name.getString() ) + 1
            + MergeFile::ULEB128Len( directoryIdx )
            + MergeFile::ULEB128Len( modTime )
            + MergeFile::ULEB128Len( length ) );
}

/*---------------------- MergeLineSection ------------------------*/


MergeLineSection::MergeLineSection()
//----------------------------------
{
    _directoriesByName = new WCValHashDict<MergeStringHdl,uint_16>(
                                        MergeStringHdl::getHashVal, 20 );

    _directoriesReloc = new WCValHashDict<MergeFileRelocKey,uint_16>(
                                        MergeFileRelocKey::getHashVal, 200 );

    _directories = new WCValOrderedVector<MergeStringHdl>( 20, 10 );

    _filesByName = new WCValHashDict<MergeStringHdl,uint_16>(
                                        MergeStringHdl::getHashVal, 200 );

    _filesReloc = new WCValHashDict<MergeFileRelocKey,uint_16>(
                                        MergeFileRelocKey::getHashVal, 500 );

    _files = new WCValOrderedVector<MergeFileEntry>( 200, 50 );
}

MergeLineSection::~MergeLineSection()
//-----------------------------------
{
    #if INSTRUMENTS
        Log.printf( "\nMergeLineSection\n----------------\n" );
        Log.printf( "MergeLineSection::_directoriesByName:  %u entries, loaded %%%3.2f\n", _directoriesByName->entries(), 100.0 * ((double)_directoriesByName->entries() / (double)_directoriesByName->buckets()) );
        Log.printf( "MergeLineSection::_directoriesReloc:   %u entries, loaded %%%3.2f\n", _directoriesReloc->entries(), 100.0 * ((double)_directoriesReloc->entries() / (double)_directoriesReloc->buckets()) );
        Log.printf( "MergeLineSection::_directories:        %u entries\n", _directories->entries() );
        Log.printf( "MergeLineSection::_filesByName:        %u entries, loaded %%%3.2f\n", _filesByName->entries(), 100.0 * ((double)_filesByName->entries() / (double)_filesByName->buckets()) );
        Log.printf( "MergeLineSection::_filesReloc:         %u entries, loaded %%%3.2f\n", _filesReloc->entries(), 100.0 * ((double)_filesReloc->entries() / (double)_filesReloc->buckets()) );
        Log.printf( "MergeLineSection::_files:              %u entries\n", _files->entries() );
    #endif

    delete _directoriesByName;
    delete _directoriesReloc;
    delete _directories;

    delete _filesByName;
    delete _filesReloc;
    delete _files;
}

#if 0 // no longer needed -- file by file
void MergeLineSection::mergeLine( WCPtrOrderedVector<MergeFile>& files )
//----------------------------------------------------------------------
{
    uint_8      i;
    uint_32 *   drSizes;
    uint_32     totLen = 0;

    for( i = 0; i < files.entries(); i += 1 ){
        drSizes = files[ i ]->getDRSizes();
        scanFile( files[ i ], i, drSizes[ DR_DEBUG_LINE ] );

        #if INSTRUMENTS
        Log.printf( "    File %s - %ld bytes\n", files[i]->getFileName(), drSizes[ DR_DEBUG_LINE ] );
        totLen += drSizes[ DR_DEBUG_LINE ];
        #endif
    }

    #if INSTRUMENTS
        Log.printf( "    %d files, %ld bytes\n", i, totLen );
    #endif

    _outFile->startWriteSect( DR_DEBUG_LINE );
    writePrologue();
    _outFile->endWriteSect();
}
#endif

void MergeLineSection::scanFile( MergeFile * file, uint_8 idx )
//-------------------------------------------------------------
{
    uint_32     len;
    MergeOffset moff( idx, 0 );

    len = file->getDRSizes()[ DR_DEBUG_LINE ];

    while( moff.offset < len ) {
        readLineSect( file, moff );
    }

    #if INSTRUMENTS
    Log.printf( "    %s .debug_line - %ld bytes\n", file->getFileName(), len );
    #endif
}
void MergeLineSection::readLineSect( MergeFile * file, MergeOffset& moff )
//------------------------------------------------------------------------
{
    MergeOffset         startOff = moff;// offset of start of line section
    uint_16             i;
    const char *        string;
    MergeStringHdl      name;
    uint_32             lineEnd;        // offset of end of compunit's line info

    uint_8      opcode;                 // standard opcode
    uint_32     oplen;
    uint_32     extopcode;

    uint_32     total_length;           // length of compunit's line information
    uint_16     version;
    uint_32     prologue_length;        // length of prologue for this compunit
    uint_8      op_base;                // number of first special opcode
    uint_8 *    standard_opcode_lengths;


    total_length =      file->readDWord( DR_DEBUG_LINE, moff.offset );
    version =           file->readWord( DR_DEBUG_LINE, moff.offset );
    prologue_length =   file->readDWord( DR_DEBUG_LINE, moff.offset );

    lineEnd = startOff.offset + sizeof(uint_32) + total_length;

    // skip minimum_instruction_length, default_is_stmt, line_base, line_range
    moff.offset += 1 + 1 + 1 + 1;

    op_base = file->readByte( DR_DEBUG_LINE, moff.offset );
    standard_opcode_lengths = new uint_8[ op_base ];

    for( i = 0; i < op_base - 1; i += 1 ) {
        standard_opcode_lengths[ i ] = file->readByte( DR_DEBUG_LINE,
                                                        moff.offset );
    }

    // directory indicies start at 1
    for( i = 1; moff.offset < lineEnd; i += 1 ) {
        string = file->readString( DR_DEBUG_LINE, moff.offset );

        if( string == NULL ) break;     // <------- end of directories
        name = string;

        addDirectory( moff.fileIdx, i, name );
    }

    for( i = 1; moff.offset < lineEnd; i += 1 ) {
        if( !readFileEntry( file, moff, i ) ) break;  // <----- end of files
    }

    while( moff.offset < lineEnd ) {
        opcode = file->readByte( DR_DEBUG_LINE, moff.offset );
        if( opcode == 0 ) {     // extended opcode
            oplen =     file->readULEB128( DR_DEBUG_LINE, moff.offset );
            extopcode = file->readULEB128( DR_DEBUG_LINE, moff.offset );

            if( extopcode == DW_LNE_define_file ) {
                readFileEntry( file, moff, i );
                i += 1;
            } else {
                moff.offset += oplen;   // skip unwanted extended opcode
            }
        } else {
            if( opcode < op_base ) {
                if( opcode == DW_LNS_fixed_advance_pc ) {
                    moff.offset += sizeof(unsigned_16); // it's a fixed size
                } else {                            // it's variable # of leb's
                    oplen = standard_opcode_lengths[opcode];
                    while( oplen > 0 ) {
                        file->readULEB128( DR_DEBUG_LINE, moff.offset ); // skip
                        oplen--;
                    }
                }
            } else {
                // it was a special opcode, only one byte long, so we
                // already skipped it
            }
        }
    }

    delete standard_opcode_lengths;
}

void MergeLineSection::addDirectory( uint_8 fileIdx, uint_16 oldDirIdx,
                                     MergeStringHdl & dir )
//---------------------------------------------------------------------
{
    uint_16 newDirIdx;
    MergeFileRelocKey relKey( fileIdx, oldDirIdx );

    // NYI -- if oldDirIdx == 0, then it means in the current compile
    // directory, but that will possibly be different accross compunits.

    InternalAssert( oldDirIdx != 0 );       // NYI throw

    if( _directoriesByName->contains( dir ) ) {
        newDirIdx = (*_directoriesByName)[ dir ];
    } else {
        newDirIdx = (uint_16)( _directories->entries() + 1 );
        _directories->append( dir );
        (*_directoriesByName)[ dir ] = newDirIdx;
    }
    (*_directoriesReloc)[ relKey ] = newDirIdx;
}

bool MergeLineSection::readFileEntry( MergeFile * file, MergeOffset & moff,
                                        uint_16 oldFileIdx )
//-------------------------------------------------------------------------
{
    const char *    string;
    MergeFileEntry  fileEntry;

    string = file->readString( DR_DEBUG_LINE, moff.offset );

    if( string == NULL ) return FALSE;  // end of files

    fileEntry.name.operator=( string );
    fileEntry.directoryIdx = (uint_16)  file->readULEB128( DR_DEBUG_LINE, moff.offset );
    fileEntry.modTime =         file->readULEB128( DR_DEBUG_LINE, moff.offset );
    fileEntry.length =          file->readULEB128( DR_DEBUG_LINE, moff.offset );

    addFile( moff.fileIdx, oldFileIdx, fileEntry );

    return TRUE;
}

void MergeLineSection::addFile( uint_8 fileIdx, uint_16 oldFileIdx,
                                MergeFileEntry& file )
//-----------------------------------------------------------------
{
    uint_16  newFileIdx;
    MergeFileRelocKey reloc( fileIdx, oldFileIdx );

    if( _filesByName->contains( file.name ) ) {
        newFileIdx = (*_filesByName)[ file.name ];
    } else {
        newFileIdx = (uint_16)( _files->entries() + 1 );

        if( file.directoryIdx != 0 ) {
            MergeFileRelocKey reloc( fileIdx, file.directoryIdx );
            file.directoryIdx = (*_directoriesReloc)[ reloc ];
        }
        _files->append( file );
        (*_filesByName)[ file.name ] = newFileIdx;
    }
    (*_filesReloc)[ reloc ] = newFileIdx;
}

uint_16 MergeLineSection::getNewFileIdx( uint_8 mbrFile, uint_16 oldIdx )
//-----------------------------------------------------------------------
{
    MergeFileRelocKey key( mbrFile, oldIdx );

    #if INSTRUMENTS
    if( !_filesReloc->contains( key ) ) {
        Log.printf( "Could not find a relocation record for <%d,%d>\n", mbrFile, oldIdx );
        Log.printf( "_fileReloc:\n" );
        WCValHashDictIter<MergeFileRelocKey,uint_16> iter( *_filesReloc );

        while( iter.operator++() ) {
            Log.printf( "<%hu,%u|%u>\n", iter.key().mbrFile, iter.key().oldIdx, iter.value() );
        }
        Log.printf( "_files:\n" );
        for( int i = 0; i < _files->entries(); i += 1 ) {
            Log.printf( "<%d|%s,%d>\n", i, (*_files)[i].name.getString(), (*_files)[i].directoryIdx );
        }
    }
    #endif

    InfoAssert( _filesReloc->contains( key ) );
    return (*_filesReloc)[ key ];
}

void MergeLineSection::writePass( MergeFile * outFile )
//-----------------------------------------------------
{
    uint_32 length;
    int     i;

    // total_length, version, prologue_length
    length = sizeof(uint_32) + sizeof(uint_16) + sizeof(uint_32);

    // minimum_instruction_length, default_is_stmt, line_base, line_range
    length += sizeof(uint_8) + sizeof(uint_8) + sizeof(int_8) + sizeof(uint_8);

    // opcode_base, standard_opcode_lengths
    length += sizeof(uint_8) + 0;

    // directories
    for( i = 0; i < _directories->entries(); i += 1 ) {
        length += strlen( (*_directories)[ i ].getString() ) + 1;
    }
    length += sizeof( uint_8 );     // terminating null for directories

    // files
    for( i = 0; i < _files->entries(); i += 1 ) {
        length += (*_files)[ i ].getLen();
    }
    length += sizeof( uint_8 );     // terminating null for directories

    outFile->writeDWord( length - sizeof(uint_32) );
    outFile->writeWord( 2 );
    outFile->writeDWord( length - 4 - 2 - 4 );          // subtract header

    outFile->writeByte( 0 );        // miminum_instruction_length
    outFile->writeByte( 0 );        // default_is_stmt
    outFile->writeByte( 0 );        // line_base
    outFile->writeByte( 0 );        // line_range
    outFile->writeByte( 1 );        // opcode_base

    for( i = 0; i < _directories->entries(); i += 1 ) {
        outFile->writeString( (*_directories)[ i ].getString() );
    }
    outFile->writeByte( 0 );    // terminate paths

    for( i = 0; i < _files->entries(); i += 1 ) {
        outFile->writeString( (*_files)[ i ].name.getString() );
        outFile->writeULEB128( (*_files)[ i ].directoryIdx );
        outFile->writeULEB128( (*_files)[ i ].modTime );
        outFile->writeULEB128( (*_files)[ i ].length );
    }
    outFile->writeByte( 0 );    // terminate files
}
