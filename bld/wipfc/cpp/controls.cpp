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
* Description:  Panel Controls data
*
****************************************************************************/

#include "controls.hpp"
#include "errors.hpp"

STD1::uint32_t Controls::write( std::FILE* out )
{
    STD1::uint32_t start( std::ftell( out ) );
    STD1::uint16_t value( static_cast< STD1::uint16_t >( controls.size() ) );
    if( std::fwrite( &value, sizeof( STD1::uint16_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    value = static_cast< STD1::uint16_t >( groups.size() );
    if( std::fwrite( &value, sizeof( STD1::uint16_t ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &coverGroup, sizeof( STD1::uint16_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    value = 0;                              //reserved word
    if( std::fwrite( &value, sizeof( STD1::uint16_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    for( ConstControlIter itr = controls.begin(); itr != controls.end(); ++itr ) {
        bytes += itr->write( out );
    }
    for( ConstGroupIter itr = groups.begin(); itr != groups.end(); ++itr ) {
        bytes += itr->write( out );
    }
    return start;
}
/***************************************************************************/
ControlButton* Controls::getButtonById( const std::wstring& i )
{
    for( ControlIter itr = controls.begin(); itr != controls.end(); ++itr ) {
        if( itr->id() == i )
            return &(*itr);
    }
    return 0;
}
/***************************************************************************/
ControlGroup* Controls::getGroupById( const std::wstring& i )
{
    for( GroupIter itr = groups.begin(); itr != groups.end(); ++itr ) {
        if( itr->id() == i )
            return &(*itr);
    }
    return 0;
}

