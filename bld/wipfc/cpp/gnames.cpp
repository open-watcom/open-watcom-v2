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
* Description:  Global Names data
* Obtained from the "id" or "name" attribute of an :hn tag iff the "global"
* attribute flag is set
* STD1::uint16_t dictIndex[ IpfHeader.panelCount ]; //in ascending order
* STD1::uint16_t TOCIndex[ IpfHeader.panelCount ];
*
****************************************************************************/

#include <cstdio>
#include "gnames.hpp"
#include "errors.hpp"

void GNames::insert( GlobalDictionaryWord* word, STD1::uint16_t toc )
{
    NameIter itr( names.find( word ) );   //look up word in names
    if( itr == names.end() )
        names.insert( std::map< GlobalDictionaryWord*, STD1::uint16_t, ptrLess< GlobalDictionaryWord* > >::value_type( word, toc ) );
    else
        throw Class3Error( ERR3_DUPID );
}
/***************************************************************************/
STD1::uint32_t GNames::write( std::FILE *out ) const
{
    STD1::uint32_t start( 0 );
    if( names.size() ) {
        start = std::ftell( out );
        for( ConstNameIter itr = names.begin(); itr != names.end(); ++itr ) {
            STD1::uint16_t index = (itr->first)->index();
            if( std::fwrite( &index, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
        for( ConstNameIter itr = names.begin(); itr != names.end(); ++itr ) {
            STD1::uint16_t toc = itr->second;
            if( std::fwrite( &toc, sizeof( STD1::uint16_t ), 1, out ) != 1 )
                throw FatalError( ERR_WRITE );
        }
    }
    return start;
}

