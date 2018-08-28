/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Links to external files
* The database table is an array of filenames, each preceded by a length byte
*
****************************************************************************/

#ifndef EXTFILES_INCLUDED
#define EXTFILES_INCLUDED

#include <cstdio>
#include <map>
#include <string>
#include "errors.hpp"


class OutFile;      // forward reference

class ExternalFiles {
public:
    ExternalFiles() : _bytes( 0 ) { };
    //need to get index by name
    byte index( std::wstring& key ) { return _table[key]; };
    //add the name of an external database to the collection
    void addFile( std::wstring& str );
    //assign indexes to each entry
    void convert();
    //get the number of bytes written by the collection
    dword length() const { return _bytes; };
    //get the number of elements in the collection
    dword size() const { return static_cast< dword >( _table.size() ); };
    dword write( OutFile* out );
private:
    ExternalFiles( const ExternalFiles& rhs );              //no copy
    ExternalFiles& operator=( const ExternalFiles& rhs );   //no assignment

    std::map< std::wstring, byte >  _table;
    typedef std::map< std::wstring, byte >::iterator TableIter;
    typedef std::map< std::wstring, byte >::const_iterator ConstTableIter;
    dword                           _bytes;     //total length when written to disk
};

#endif //EXTFILES_INCLUDED

