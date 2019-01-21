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
* Description:  Describes a group of panel controls
*
****************************************************************************/

#ifndef CONTROLGROUP_INCLUDED
#define CONTROLGROUP_INCLUDED

#include <cstdio>
#include <string>
#include <vector>


class OutFile;

class ControlGroup {
public:
    ControlGroup() : _index( 0 ) {};
    ControlGroup( const std::wstring& id ) : _idnt( id ), _index( 0 ) { };
    void addButtonIndex( word i ) { _buttonIndex.push_back( i ); };
    void setId( const std::wstring& i ) { _idnt = i; };
    const std::wstring& id() const { return _idnt; };
    void setIndex( word i ) { _index = i; };
    word index() const { return _index; };
    dword write( OutFile* out ) const;
private:
    std::vector< word > _buttonIndex;
    typedef std::vector< word >::iterator ButtonIter;
    typedef std::vector< word >::const_iterator ConstButtonIter;
    std::wstring        _idnt;      //identifier
    word                _index;     //index of this item in group array
};

#endif //CONTROLGROUP_INCLUDED
