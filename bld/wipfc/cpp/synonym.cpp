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
#include "document.hpp"


void Synonym::write( std::FILE* out, Document *document )
{
    // convert wide vector _synonyms to mbcs vector synonyms
    std::vector< std::string > synonyms;
    for( SynonymWIter itr = _synonyms.begin(); itr != _synonyms.end(); ++itr ) {
        std::string txt;
        document->wtomb_string( *itr, txt );
        if( txt.size() > 255 )
            txt.erase( 255 );
        synonyms.push_back( txt );
    }
    // process mbcs vector
    _offset = std::ftell( out );
    word size( 1 );
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr )
        size += static_cast< word >( itr->size() );
    if( std::fwrite( &size, sizeof( word ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr ) {
        byte length( static_cast< byte >( itr->size() ) );
        if( std::fputc( length, out ) == EOF ||
            std::fwrite( itr->data(), sizeof( byte ), length, out ) != length ) {
            throw FatalError( ERR_WRITE );
        }
    }
    if( std::fputc( '\0', out ) == EOF ) {
        throw FatalError( ERR_WRITE );
    }
}
