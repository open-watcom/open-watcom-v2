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
* Description:  Dictionary (vocabulary) data
*
****************************************************************************/

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include "gdict.hpp"
#include "errors.hpp"

GlobalDictionary::~GlobalDictionary()
{
    for( WordIter iter = words.begin(); iter != words.end(); ++iter)
        delete *iter;
}    
/***************************************************************************/
GlobalDictionaryWord* GlobalDictionary::insert( GlobalDictionaryWord * word)
{
    std::pair< WordIter, bool > status( words.insert( word ) );
    if( !status.second )
        delete word;
    return *status.first;
}
/***************************************************************************/
GlobalDictionaryWord* GlobalDictionary::insert( std::wstring& word )
{
    GlobalDictionaryWord* tmp( new GlobalDictionaryWord( word ) );
    return insert( tmp );
}
/***************************************************************************/
// Call after parsing, but before local dictionary construction
void GlobalDictionary::convert( size_t count )
{
    STD1::uint16_t index( 0 );
    for( WordIter iter = words.begin(); iter != words.end(); ++iter, ++index) {
        (*iter)->setIndex( index );
        (*iter)->setPages( count );
    }
}
/***************************************************************************/
STD1::uint16_t GlobalDictionary::findIndex( std::wstring& word )
{
    GlobalDictionaryWord tmp( word );
    return findIndex( &tmp );
}
/***************************************************************************/
GlobalDictionaryWord* GlobalDictionary::findWord( std::wstring& word )
{
    GlobalDictionaryWord tmp( word );
    WordIter wIter( words.find( &tmp ) );   // find the pointer in the set
    return *wIter;
}
/***************************************************************************/
STD1::uint32_t GlobalDictionary::write( std::FILE *out )
{
    STD1::uint32_t start( std::ftell( out ) );
    for( ConstWordIter itr = words.begin(); itr != words.end(); ++itr )
        bytes += (*itr)->writeWord( out );
    return start;
}
/***************************************************************************/
bool GlobalDictionary::buildFTS()
{
    bool big( false );
    for( ConstWordIter itr = words.begin(); itr != words.end(); ++itr ) {
        (*itr)->buildFTS();
        if( (*itr)->bigFTS() )
            big = true;
    }
    return big;
}
/***************************************************************************/
STD1::uint32_t GlobalDictionary::writeFTS( std::FILE *out, bool big )
{
    STD1::uint32_t start( std::ftell( out ) );
    for( ConstWordIter itr = words.begin(); itr != words.end(); ++itr )
        ftsBytes += (*itr)->writeFTS( out, big );
    return start;
}

