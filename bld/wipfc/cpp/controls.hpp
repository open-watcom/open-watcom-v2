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

#ifndef CONTROLS_INCLUDED
#define CONTROLS_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include "ctrlbtn.hpp"
#include "ctrlgrp.hpp"

class Controls {
public:
    Controls() : bytes( 8 ), coverGroup( 0 ) { };
    ~Controls() { };
    void addButton( ControlButton& btn )
    {
        btn.setIndex( static_cast< STD1::uint16_t >( controls.size() + 7 ) );
        controls.push_back( btn );
    };
    ControlButton* button() { return &controls[ controls.size() - 1 ]; };
    ControlButton* getButtonById( const std::wstring& i );
    void addGroup( ControlGroup& grp )
    {
        groups.push_back( grp );
        grp.setIndex( static_cast< STD1::uint16_t >( groups.size() ) );
    };
    ControlGroup* group() { return &groups[ groups.size() - 1 ]; };
    ControlGroup* getGroupById( const std::wstring& i );
    void setCover( STD1::uint16_t c ) { coverGroup = c; };
    STD1::uint32_t length() const { return bytes; };
    STD1::uint32_t write( std::FILE *out );
private:
    Controls( const Controls& rhs );            //no copy
    Controls& operator=( const Controls& rhs ); //no assignment
    std::vector< ControlButton > controls;
    typedef std::vector< ControlButton >::iterator ControlIter;
    typedef std::vector< ControlButton >::const_iterator ConstControlIter;
    std::vector< ControlGroup > groups;
    typedef std::vector< ControlGroup >::iterator GroupIter;
    typedef std::vector< ControlGroup >::const_iterator ConstGroupIter;
    STD1::uint32_t bytes;       //size of all controls together
    STD1::uint16_t coverGroup;
};

#endif //CONTROLS_INCLUDED
