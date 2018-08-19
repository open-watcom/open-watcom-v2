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
* Description:  A synonym entry
*
****************************************************************************/


#include "wipfc.hpp"
#include "synonym.hpp"
#include "errors.hpp"
#include "outfile.hpp"


void Synonym::write( OutFile* out )
{
    // convert wide vector _synonyms to mbcs vector synonyms
    std::vector< std::string > synonyms;
    for( SynonymWIter itr = _synonyms.begin(); itr != _synonyms.end(); ++itr ) {
        std::string buffer( out->wtomb_string( *itr ) );
        if( buffer.size() > 255 )
            buffer.erase( 255 );
        synonyms.push_back( buffer );
    }
    // process mbcs vector
    _offset = out->tell();
    std::size_t size = 1;
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr )
        size += itr->size();
    if( out->put( static_cast< word >( size ) ) )
        throw FatalError( ERR_WRITE );
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr ) {
        byte length( static_cast< byte >( itr->size() ) );
        if( out->put( length ) )
            throw FatalError( ERR_WRITE );
        if( out->put( *itr ) ) {
            throw FatalError( ERR_WRITE );
        }
    }
    if( out->put( static_cast< byte >( '\0' ) ) ) {
        throw FatalError( ERR_WRITE );
    }
}
