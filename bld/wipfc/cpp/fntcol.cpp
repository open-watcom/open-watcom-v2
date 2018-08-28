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
#include "outfile.hpp"


FontCollection::FontCollection( word maxFontCount ) : _bytes( 0 ), _maxFontCount( maxFontCount )
/**********************************************************************************************/
{
    _fonts.reserve( _maxFontCount );
    try {
        add( FontEntry( L"System Proportional", 0, 0 ) );   //set the default font
    }
    catch( Class2Error &e ) {
        (void)e;
    }
}

std::size_t FontCollection::add( const FontEntry& fnt )
/*****************************************************/
{
    std::size_t index( 0 );
    for( ConstFontIter itr = _fonts.begin(); itr != _fonts.end(); ++itr, ++index ) {
        if( *itr == fnt ) {
            return( index );
        }
    }
    if( _fonts.size() >= _maxFontCount )
        throw Class2Error( ERR2_FONTS );
    _fonts.push_back( fnt );
    return( _fonts.size() - 1 );
}

dword FontCollection::write( OutFile* out )
/*****************************************/
{
    dword start = out->tell();
    for( FontIter itr = _fonts.begin(); itr != _fonts.end(); ++itr ) {
        _bytes += itr->write( out );
    }
    return( start );

}
