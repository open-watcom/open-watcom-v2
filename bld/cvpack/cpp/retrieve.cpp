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


#include <stdlib.h>
#include "common.hpp"
#include "retrieve.hpp"

unsigned_8 Directory::_orderTable[] = {
    1, 2, 6, 6, 4, 0xff, 0xff,
    3, 7, 0xff, 0xff, 0xff, 0xff,
    5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

// unused function section :
bool cv_dir_entry::operator == ( const cv_dir_entry& ) const {
    throw InternalError("cv_dir_entry == gets call.");
}
// unused function section end.

char Retriever::_inputBuffer[DEF_BUF_SIZE];

//
// Directory::Directory().
// Constructs a Directory object that contains the header and entry infos
// by reading from "infile".  The file pointer of the directory
// has to be provided.
//
Directory::Directory(ifstream&  inFile,
                     streampos  directoryPos)
        : _cvDirEntry (0),
          _currentSub (0)
/*******************************************/
{
    inFile.seekg(directoryPos);
    inFile.read( (unsigned char *) &_cvDirHeader,sizeof(cv_subsection_directory));
    if ( inFile.ios::fail() ) {
        throw DebugInfoError();
    }
    _cvDirEntry = new cv_dir_entry[_cvDirHeader.cDir];
    inFile.read( (unsigned char *) _cvDirEntry,
                 sizeof(cv_directory_entry)*_cvDirHeader.cDir);
    // sort the directory into prefer reading order.
    qsort(_cvDirEntry,_cvDirHeader.cDir,sizeof(cv_directory_entry),Directory::Compare);
    if ( inFile.ios::fail() ) {
        throw DebugInfoError();
    }
}

int Directory::CompMod( const module mod1, const module mod2 )
/************************************************************/
{
    if ( mod1 < mod2 ) {
        return -1;
    }
    if ( mod1 > mod2 ) {
        return 1;
    }
    return 0;
}

int Directory::CompSub( const unsigned_16 sub1, const unsigned_16 sub2 )
/**********************************************************************/
{
    if ( _orderTable[sub1-sstModule] <
         _orderTable[sub2-sstModule] ) {
        return -1;
    }
    if ( _orderTable[sub1-sstModule] >
         _orderTable[sub2-sstModule] ) {
        return 1;
    }
    return 0;
}

bool Directory::IsModule( const unsigned_16 sub )
/***********************************************/
{
    return ( sub == sstTypes ||
             sub == sstSymbols ||
             sub == sstSrcModule );
}

bool Directory::IsModuleBasis( const unsigned_16 sub1,
                               const unsigned_16 sub2 )
/*****************************************************/
{
    return ( IsModule(sub1) && IsModule(sub2) );
}

int Directory::Compare( const void* ptr1, const void* ptr2 )
/**********************************************************/
{
    const cv_dir_entry* cvDirEntry1 = ( cv_dir_entry * ) ptr1;
    const cv_dir_entry* cvDirEntry2 = ( cv_dir_entry * ) ptr2;
    int   retVal;
    if ( IsModuleBasis(cvDirEntry1->subsection,cvDirEntry2->subsection) ) {
        retVal = CompMod(cvDirEntry1->iMod,cvDirEntry2->iMod);
        return retVal ? retVal : CompSub(cvDirEntry1->subsection,cvDirEntry2->subsection);
    }
    retVal = CompSub(cvDirEntry1->subsection,cvDirEntry2->subsection);
    if ( retVal ) {
        return retVal;
    }
    retVal = CompMod(cvDirEntry1->iMod,cvDirEntry2->iMod);
    return retVal ? retVal : 0;
}

bool Directory::GetDirInfo( dir_info&    di,
                            const sst    subsection,
                            const module mod )
/**************************************************/
{
    if ( _cvDirEntry[_currentSub].subsection == subsection &&
         _cvDirEntry[_currentSub].iMod == mod ) {
        di.offset = _cvDirEntry[_currentSub].lfo;
        di.length = _cvDirEntry[_currentSub].cb;
        _currentSub++;
        return TRUE;
    }
    return FALSE;
}

bool Directory::IsAtSubsection( const sst subsection )
/****************************************************/
{
    return _cvDirEntry[_currentSub].subsection == subsection;
}

//
// Directory::~Directory().
//
Directory::~Directory()
/*********************/
{
    delete [] _cvDirEntry;
}

//
// Retriever::GetBasePos().
// Returns the base address of the debugging informations.
// Assumes OMF style.
//
streampos Retriever::GetBasePos()
/*******************************/
{
    unsigned_32 lfoBase;

    _inputFile.seekg(-LONG_WORD, ios::end);
    _inputFile.read( (char *) &lfoBase,LONG_WORD);
    _inputFile.seekg(-lfoBase, ios::end);
    if ( _inputFile.ios::fail() ) {
        throw DebugInfoError();
    }
    return (_inputFile.tellg());
}

//
// Retriever::GetDirPos().
// Returns the address of the directory informations.
//
streampos Retriever::GetDirPos()
/******************************/
{
    unsigned_32 lfoDir;

    if ( ! SeekRead(&lfoDir, _lfaBase + LONG_WORD, LONG_WORD) ) {
        throw DebugInfoError();
    }
    return (_lfaBase+lfoDir);
}

//
// Retriever::SeekRead().
// Seek to a specify point in the file and read some data.
//
unsigned_32 Retriever::SeekRead( void*     buffer,
                                 streampos position,
                                 size_t    length )
/**************************************************/
{
    _inputFile.seekg(position);
    if ( _inputFile.ios::fail() ) {
        throw DebugInfoError();
    }
    _inputFile.read( (char *) buffer, length);
    if ( _inputFile.ios::rdstate() == ios::badbit ) {
        throw DebugInfoError();
    }
    if ( _inputFile.ios::rdstate() == ios::failbit ) {
        if ( _inputFile.gcount() != length ) {
            _inputFile.clear();
            return _inputFile.gcount();
        }
        throw DebugInfoError();
    }
    return length;
}

//
// Retriever::Retriever().
// Constructs a Retriever object that is attached to inputStream.
//
Retriever::Retriever( ifstream& inputStream ) :
                  _inputFile ( inputStream.fstreambase::fd() ),
                  _lfaBase   ( GetBasePos() ),
                  _aDirectory(_inputFile, GetDirPos()),
                  _pageStartOffset(0),
                  _heapBuffer( NULL ),
                  _missRate(0)
/*************************************************************/
{
    ReadPage(0);
    CheckSig();
}

//
// Retriever::CheckSig().
// Checks the validness of the CV signature.
//
void Retriever::CheckSig()
/************************/
{
    if ( strncmp(_inputBuffer,NB09,4) == 0 ) {
        throw MiscError("file already packed.");
    }
    // NB05 is the only version that we support.
    if ( strncmp(_inputBuffer,NB05,4) != 0 ) {
        throw CVSigError();
    }
}

unsigned_32 Retriever::LocalPageOff( const unsigned_32 offset ) const
/*******************************************************************/
{
    return ( offset-_pageStartOffset );
}

bool Retriever::IsCrossPage( const dir_info& di ) const
/*****************************************************/
{
    return ( LocalPageOff(di.offset) + di.length > DEF_BUF_SIZE );
}

void Retriever::ReadPage( const unsigned_32 offset )
/**************************************************/
{
    if ( ! _inputFile.eof() ) {
        SeekRead(_inputBuffer,_lfaBase+offset,DEF_BUF_SIZE);
        _pageStartOffset = offset;
    }
}

bool Retriever::IsInCurrentPage( const unsigned_32 offset ) const
/***************************************************************/
{
    long diff = offset-_pageStartOffset;
    return ( diff >= 0 && diff <= DEF_BUF_SIZE );
}

char* Retriever::Read( const dir_info& di )
/*****************************************/
{
    // if length larger than one page, then do a direct read and read in
    // the subsequent page onto input buffer.
    if ( di.length > DEF_BUF_SIZE ) {
        _heapBuffer = new char [di.length];
        if ( SeekRead(_heapBuffer, _lfaBase+di.offset, di.length) != di.length ) {
            delete [] _heapBuffer;
            return NULL;
        }
        ReadPage(di.offset+di.length);
        return _heapBuffer;
    }
    ReadPage(di.offset);
    return _inputBuffer;
}

// The read subsection routine is most efficient when debugging info emitted
// by the linker are in the order that cvpack reads.
bool Retriever::ReadSubsection( char*&       buffer,
                                unsigned_32& length,
                                const sst    subsection,
                                const module mod )
/******************************************************/
{
    // release last used heap buffer, if there is one.
    // C++ guaranteeds delete NULL without error.
    delete [] _heapBuffer;
    dir_info di;
    if ( !_aDirectory.GetDirInfo(di,subsection,mod) ) {
        return FALSE;
    }
    length = di.length;
    if ( length == 0 ) {
        return FALSE;
    }
    if ( _missRate > DEF_MISS_THRESHOLD ) {
        _heapBuffer = buffer = new char [di.length];
        if ( SeekRead( buffer, _lfaBase+di.offset, di.length ) != di.length ) {
            delete [] _heapBuffer;
            return FALSE;
        }
        return TRUE;
    }
    // if the request read is not in current page, readin the request
    // page.
    if ( ! IsInCurrentPage(di.offset) ) {
        ++_missRate;
        buffer = Read(di);
        return ( buffer ? TRUE : FALSE );
    }
    if ( IsCrossPage(di) ) {
        buffer = Read(di);
        return ( buffer ? TRUE : FALSE );
    }
    buffer = _inputBuffer + LocalPageOff(di.offset);
    return TRUE;
}

bool Retriever::IsAtSubsection( const sst subsection )
/****************************************************/
{
    return _aDirectory.IsAtSubsection( subsection );
}
