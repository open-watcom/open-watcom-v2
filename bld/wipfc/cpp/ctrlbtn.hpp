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
* Description:  Panel control button data
*
****************************************************************************/

#ifndef CONTROLBUTTON_INCLUDED
#define CONTROLBUTTON_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>

class ControlButton {
public:
    ControlButton() : res( 0 ), idx( 0 ) {};
    ControlButton( const std::wstring& i, STD1::uint16_t r, const std::wstring& t ) :
        res( r ), idnt( i ), txt( t ) { };
    void setId( std::wstring& i ) { idnt = i; };
    const std::wstring& id() const { return idnt; };
    void setRes( STD1::uint16_t r ) { res = r; };
    void setText( std::wstring& t ) { txt = t; };
    void setIndex( STD1::uint16_t i ) { idx = i; };
    STD1::uint16_t index() { return idx; };
    STD1::uint32_t write( std::FILE* out ) const;
private:
    STD1::uint16_t res;     //message number
    STD1::uint16_t idx;     //array index of this item
    std::wstring idnt;      //identifier
    std::wstring txt;       //button text
};

#endif //CTRLBUTTON_INCLUDED
