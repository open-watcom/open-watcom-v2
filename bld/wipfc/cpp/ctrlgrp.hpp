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
* Description:  Describes a group of panel controls
*
****************************************************************************/

#ifndef CONTROLGROUP_INCLUDED
#define CONTROLGROUP_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include <vector>

class ControlGroup {
public:
    ControlGroup() : idx( 0 ) {};
    ControlGroup( const std::wstring& id ) : idnt( id ), idx( 0 ) { };
    void addButtonIndex( STD1::uint16_t i ) { buttonIndex.push_back( i ); };
    void setId( const std::wstring& i ) { idnt = i; };
    const std::wstring& id() const { return idnt; };
    void setIndex( STD1::uint16_t i ) { idx = i; };
    STD1::uint16_t index() const { return idx; };
    STD1::uint32_t write( std::FILE *out ) const;
private:
    std::vector< STD1::uint16_t > buttonIndex;
    typedef std::vector< STD1::uint16_t >::iterator ButtonIter;
    typedef std::vector< STD1::uint16_t >::const_iterator ConstButtonIter;
    std::wstring idnt;  //identifier
    STD1::uint16_t idx; //index of this item in group array
};

#endif //CONTROLGROUP_INCLUDED
