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
* Description:  An index entry
*
****************************************************************************/

#include <cwctype>
#include "index.hpp"
#include "errors.hpp"
#include "util.hpp"

IndexItem::IndexItem( Type t )
{
    if( t == PRIMARY )
        hdr.primary = 1;
    else if( t == SECONDARY )
        hdr.secondary = 1;
}
/***************************************************************************/
bool IndexItem::operator==( const IndexItem& rhs ) const
{
    if( sortKey.empty() ) {
        if( rhs.sortKey.empty() ) 
            return wstricmp( text.c_str(), rhs.text.c_str() ) == 0;
        else
            return wstricmp( text.c_str(), rhs.sortKey.c_str() ) == 0;
    }
    else {
        if( rhs.sortKey.empty() )
            return wstricmp( sortKey.c_str(), rhs.text.c_str() ) == 0;
        else
            return wstricmp( sortKey.c_str(), rhs.sortKey.c_str() ) == 0;
    }
}
/***************************************************************************/
bool IndexItem::operator==( const std::wstring& rhs ) const
{
    if( sortKey.empty() )
        return wstricmp( text.c_str(), rhs.c_str() ) == 0;
    else
        return wstricmp( sortKey.c_str(), rhs.c_str() ) == 0;
}
/***************************************************************************/
bool IndexItem::operator<( const IndexItem& rhs ) const
{
    if( sortKey.empty() ) {
        if( rhs.sortKey.empty() ) 
            return wstricmp( text.c_str(), rhs.text.c_str() ) < 0;
        else
            return wstricmp( text.c_str(), rhs.sortKey.c_str() ) < 0;
    }
    else {
        if( rhs.sortKey.empty() )
            return wstricmp( sortKey.c_str(), rhs.text.c_str() ) < 0;
        else
            return wstricmp( sortKey.c_str(), rhs.sortKey.c_str() ) < 0;
    }
}
/***************************************************************************/
//return <0 if s < t, 0 if s == t, >0 if s > t
int IndexItem::wstricmp( const wchar_t *s, const wchar_t *t ) const
{
    wchar_t c1( std::towupper( *s ) );
    wchar_t c2( std::towupper( *t ) );
    while( c1 == c2 && c1 != L'\0' ) {
        ++s;
        ++t;
        c1 = std::towupper( *s );
        c2 = std::towupper( *t );
    }
    return( c1 - c2 );
}/***************************************************************************/
//calculate size of text
//write header
//variable length data follows:
//if sortKey bit set
//  char size2                          //size of sortKey text
//  char sortText[size2]                //sort key text
//char indexText[size or size-size2];   //index word [not zero-terminated]
//unsigned long synonyms[synonymCount]; //32 bit file offsets to synonyms referencing this word
size_t IndexItem::write( std::FILE* out )
{
    std::string buffer1;
    std::string buffer2;
    size_t length1( 0 );
    size_t length2( 0 );
    if( hdr.sortKey ) {
        wtombstring( sortKey, buffer1 );
        length1 = buffer1.size();
    }
    wtombstring( text, buffer2 );
    length2 = buffer2.size();
    if( length1 + length2 > 254 ) {
        length2 = length1 > 254 ? 0 : 254 - length1;
    }
    else if( hdr.sortKey )
        hdr.size = static_cast< STD1::uint8_t >( length1 + length2 + 1 );
    else
        hdr.size = static_cast< STD1::uint8_t >( length2 );
    hdr.synonymCount = static_cast< STD1::uint8_t >( synonyms.size() );
    if( std::fwrite( &hdr, sizeof( IndexHeader ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    size_t written( sizeof( IndexHeader ) );
    if( hdr.sortKey ) {
        if( std::fputc( length1, out ) == EOF ||
            std::fwrite( buffer1.data(), sizeof( char ), length1, out ) != length1 )
            throw FatalError( ERR_WRITE );
        written += length1 + 1;
    }
    if( std::fwrite( buffer2.data(), sizeof( char ), length2, out ) != length2 )
        throw FatalError( ERR_WRITE );
    written += length2;
    if( !synonyms.empty() &&
        std::fwrite( &synonyms[0], sizeof( STD1::uint32_t ), synonyms.size(), out ) != synonyms.size() )
        throw FatalError( ERR_WRITE );
    written += synonyms.size() * sizeof( STD1::uint32_t );
    return written;
}
