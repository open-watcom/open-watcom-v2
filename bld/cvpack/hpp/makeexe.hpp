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


#ifndef MAKEEXE_H_INCLUDED
#define MAKEEXE_H_INCLUDED

#include <wclist.h>
#include <iostream.h>
#include "retrieve.hpp"

class ExeMaker;

class ODirectory {

    public :

        ODirectory() {
            _dirHeader.cbDirHeader = sizeof(cv_subsection_directory);
            _dirHeader.cbDirEntry = sizeof(cv_directory_entry);
            _dirHeader.cDir = 0;
            _dirHeader.lfoNextDir = 0;
            _dirHeader.flags = 0;
        }

        ~ODirectory() {
            _dirEntry.clearAndDestroy();
        }

        void Insert( const sst,
                     const unsigned_16,
                     const unsigned_32,
                     const unsigned_32 );

        void Put( ExeMaker& );

    private :


        cv_subsection_directory         _dirHeader;
        WCPtrSList<cv_dir_entry>        _dirEntry;
};


class ExeMaker {

    public :

        ExeMaker::ExeMaker( const char* tmpName )
                : _outFile(tmpName,ios::out | ios::binary) {
            _outFile.seekp(+0L, ios::beg); // optional code.
        }

        ~ExeMaker() { }

        // Dump out a sequence of raw bytes.
        void DumpToExe( const void*  buffer,
                        const size_t length ) {
            Write( (char *) buffer, length );
        }

        // dump out a character.
        void DumpToExe( const unsigned_8 num ) {
            Write( (char *) &num, sizeof(unsigned_8) );
        }

        // dump out a 16 bit integer.
        void DumpToExe( const unsigned_16 num ) {
            Write( (char *) &num, sizeof(unsigned_16) );
        }

        // dump out a 32 bit integer.
        void DumpToExe( const unsigned_32 num ) {
            Write( (char *) &num, sizeof(unsigned_32) );
        }

        // dump out a variant string object.
        void DumpToExe( const VariantString& str ) {
            Write( str._string, str._strLen );
        }

        void Reserve( const uint size ) {
            _outFile.seekp( +size, ios::cur );
        }

        streampos TellPos() {
            return _outFile.tellp();
        }

        void SeekTo( streampos pos ) {
            _outFile.seekp( pos, ios::beg );
        }

        void Close() {
            _outFile.close();
        }

    private :

        void Write( const char*, const size_t );
        ofstream        _outFile;
};
#endif
