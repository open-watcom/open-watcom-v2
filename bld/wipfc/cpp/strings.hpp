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
* Description:  Strings data
*
*   StringTable strings from :docprof objectname, dll, objectinfo attributes
*   DictString StringTable[];
*
****************************************************************************/

#ifndef STRINGS_INCLUDED
#define STRINGS_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include <vector>

class StringTable {
public:
    StringTable() : bytes( 0 ) { table.reserve( 3 ); };
    void addString( std::wstring& str ) { table.push_back( str ); };
    //the number of bytes written to disk
    STD1::uint32_t length() const { return bytes; };
    STD1::uint32_t write( std::FILE *out );
private:
    std::vector< std::wstring > table;
    typedef std::vector< std::wstring >::iterator TableIter;
    typedef std::vector< std::wstring >::const_iterator ConstTableIter;
    STD1::uint32_t bytes;
};

#endif //STRINGS_INCLUDED
