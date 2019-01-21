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
* Description:  Strings data
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include "strings.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "outfile.hpp"


dword StringTable::write( OutFile* out )
{
    if( _table.empty() )
        return 0L;
    dword start( out->tell() );
    for( ConstTableIter itr = _table.begin(); itr != _table.end(); ++itr ) {
        std::string buffer( out->wtomb_string( *itr ) );
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

