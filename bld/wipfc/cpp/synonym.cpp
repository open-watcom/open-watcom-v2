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
* Description:  A synonym entry
*
****************************************************************************/

#include "synonym.hpp"
#include "errors.hpp"

void Synonym::write( std::FILE* out )
{
    offset = std::ftell( out );
    unsigned short int size( 1 );
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr )
        size += static_cast< unsigned short int >( itr->size() );
    if( std::fwrite( &size, sizeof( unsigned short int ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    for( SynonymIter itr = synonyms.begin(); itr != synonyms.end(); ++itr ) {
        unsigned char length( static_cast< unsigned char >( itr->size() ) );
        if( std::fputc( length, out ) == EOF ||
            std::fwrite( itr->data(), sizeof( unsigned char ), length, out ) != length )
            throw FatalError( ERR_WRITE );
    }
    if( std::fputc( '\0', out ) == EOF )
        throw FatalError( ERR_WRITE );
}
