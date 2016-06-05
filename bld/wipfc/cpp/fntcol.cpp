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
* Description:  Font data
*
****************************************************************************/

#include "fntcol.hpp"
#include "errors.hpp"

FontCollection::FontCollection( int cp ) : bytes( 0 )
{
    fonts.reserve( 14 );
    FontEntry fnt;
    std::strncpy( fnt.faceName, "System Proportional", sizeof( fnt.faceName ));
    fnt.height = 0;
    fnt.width = 0;
    fnt.codePage = cp;
    try {
        add( fnt );                 //set the default font
    }
    catch( Class2Error &e ) {
    }
}
/****************************************************************************/
size_t FontCollection::add( FontEntry& fnt )
{
    size_t index( 0 );
    for( ConstFontIter itr = fonts.begin(); itr != fonts.end(); ++itr, ++index ) {
        if( *itr == fnt )
            return index;
    }
    if( fonts.size() < 15 )
        fonts.push_back( fnt );
    else
        throw Class2Error( ERR2_FONTS );
    return fonts.size() - 1;
}
/****************************************************************************/
STD1::uint32_t FontCollection::write( std::FILE *out )
{
    STD1::uint32_t start( std::ftell( out ) );
    for( FontIter itr = fonts.begin(); itr != fonts.end(); ++itr ) {
        bytes += itr->write( out );
    }
    return start;

}
