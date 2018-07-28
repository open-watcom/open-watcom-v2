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
* Description:  Font data
*
****************************************************************************/


#include "wipfc.hpp"
#include <string>
#include "fntcol.hpp"
#include "errors.hpp"

FontCollection::FontCollection( STD1::uint16_t cp ) : bytes( 0 )
/**************************************************************/
{
    fonts.reserve( MAX_FONTS );
    try {
        add( FontEntry( L"System Proportional", 0, 0, cp ) );   //set the default font
    }
    catch( Class2Error &e ) {
    }
}

std::size_t FontCollection::add( const FontEntry& fnt )
/*****************************************************/
{
    std::size_t index( 0 );
    for( ConstFontIter itr = fonts.begin(); itr != fonts.end(); ++itr, ++index ) {
        if( *itr == fnt ) {
            return( index );
        }
    }
    if( fonts.size() >= MAX_FONTS )
        throw Class2Error( ERR2_FONTS );
    fonts.push_back( fnt );
    return( fonts.size() - 1 );
}

STD1::uint32_t FontCollection::write( std::FILE *out )
/****************************************************/
{
    STD1::uint32_t start( std::ftell( out ) );
    for( FontIter itr = fonts.begin(); itr != fonts.end(); ++itr ) {
        bytes += itr->write( out );
    }
    return( start );

}
