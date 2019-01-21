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
* Description:  Panel Controls data
*
****************************************************************************/


#include "wipfc.hpp"
#include "controls.hpp"
#include "errors.hpp"
#include "outfile.hpp"


dword Controls::write( OutFile* out )
{
    dword start( out->tell() );
    if( out->put( static_cast< word >( _controls.size() ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( static_cast< word >( _groups.size() ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _coverGroup ) )
        throw FatalError( ERR_WRITE );
    //reserved word 0
    if( out->put( static_cast< word >( 0 ) ) )
        throw FatalError( ERR_WRITE );
    for( ConstControlIter itr = _controls.begin(); itr != _controls.end(); ++itr ) {
        _bytes += itr->write( out );
    }
    for( ConstGroupIter itr = _groups.begin(); itr != _groups.end(); ++itr ) {
        _bytes += itr->write( out );
    }
    return start;
}
/***************************************************************************/
ControlButton* Controls::getButtonById( const std::wstring& id )
{
    for( ControlIter itr = _controls.begin(); itr != _controls.end(); ++itr ) {
        if( itr->id() == id ) {
            return &(*itr);
        }
    }
    return 0;
}
/***************************************************************************/
ControlGroup* Controls::getGroupById( const std::wstring& id )
{
    for( GroupIter itr = _groups.begin(); itr != _groups.end(); ++itr ) {
        if( itr->id() == id ) {
            return &(*itr);
        }
    }
    return 0;
}

