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


#include "wipfc.hpp"
#include <cstdlib>
#include "extfiles.hpp"
#include "errors.hpp"
#include "outfile.hpp"


void ExternalFiles::addFile( std::wstring& str )
{
    if( _table.find( str ) == _table.end() ) {
        _table.insert( std::map< std::wstring, byte >::value_type( str, 0 ) );
        if( _table.size() > 255 ) {
            throw Class1Error( ERR1_EXTFILESLARGE );
        }
    }
}
/***************************************************************************/
void ExternalFiles::convert()
{
    byte count1( 0 );
    for( TableIter itr = _table.begin(); itr != _table.end(); ++itr, ++count1 ) {
        itr->second = count1;
    }
}
/***************************************************************************/
dword ExternalFiles::write( OutFile* out )
{
    if( _table.empty() )
        return 0;
    dword start( out->tell() );
    for( ConstTableIter itr = _table.begin(); itr != _table.end(); ++itr ) {
        std::string buffer( out->wtomb_string( itr->first ) );
        if( buffer.size() > ( 255 - 1 ) )
            buffer.erase( 255 - 1 );
        byte length( static_cast< byte >( buffer.size() + 1 ) );
        if( out->put( length ) )
            throw FatalError( ERR_WRITE );
        if( out->put( buffer ) )
            throw FatalError( ERR_WRITE );
        _bytes += length;
    }
    return start;
}

