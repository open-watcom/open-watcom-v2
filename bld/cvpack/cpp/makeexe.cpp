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


#include <iostream.h>
#include <wclistit.h>
#include "common.hpp"
#include "cverror.hpp"
#include "symdis.hpp"
#include "makeexe.hpp"

void ODirectory::Insert( const sst         subsection,
                         const unsigned_16 module,
                         const unsigned_32 offset,
                         const unsigned_32 length )
{
    ++_dirHeader.cDir;
    cv_dir_entry* newEntry = new cv_dir_entry;
    newEntry->subsection = subsection;
    newEntry->iMod = module;
    newEntry->lfo = offset;
    newEntry->cb = length;
    _dirEntry.append(newEntry);
}

void ODirectory::Put( ExeMaker& eMaker ) const
{
    eMaker.DumpToExe( &_dirHeader, sizeof(cv_subsection_directory) );
/*    eMaker.DumpToExe( (unsigned_16) sizeof(cv_subsection_directory));
    eMaker.DumpToExe( (unsigned_16) sizeof(cv_directory_entry));
    eMaker.DumpToExe( _cDir );
   eMaker.DumpToExe( (unsigned_32) 0L );
    eMaker.DumpToExe( (unsigned_32) 0L ); */
    WCPtrConstSListIter<cv_dir_entry> iter(_dirEntry);
    while ( ++iter ) {
        eMaker.DumpToExe( iter.current(),sizeof(cv_directory_entry) );
    }
}

/*ExeMaker::ExeMaker( const char* tmpName )
        : _outFile(tmpName,ios::out | ios::binary) {
    _outFile.seekp(+0L, ios::beg); // optional code.
}

CVpack::CVpack( ifstream& inFile, const char* tmpName )
        : _currentOffset(0),
          _lfaBase(0),
          _eMaker(tmpName),
          _aRetriever(inFile) { } */

void ExeMaker::Write( const char*     buffer,
                      const size_t    length )
/*******************************************/
{
    if ( length > 0 ) {
        _outFile.write(buffer, length);
        if ( _outFile.ios::fail() ) {
            throw WriteError(NULL);
        }
    }
}
