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


#ifndef __MRLINE_H__

#include "brmerge.h"
#include "mrstrhdl.h"
#include "mrfile.h"

class MergeOffset;

class MergeFileRelocKey {
public:
                MergeFileRelocKey() : mbrFile( 0 ), oldIdx( 0 ) {}
                MergeFileRelocKey( uint_8 m, uint_16 o )
                    : mbrFile( m )
                    , oldIdx( o ) {}

                MergeFileRelocKey( const MergeFileRelocKey & o )
                    : mbrFile( o.mbrFile )
                    , oldIdx( o.oldIdx ) {}

                ~MergeFileRelocKey() {}

    bool        operator== ( const MergeFileRelocKey& o ) const {
                    return( mbrFile == o.mbrFile && oldIdx == o.oldIdx );
                }
    unsigned    hashVal() const;
    unsigned    static getHashVal( const MergeFileRelocKey& k ) { return k.hashVal(); }

    uint_8      mbrFile;        // source mbr file
    uint_16     oldIdx;         // old file index
};

class MergeFileEntry {
public:
                    MergeFileEntry();
                    MergeFileEntry( const MergeFileEntry& other );

    bool            operator==( const MergeFileEntry& other ) const;
    MergeFileEntry& operator=( const MergeFileEntry& other );
    uint            getLen();

    MergeStringHdl  name;
    uint_16         directoryIdx;
    uint_32         modTime;
    uint_32         length;
};

class MergeLineSection {
public:
                    MergeLineSection();
                    ~MergeLineSection();
    uint_16         getNewFileIdx( uint_8 mbrFile, uint_16 oldIdx );

    void            scanFile( MergeFile * file, uint_8 idx );
    void            writePass( MergeFile * outFile );

private:
    void            readLineSect( MergeFile * file, MergeOffset& moff );
    void            addDirectory( uint_8, uint_16, MergeStringHdl & );
    void            addFile( uint_8, uint_16, MergeFileEntry& file );
    bool            readFileEntry( MergeFile * file, MergeOffset & moff,
                                    uint_16 oldFileIdx );

    MergeFile *                                 _outFile;

    WCValHashDict<MergeStringHdl, uint_16> *    _directoriesByName;
    WCValHashDict<MergeFileRelocKey, uint_16> * _directoriesReloc;
    WCValOrderedVector<MergeStringHdl> *        _directories;

    WCValHashDict<MergeStringHdl, uint_16> *    _filesByName;
    WCValHashDict<MergeFileRelocKey, uint_16> * _filesReloc;
    WCValOrderedVector<MergeFileEntry> *        _files;
};

#define __MRLINE_H__
#endif
