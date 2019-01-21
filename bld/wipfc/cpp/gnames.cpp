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
* Description:  Global Names data
* Obtained from the "id" or "name" attribute of an :hn tag iff the "global"
* attribute flag is set
* word dictIndex[ IpfHeader.panelCount ]; //in ascending order
* word TOCIndex[ IpfHeader.panelCount ];
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdio>
#include "gnames.hpp"
#include "errors.hpp"
#include "outfile.hpp"


void GNames::insert( GlobalDictionaryWord* gdentry, word toc )
{
    NameIter itr( _names.find( gdentry ) );   //look up word in names
    if( itr != _names.end() )
        throw Class3Error( ERR3_DUPID );
    _names.insert( std::map< GlobalDictionaryWord*, word, ptrLess< GlobalDictionaryWord* > >::value_type( gdentry, toc ) );
}
/***************************************************************************/
dword GNames::write( OutFile* out ) const
{
    dword start( 0 );
    if( _names.size() ) {
        start = out->tell();
        for( ConstNameIter itr = _names.begin(); itr != _names.end(); ++itr ) {
            // name index
            if( out->put( itr->first->index() ) ) {
                throw FatalError( ERR_WRITE );
            }
        }
        for( ConstNameIter itr = _names.begin(); itr != _names.end(); ++itr ) {
            // TOC index
            if( out->put( itr->second ) ) {
                throw FatalError( ERR_WRITE );
            }
        }
    }
    return start;
}

