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

#ifndef CONTROLS_INCLUDED
#define CONTROLS_INCLUDED

#include <cstdio>
#include <string>
#include <vector>
#include "ctrlbtn.hpp"
#include "ctrlgrp.hpp"


class Controls {
public:
    Controls() : _bytes( 8 ), _coverGroup( 0 ) { };
    ~Controls() { };
    void addButton( ControlButton& btn )
    {
        btn.setIndex( static_cast< word >( _controls.size() + 7 ) );
        _controls.push_back( btn );
    };
    ControlButton* button() { return &_controls[ _controls.size() - 1 ]; };
    ControlButton* getButtonById( const std::wstring& id );
    void addGroup( ControlGroup& grp )
    {
        _groups.push_back( grp );
        grp.setIndex( static_cast< word >( _groups.size() ) );
    };
    ControlGroup* group() { return &_groups[ _groups.size() - 1 ]; };
    ControlGroup* getGroupById( const std::wstring& id );
    void setCover( word c ) { _coverGroup = c; };
    dword length() const { return _bytes; };
    dword write( OutFile* out );
private:
    Controls( const Controls& rhs );            //no copy
    Controls& operator=( const Controls& rhs ); //no assignment

    std::vector< ControlButton >    _controls;
    typedef std::vector< ControlButton >::iterator ControlIter;
    typedef std::vector< ControlButton >::const_iterator ConstControlIter;
    std::vector< ControlGroup >     _groups;
    typedef std::vector< ControlGroup >::iterator GroupIter;
    typedef std::vector< ControlGroup >::const_iterator ConstGroupIter;
    dword                           _bytes;         //size of all controls together
    word                            _coverGroup;
};

#endif //CONTROLS_INCLUDED
